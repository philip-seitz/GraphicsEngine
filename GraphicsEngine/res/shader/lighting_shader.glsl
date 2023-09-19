#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * a_Position;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;

uniform vec4 u_Object_Color;
uniform vec4 u_Light_Color;

void main()
{
	vec4 color = u_Light_Color*u_Object_Color;
	color.w = 1.0f;
	o_Color = color;
}