#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;


uniform mat4 u_MVP;
void main()
{
	gl_Position = a_Position;

	// Disable perspective division (homogeneous cords to position)
	gl_Position /= gl_Position.w;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;

uniform vec4 u_Color;
void main()
{
	o_Color = u_Color;
}