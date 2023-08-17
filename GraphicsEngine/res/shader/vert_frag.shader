#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
out vec4 v_Color;

uniform mat4 u_Projection;
uniform mat4 u_View;
void main()
{
	gl_Position = u_Projection*u_View*a_Position;
	v_Color = a_Color;

	// Disable perspective division (homogeneous cords to position)
	// gl_Position /= gl_Position.w;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec4 v_Color;

uniform vec4 u_Color;
void main()
{
	o_Color = v_Color;
}