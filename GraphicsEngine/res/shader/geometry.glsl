#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;

out VS_OUT
{
	vec4 color;
}vs_out;
uniform mat4 u_Model;

layout(std140) uniform u_Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};

void main()
{
	gl_Position = u_Model * a_Position;
	vs_out.color = a_Color;
}

#shader geometry
#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;
out vec4 g_Color;
in VS_OUT
{
	vec4 color;
}gs_in[];


void create_house(vec4 position)
{
	g_Color = gs_in[0].color;
	gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0);
	EmitVertex();
	EndPrimitive();
}

void main()
{
	create_house(gl_in[0].gl_Position);	
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec4 g_Color;

void main()
{
	o_Color = g_Color;
}