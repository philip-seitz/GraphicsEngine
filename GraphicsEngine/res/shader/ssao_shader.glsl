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
layout(location = 0) out float o_Color;		// color attachment 0
in vec2 v_TexCoord;

layout(std140) uniform u_Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};

// input from g Buffer
uniform sampler2D g_Position;
uniform sampler2D g_Normal;
// noise texture
uniform sampler2D u_NoiseTexture;

uniform float u_ScreenWidth;
uniform float u_ScreenHeight;

// kernel samples
uniform vec3 u_Samples[64];
int u_KernelSize = 64;					// TOOD: use uniform here, so kernel size could be configured on the fly
float u_Radius = 0.5;
float bias = 0.025;

vec2 noiseScale = vec2(u_ScreenWidth / 3.0, u_ScreenHeight / 3.0);

void main()
{
	vec3 fragPos = texture(g_Position, v_TexCoord).xyz;
	vec3 normal = texture(g_Normal, v_TexCoord).rgb;
	vec3 randomVec = texture(u_NoiseTexture, v_TexCoord * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for (int i = 0; i < u_KernelSize; i++)
	{
		vec3 kernel_sample = TBN * u_Samples[i];
		kernel_sample = fragPos + kernel_sample * u_Radius;

		vec4 offset = vec4(kernel_sample, 1.0);
		offset = u_Projection * offset;							// to clip space
		offset.xyz /= offset.w;									// to NDC
		offset.xyz = offset.xyz * 0.5 + 0.5;					// to range {0...1}

		float sampleDepth = texture(g_Position, offset.xy).z;

		float rangeCheck = smoothstep(0.0, 1.0, u_Radius / abs(fragPos.z - sampleDepth));

		// works because in view space (camera -> lookAt) the z axis points away from the target
		occlusion += (sampleDepth >= kernel_sample.z + bias ? 1.0 : 0.0) * rangeCheck;
	}

	float ambient = 1 - occlusion / u_KernelSize;			// higher 
	o_Color = ambient;
}
