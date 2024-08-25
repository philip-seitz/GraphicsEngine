#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Normal;

out vec2 v_TexCoord;


void main()
{
	gl_Position = a_Position;
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 460 core
layout(location = 0) out vec4 o_Color;		// color attachment 0
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

// only use one point light for this example
uniform PointLight u_PointLight[1];

// ambience result after ssao and blur
uniform sampler2D u_AmbienceSSAO;

// input from g Buffer
uniform sampler2D g_Position;
uniform sampler2D g_Normal;
uniform sampler2D g_ColorSpec;

void main()
{	

	// get data from g-buffer
	vec3 FragPos = texture(g_Position, v_TexCoord).rgb;
	vec3 Normal = texture(g_Normal, v_TexCoord).rgb;
	vec3 Albedo = texture(g_ColorSpec, v_TexCoord).rgb;

	// get ambience from ssao texture
	float Ambience = texture(u_AmbienceSSAO, v_TexCoord).r;

	// usual lighting calculation
	vec3 viewDir = normalize(-FragPos);
	vec3 lighting = vec3(Albedo * Ambience * 0.3);
	
	// diffuse 
	vec3 lightDir = normalize(u_PointLight[0].position - FragPos);
	vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * u_PointLight[0].diffuse;

	// // specular 
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
	vec3 specular = u_PointLight[0].specular * spec;

	// // attenuation
	float distance = length(u_PointLight[0].position - FragPos);
	float attenuation = 1.0 / (1.0 + u_PointLight[0].linear * distance + u_PointLight[0].quadratic * distance * distance);
	diffuse *= attenuation;
	specular *= attenuation;
	lighting += diffuse + specular;
	o_Color = vec4(lighting, 1.0);
}
