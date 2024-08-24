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

// input from g Buffer
uniform sampler2D u_AmbienceSSAO;
uniform sampler2D g_ColorSpec;

void main()
{	

	vec3 Albedo = texture(g_ColorSpec, v_TexCoord).rgb;
	float ambient = texture(u_AmbienceSSAO, v_TexCoord).r;
	vec3 lighting = Albedo * ambient;
	o_Color = vec4(lighting, 1.0);
}
