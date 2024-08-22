#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
	gl_Position = a_Position;
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 460 core
layout(location = 0) out vec4 o_Color;
in vec2 v_TexCoord;

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

layout(std140) uniform u_CameraData
{
	vec3 u_CameraPosition;
};

const int NR_LIGHTS = 32;
uniform PointLight u_PointLight[NR_LIGHTS];		// lights max
uniform int u_NumberPointLights;

uniform sampler2D g_Position;
uniform sampler2D g_Normal;
uniform sampler2D g_Colorspec;


void main()
{
	// get data from g-buffer
	vec3 FragPos = texture(g_Position, v_TexCoord).rgb;
	vec3 Normal = texture(g_Normal, v_TexCoord).rgb;
	vec3 Albedo = texture(g_Colorspec, v_TexCoord).rgb;
	float specular = texture(g_Colorspec, v_TexCoord).a;

	// usual lighting calculation
	vec3 viewDir = normalize(u_CameraPosition - FragPos);
	vec3 lighting = Albedo * 0.1;
	for (int i = 0; i < u_NumberPointLights; i++)
	{	
		// diffuse 
		vec3 lightDir = normalize(u_PointLight[i].position - FragPos);
		vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * u_PointLight[i].diffuse;
		lighting += diffuse;

		// // specular 
		// vec3 halfwayDir = normalize(lightDir + viewDir);
		// float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
		// vec3 specular = u_PointLight[i].specular * spec * specular;
		// 
		// // attenuation
		// float distance = length(u_PointLight[i].position - FragPos);
		// float attenuation = 1.0 / (1.0 + u_PointLight[i].linear * distance + u_PointLight[i].quadratic * distance * distance);
		// diffuse *= attenuation;
		// specular *= attenuation;
		// lighting += diffuse + specular;
	} 

	o_Color = vec4(lighting, 1.0);
}