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
uniform sampler2D u_hdrBuffer;


void main()
{
	float exposure = 5.0f;
	float gamma = 2.2; 
	vec3 hdrColor = texture(u_hdrBuffer, v_TexCoord).xyz;

	// exposure tonemapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	// reinhard tonemapping (HDR to LDR)
	// vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

	// gamma correction 
	mapped = pow(mapped, vec3(1.0 / gamma));

	o_Color = vec4(mapped, 1.0);
}
