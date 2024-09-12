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
layout(location = 0) out vec2 o_Color;

// result stored in texture with only two color values
// r -> scale; b -> bias for initial Fresnel value F0

// texture coordinates respresents convolution input
// angle or <n, w_o> in x; roughness in y
in vec2 v_TexCoord;

const float PI = 3.14159265359;

// function declarations
float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec2 IntegrateBRDF(float NdotV, float roughness);

void main()
{
    vec2 integratedBRDF = IntegrateBRDF(v_TexCoord.x, v_TexCoord.y);
    o_Color = integratedBRDF;
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

float GeometrySchlickGGX(float NdotV, float roughness)
{
    // k for IBL is used here
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}


// this function solves the second part of the specular split sum approximation
// this integral can be split up further into two parts:
// part A: scale part; part B: bias part for F0
// this essentially follows the exaxt mathematics behind that integral
// note, that this is very much similar to how the prefilter convolution works
vec2 IntegrateBRDF(float NdotV, float roughness)
{
    vec3 V;
    V.x = sqrt(1.0 - NdotV * NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.0)
        {
            float G = GeometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}
