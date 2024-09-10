#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Normal;


uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 v_FragmentPosition;								// what coordinates in world space does this each fragment correspond to

void main()
{
	// model matrix not needed as cube will be at the origin
	gl_Position = u_Projection * u_View * a_Position;	// to capture faces, we need the view matrix
	v_FragmentPosition = a_Position.xyz;				// we use the cubes position as dir vector to sample
}

#shader fragment
#version 460 core
layout(location = 0) out vec4 o_Color;

in vec3 v_FragmentPosition;

uniform sampler2D u_EquirectangularMap;

vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));			// fragment position in polar coords
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(v_FragmentPosition));
	vec3 color = texture(u_EquirectangularMap, uv).rgb;

	o_Color = vec4(color, 1.0);
}