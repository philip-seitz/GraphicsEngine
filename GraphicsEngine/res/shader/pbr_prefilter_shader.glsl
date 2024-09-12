#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;
out vec3 v_FragmentPosition;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	v_FragmentPosition = a_Position.xyz;							// fragment position as sample direction
	gl_Position = u_Projection * u_View * a_Position;				// to capture each cube face from the inside
}

#shader fragment
#version 460 core
layout(location = 0) out vec4 o_Color;
in vec3 v_FragmentPosition;

uniform samplerCube u_EnvironmentMap;
uniform float u_Roughness;
uniform float u_Resolution;

const float PI = 3.14159265359;

// function declarations
float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);

void main()
{
    // approximation as used by Epic
    // output sample direction (N) equals the view direction V, which allows convolution without prior knowledge of V
    // the effects this has on visuals is negligible as documented in 
    // "Moving Frostbite to PBR" (https://media.contentapi.ea.com/content/dam/eacom/frostbite/files/course-notes-moving-frostbite-to-pbr-v2.pdf)
    vec3 N = normalize(v_FragmentPosition);
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;
    vec3 prefilteredColor = vec3(0.0);

    // Monte Carlo integral 
    // sum over different L(w_i), importance scaled, is the outgoing radiance L(w_o) 
    // see first part of split sum approximation
    for (uint i = 0u; i < SAMPLE_COUNT; i++)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, u_Roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float D = DistributionGGX(N, H, u_Roughness);
        float NdotH = max(dot(N, H), 0.0);
        float HdotV = max(dot(H, V), 0.0);
        float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;

        float saTexel = 4.0 * PI / (6.0 * u_Resolution * u_Resolution);
        float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

        float mipLevel = u_Roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            // sample env map with light direction L as per varying halfway vector
            // sample env-map with different mips as explained under Aliasing Artifacts
            // (article: https://chetanjags.wordpress.com/2015/08/26/image-based-lighting/)
            prefilteredColor += textureLod(u_EnvironmentMap, L, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

	o_Color = vec4(prefilteredColor, 1.0);
}

// the bit shift operator (left shift -> "<<") is used to increase the binary exponent of each binary digit by a specified amount
// example: 1100 << 2 (shift two exponents to the left) = 110000, i.e. 2^5 + 2^4 instead of 2^3 + 2^2.
float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Hammersley returns a low discrepancy 2D-sample as described 
// by Holger Dammertz (http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html)
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    // roughness squared for better visuals (as described by Epic)
    float a = roughness * roughness;

    // calculate polar coordinates or angles theta and phi from low-discrepancy sample and roughness
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);           // pythagoras

    // half-way vector, from spherical to cartesian
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // construct tangent-space base 
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    // H in world space using tangent-space base
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;

    return normalize(sampleVec);
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