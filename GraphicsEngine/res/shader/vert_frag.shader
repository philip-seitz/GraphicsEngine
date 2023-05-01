#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;

void main()
{
	gl_Position = a_Position;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;

uniform vec4 u_Color;
void main()
{
	o_Color = u_Color;
}