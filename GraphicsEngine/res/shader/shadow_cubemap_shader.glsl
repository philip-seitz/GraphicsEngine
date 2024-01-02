#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat3 u_Normal_Matrix;

layout(std140) uniform u_Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};

//layout(std140) uniform u_LightSpaceMatrices
//{
//	mat4 u_LightProjection;
//	mat4 u_LightView;
//};

out VS_OUT{
	vec3 v_FragmentPositionWorldSpace;
	vec3 v_Normal;
	vec2 v_TexCoord;
	// vec4 v_FragmentPositionLightSpace;
} vs_out;
out vec2 v_TexCoord;

void main()
{
	vs_out.v_FragmentPositionWorldSpace = (u_Model * a_Position).xyz;
	// vs_out.v_FragmentPositionLightSpace = u_LightProjection * u_LightView * vec4(vs_out.v_FragmentPositionWorldSpace, 1.0);
	vs_out.v_Normal = u_Normal_Matrix * a_Normal;
	vs_out.v_TexCoord = a_TexCoord;
	gl_Position = u_Projection * u_View * u_Model * a_Position;
}

#shader fragment
#version 460 core
layout(location = 0) out vec4 o_Color;


struct Material
{
	sampler2D texture_diffuse1;		// also used as ambient
	sampler2D texture_diffuse2;
	sampler2D texture_diffuse3;

	sampler2D texture_specular1;
	sampler2D texture_specular2;
	sampler2D texture_specular3;

	// float shininess;
};


vec3 sampleDirections[20] = vec3[]
(
	vec3(1, 1, 1), vec3(-1, 1, 1), vec3(1, -1, 1), vec3(1, 1, -1),
	vec3(-1, -1, 1), vec3(-1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1),
	vec3(0, 1, 1), vec3(1, 0, 1), vec3(1, 1, 0), vec3(-1, 0, 1),
	vec3(-1, 1, 0), vec3(0, 1, -1), vec3(0, -1, 1), vec3(1, 0, -1),
	vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 0, -1), vec3(0, -1, -1)
	);

float ShadowCalculation(vec3 normal, vec3 lightDir);
float LinearizeDepth(float depth);

float near = 10.0f;
float far = 100.0f;

uniform Material u_Material;
uniform vec3 u_LightPosition;
uniform samplerCube u_DepthMap;
uniform float u_FarPlane;

in vec2 v_TexCoord;
in VS_OUT{
	vec3 v_FragmentPositionWorldSpace;
	vec3 v_Normal;
	vec2 v_TexCoord;
	// vec4 v_FragmentPositionLightSpace;
}fs_in;



layout(std140) uniform u_CameraData
{
	vec3 u_CameraPosition;
};

void main()
{
	vec4 lightColor = vec4(1.0);
	vec4 diffuseMap = texture(u_Material.texture_diffuse1, fs_in.v_TexCoord);
	vec4 specularMap = texture(u_Material.texture_specular1, fs_in.v_TexCoord);

	vec3 normal = normalize(fs_in.v_Normal);
	vec3 view_direction = -normalize(fs_in.v_FragmentPositionWorldSpace - u_CameraPosition);
	vec3 light_direction = -normalize(fs_in.v_FragmentPositionWorldSpace - u_LightPosition);	// from object to light source (easier calc)
	vec3 reflection_direction = reflect(-light_direction, normal);

	// specular calculation
	vec3 halfWay = view_direction + light_direction;
	halfWay = normalize(halfWay);
	float specularValue = pow(max(dot(normal, halfWay), 0.0f), 16.0f);

	// diffuse calculation
	float diffuseValue = max(dot(normal, light_direction), 0.0f);

	// shadow calculation 
	float shadow = ShadowCalculation(normal, light_direction);
	vec4 ambient = 0.3 * vec4(diffuseMap.xyz, 1.0);
	vec4 diffuse = (diffuseValue * vec4(diffuseMap.xyz, 1.0)) * (1.0 - shadow);
	vec4 specular = (specularValue * vec4(specularMap.xyz, 1.0)) * (1.0 - shadow);
	o_Color = vec4((lightColor * (ambient + diffuse + specular)).xyz, 1.0);

	// === show depth map using perspective projection matrix
	// vec3 temp = fs_in.v_FragmentPositionLightSpace.xyz / fs_in.v_FragmentPositionLightSpace.w;		// manual perspective divide (not gl_Pos)
	// o_Color = vec4(vec3(LinearizeDepth(temp.z)/far), 1.0);
}

float ShadowCalculation(vec3 normal, vec3 lightDir)
{
	float shadow = 0.0;
	vec3 lightToFragment = fs_in.v_FragmentPositionWorldSpace - u_LightPosition;

	// varying bias depending on light angle to normal
	float bias = max(0.25 * (1.0 - dot(normal, lightDir)), 0.15);
	float viewDistance = length(fs_in.v_FragmentPositionWorldSpace - u_CameraPosition);
	float filterRadius = 0.05;
	float currentDepth = length(lightToFragment);

	// for filtering =====
	for (int i = 0; i < 20; i++)
	{
		float closestDepth = texture(u_DepthMap, lightToFragment + sampleDirections[i] * filterRadius).r * u_FarPlane;
		shadow += currentDepth > (closestDepth + bias) ? 1.0 : 0.0;
	}

	// o_Color = vec4(vec3(closestDepth / u_FarPlane), 1.0);			// display depth cubemap
	return shadow/20.0;
}

float LinearizeDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * near * far) / (far + near - z * (far - near));
}