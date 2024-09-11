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
out vec3 v_FragmentPosition;								// what coordinates in world space does this each fragment correspond to
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

in vec3 v_Normal;
in vec3 v_FragmentPosition;
in vec2 v_TexCoord;

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;			
	vec3 specular;						
	// for this shader I will just use diffuse * 2 (diffuse = 0.5 * color) as the "light color" as per LearnOpenGL

	// attenuation coefficients
	float constant;
	float linear;
	float quadratic;
};

struct Material
{
	sampler2D texture_diffuse1;		// also used as ambient
	sampler2D texture_diffuse2;
	sampler2D texture_diffuse3;

	sampler2D texture_specular1;
	sampler2D texture_specular2;
	sampler2D texture_specular3;
	
	sampler2D texture_metallicness1;

	sampler2D texture_roughness1;

	sampler2D texture_albedo1;

	sampler2D texture_normal1;

	sampler2D texture_ao1;
};


layout(std140) uniform u_CameraData
{
	vec3 u_CameraPosition;
};

const int NR_LIGHTS = 32;
uniform PointLight u_PointLight[NR_LIGHTS];		// lights max
uniform int u_NumberPointLights;

uniform samplerCube u_IrradianceMap;

uniform Material u_Material;

// configurable material settings 
uniform float u_Metallic;
uniform float u_Roughness;

const float PI = 3.14159265359;

// functions for reflectance equation
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 GetNormalFromMap();

void main()
{
	// sample material textures
	//float metallicness = texture(u_Material.texture_metallicness1, v_TexCoord).r;
	float metallicness = u_Metallic;
	// float roughness = texture(u_Material.texture_roughness1, v_TexCoord).r;
	float roughness = u_Roughness;
	// vec3 albedo = pow(texture(u_Material.texture_albedo1, v_TexCoord).rgb, vec3(2.2));		// sRGB to linear color space
	vec3 albedo = vec3(0.6, 0.0, 0.0);
	vec3 normal = GetNormalFromMap();
	float ao = texture(u_Material.texture_ao1, v_TexCoord).r;

	vec3 N = normalize(normal);
	vec3 V = normalize(u_CameraPosition - v_FragmentPosition);

	vec3 F0 = vec3(0.04);

	F0 = mix(F0, albedo, metallicness);

	vec3 Lo = vec3(0.0);
	for (int i = 0; i < u_NumberPointLights; i++)
	{
		// per light radiance
		vec3 L = normalize(u_PointLight[i].position - v_FragmentPosition);
		vec3 H = normalize(V + L);
		float distance = length(u_PointLight[i].position - v_FragmentPosition);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = 2.0 * u_PointLight[i].diffuse * attenuation;				// (2.0 bc of specular) specular is our light color for this (diffuse doesnt work somehow)

		// cook-torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallicness;

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = numerator / max(denominator, 0.001);

		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	vec3 kS = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kD = 1.0 - kS;
	vec3 irradiance = texture(u_IrradianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo;
	vec3 ambient = (kD * diffuse) * ao;
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));

	o_Color = vec4(color, 1.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH; 

	float num = a2; 
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 GetNormalFromMap()
{
	vec3 tangentNormal = texture(u_Material.texture_normal1, v_TexCoord).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(v_FragmentPosition);
	vec3 Q2 = dFdy(v_FragmentPosition);
	vec2 st1 = dFdx(v_TexCoord);
	vec2 st2 = dFdy(v_TexCoord);

	vec3 N = normalize(v_Normal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{

	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);


}