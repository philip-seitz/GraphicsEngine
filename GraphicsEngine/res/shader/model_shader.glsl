#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Normal;

layout(std140) uniform u_Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};

out vec3 v_Normal;
out vec3 v_FragmentPosition;		// what coordinates in world space does this each fragment correspond to
out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat3 u_Normal_Matrix;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * a_Position;
	v_Normal = u_Normal_Matrix * a_Normal;
	v_FragmentPosition = vec3(u_Model * a_Position);		// in world space
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 460 core
layout(location = 0) out vec4 o_Color;

struct DirectionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// attenuation coefficients
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// attenuation coefficients
	float constant;
	float linear;
	float quadratic;

	// spot light parameters 
	vec3 direction;
	float innerCutoff;
	float outerCutoff;

};

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

struct Light
{
	vec4 vector;		// position or direction

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// attenuation coefficients
	float constant;
	float linear;
	float quadratic;

	// spot light parameters 
	vec3 direction;
	float innerCutoff;
	float outerCutoff;
};

vec4 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir);
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 view_dir);
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 view_dir);
vec4 CalcReflection(vec3 normal, vec3 view_direction);
vec4 CalcRefraction(vec3 normal, vec3 view_direction);

float LinearizeDepth(float depth);

in vec3 v_Normal;
in vec3 v_FragmentPosition;
in vec3 v_LightPosition;
in vec2 v_TexCoord;

layout(std140) uniform u_CameraData
{
	vec3 u_CameraPosition;
};

uniform vec3 u_Object_Color;
uniform vec3 u_Light_Color;
uniform Material u_Material;
uniform Light u_Light;
uniform PointLight u_PointLight[5];		// 5 point lights max
uniform DirectionalLight u_DirLight;
uniform SpotLight u_SpotLight;

uniform int u_NumberPointLights;		// number of point lights (uniform)

uniform samplerCube u_Skybox;

float near = 0.5f;
float far = 1000.0f;

void main()
{
	vec4 color = vec4(0.0f);

	vec3 normal = normalize(v_Normal);
	vec3 view_direction = -normalize(v_FragmentPosition - u_CameraPosition);

	// light source should exist, else the color might be substracted and is completely dark
	for (int i = 0; i < u_NumberPointLights; i++)
	{
		// color += CalcPointLight(u_PointLight[i], normal, view_direction);
	}
	color += CalcDirectionalLight(u_DirLight, normal, view_direction);
	// color += CalcSpotLight(u_SpotLight, normal, view_direction); 

	// color = CalcReflection(normal, view_direction);
	// color = CalcRefraction(normal, view_direction);
	color.w = 1.0f;

	// o_Color = vec4(vec3(LinearizeDepth(gl_FragCoord.z)/far), 1.0f);
	o_Color = color;
}

vec4 CalcReflection(vec3 normal, vec3 view_direction)
{
	vec3 reflectedSampleVector = normalize(reflect(-view_direction, normal));		// view direction from fragment to camera
	vec4 color = texture(u_Skybox, reflectedSampleVector);
	return color;
}

vec4 CalcRefraction(vec3 normal, vec3 view_direction)
{
	float ratioAirGlass = 1.0f/1.52f;
	// float ratioGlassAir = 1.52f/1.0f;
	vec3 refractedSampleVector = normalize(refract(-view_direction, normal, ratioAirGlass));
	vec4 color = texture(u_Skybox, refractedSampleVector);
	return color;
}


vec4 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir)
{
	vec4 diffuseMap = texture(u_Material.texture_diffuse1, v_TexCoord);
	vec4 specularMap = texture(u_Material.texture_specular1, v_TexCoord);

	// diffuse calculation
	vec3 light_direction = -normalize(light.direction.xyz);
	float diffuseValue = max(dot(normal, light_direction), 0.0f);

	// specular calculation
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specularValue = pow(max(dot(view_dir, reflection_direction), 0.0f), 32.0f);

	vec4 ambient = vec4(light.ambient, 1.0f) * diffuseMap;
	vec4 diffuse = vec4(light.diffuse, 1.0f) * (diffuseValue * diffuseMap);
	vec4 specular = vec4(light.specular, 1.0f) * (specularValue * specularMap);
	vec4 color = ambient + diffuse + specular;
	return color;
}

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 view_dir)
{
	vec4 diffuseMap = texture(u_Material.texture_diffuse1, v_TexCoord);
	vec4 specularMap = texture(u_Material.texture_specular1, v_TexCoord);

	vec3 light_direction = -normalize(v_FragmentPosition - light.position.xyz);	// from object to light source (easier calc)
	float diffuseValue = max(dot(normal, light_direction), 0.0f);
	// light attenuation
	float d = distance(light.position.xyz, v_FragmentPosition);
	float attenuation_factor = 1.0f / (light.constant + light.linear * d + light.quadratic * d * d);

	// specular calculation
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specularValue = pow(max(dot(view_dir, reflection_direction), 0.0f), 32.0f);

	vec4 ambient = vec4(light.ambient, 1.0f) * attenuation_factor * diffuseMap;
	vec4 diffuse = vec4(light.diffuse, 1.0f) * attenuation_factor * (diffuseValue * diffuseMap);
	vec4 specular = vec4(light.specular, 1.0f) * attenuation_factor * (specularValue * specularMap);
	return ambient + diffuse + specular;
}

vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 view_dir)
{
	vec4 diffuseMap = texture(u_Material.texture_diffuse1, v_TexCoord);
	vec4 specularMap = texture(u_Material.texture_specular1, v_TexCoord);

	vec3 light_direction = -normalize(v_FragmentPosition - light.position.xyz);			// from object to light source (easier calc)
	float diffuseValue = max(dot(normal, light_direction), 0.0f);

	// specular calculation
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specularValue = pow(max(dot(view_dir, reflection_direction), 0.0f), 32.0f);

	// light attenuation
	float d = distance(light.position.xyz, v_FragmentPosition);
	float attenuation_factor = 1.0f / (light.constant + light.linear * d + light.quadratic * d * d);

	vec3 spotDirection = -light.direction;
	float theta = dot(light_direction, spotDirection);
	float intensity = (theta - light.outerCutoff) / (light.innerCutoff - light.outerCutoff);
	intensity = clamp(intensity, 0.0f, 1.0f);

	vec4 diffuse = vec4(light.diffuse, 1.0f) * attenuation_factor * intensity * (diffuseValue * diffuseMap);
	vec4 specular = vec4(light.specular, 1.0f) * attenuation_factor * intensity * (specularValue * specularMap);
	vec4 ambient = vec4(light.ambient, 1.0f) * attenuation_factor * intensity * diffuseMap;

	return ambient + diffuse + specular;
}

float LinearizeDepth(float depth)
{
	float z = depth*2.0f - 1.0f;
	return (2.0f * near * far) / (far + near - z * (far - near));
}