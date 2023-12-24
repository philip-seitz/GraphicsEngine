#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;

uniform mat4 u_Model;
// uniform mat4 u_Projection;
// uniform mat4 u_View;

layout(std140) uniform u_LightSpaceMatrices
{
	mat4 u_LightProjection;
	mat4 u_LightView;
};

void main()
{
	gl_Position = u_LightProjection * u_LightView * u_Model * a_Position;
}

#shader fragment
#version 460 core
layout(location = 0) out vec4 o_Color;

void main()
{
	// o_Color = vec4(0.3f, 0.3f, 0.9f, 1.0f);
}
