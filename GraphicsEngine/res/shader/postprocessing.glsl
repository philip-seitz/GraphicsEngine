#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 2) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
	gl_Position = a_Position;
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec2 v_TexCoord;
uniform sampler2D tex;

void main()
{
	o_Color = texture(tex, v_TexCoord); // vec4(0.7f, 0.2f, 0.2f, 1.0f);//texture(tex, v_TexCoord);
}
