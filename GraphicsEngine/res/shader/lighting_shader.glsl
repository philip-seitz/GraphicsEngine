#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Normal;

out vec3 v_Normal;
out vec3 v_FragmentPosition;		// what coordinates in world space does this each fragment correspond to
out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat3 u_Normal_Matrix;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * a_Position;
	v_Normal = u_Normal_Matrix * a_Normal;
	v_FragmentPosition = vec3(u_Model * a_Position);
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 330 core
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
	sampler2D diffuse;		// also used as ambient
	sampler2D specular;
	sampler2D emissive;
	float shininess;
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

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 view_dir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 view_dir);

#define NR_POINTS_LIGHTS 4

in vec3 v_Normal;
in vec3 v_FragmentPosition;
in vec3 v_LightPosition;
in vec2 v_TexCoord;

uniform mat4 u_View;
uniform vec3 u_Object_Color;
uniform vec3 u_Light_Color;
uniform vec3 u_CameraPosition;
uniform Material u_Material;
uniform Light u_Light;
uniform PointLight u_PointLight[NR_POINTS_LIGHTS];
uniform DirectionalLight u_DirLight;
uniform SpotLight u_SpotLight;

void main()
{
	vec3 color = vec3(0.0f);		

	vec3 normal = normalize(v_Normal);
	vec3 view_direction = -normalize(v_FragmentPosition - u_CameraPosition);

	for (int i = 0; i < NR_POINTS_LIGHTS; i++)
	{
		color += CalcPointLight(u_PointLight[i], normal, view_direction);
	}
	// color += CalcDirectionalLight(u_DirLight, normal, view_direction);
	// color += CalcSpotLight(u_SpotLight, normal, view_direction); 

	o_Color = vec4(color, 1.0f);
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir)
{
	vec3 diffuseMap = texture(u_Material.diffuse, v_TexCoord).xyz;
	vec3 specularMap = texture(u_Material.specular, v_TexCoord).xyz;
	vec3 emissionMap = texture(u_Material.emissive, v_TexCoord).xyz;

	// diffuse calculation
	vec3 light_direction = -normalize(light.direction.xyz);
	float diffuseValue = max(dot(normal, light_direction), 0.0f);

	// specular calculation
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specularValue = pow(max(dot(view_dir, reflection_direction), 0.0f), u_Material.shininess);

	vec3 ambient = light.ambient * diffuseMap;
	vec3 diffuse = light.diffuse * (diffuseValue * diffuseMap);
	vec3 specular = light.specular * (specularValue * specularMap);
	vec3 emission = emissionMap;		// for glow effects
	vec3 color = ambient + diffuse + specular; 
	return color;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 view_dir)
{
	vec3 diffuseMap = texture(u_Material.diffuse, v_TexCoord).xyz;
	vec3 specularMap = texture(u_Material.specular, v_TexCoord).xyz;
	vec3 emissionMap = texture(u_Material.emissive, v_TexCoord).xyz;
	
	vec3 light_direction = -normalize(v_FragmentPosition - light.position.xyz);	// from object to light source (easier calc)
	float diffuseValue = max(dot(normal, light_direction), 0.0f);
	// light attenuation
	float d = distance(light.position.xyz, v_FragmentPosition);
	float attenuation_factor = 1.0f/(light.constant + light.linear*d + light.quadratic*d*d);
	
	// specular calculation
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specularValue = pow(max(dot(view_dir, reflection_direction), 0.0f), u_Material.shininess);
	
	
	vec3 ambient = light.ambient * attenuation_factor * diffuseMap;
	vec3 diffuse = light.diffuse * attenuation_factor * (diffuseValue * diffuseMap);
	vec3 specular = light.specular * attenuation_factor * (specularValue * specularMap);
	vec3 emission = emissionMap;		// for glow effects
	return ambient + diffuse + specular; 
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 view_dir)
{
	vec3 diffuseMap = texture(u_Material.diffuse, v_TexCoord).xyz;
	vec3 specularMap = texture(u_Material.specular, v_TexCoord).xyz;
	vec3 emissionMap = texture(u_Material.emissive, v_TexCoord).xyz;

	vec3 light_direction = -normalize(v_FragmentPosition - light.position.xyz);	// from object to light source (easier calc)
	float diffuseValue = max(dot(normal, light_direction), 0.0f);

	// specular calculation
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specularValue = pow(max(dot(view_dir, reflection_direction), 0.0f), u_Material.shininess);

	// light attenuation
	float d = distance(light.position.xyz, v_FragmentPosition);
	float attenuation_factor = 1.0f/(light.constant + light.linear*d + light.quadratic*d*d);

	vec3 spotDirection = -light.direction;
	float theta = dot(light_direction, spotDirection);
	float intensity = (theta - light.outerCutoff)/(light.innerCutoff - light.outerCutoff);
	intensity = clamp(intensity, 0.0f, 1.0f);

	vec3 diffuse = light.diffuse * attenuation_factor * intensity * (diffuseValue * diffuseMap);
	vec3 specular = light.specular * attenuation_factor * intensity * (specularValue * specularMap);
	vec3 ambient = light.ambient * attenuation_factor * intensity * diffuseMap;
	vec3 emission = emissionMap;		// for glow effects

	vec3 color = ambient + diffuse + specular; 
	return color;
}