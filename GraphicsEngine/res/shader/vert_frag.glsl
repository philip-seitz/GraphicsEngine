#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
out vec4 v_Color;
out vec2 v_TexCoord;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	gl_Position = u_Projection*u_View*a_Position;
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;

	// Disable perspective division (homogeneous cords to position)
	// gl_Position /= gl_Position.w;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec4 v_Color;
in vec2 v_TexCoord;

uniform sampler2D u_Tex_Index;
void main()
{
	vec4 texColor = texture(u_Tex_Index, v_TexCoord);
	o_Color = texColor;
}