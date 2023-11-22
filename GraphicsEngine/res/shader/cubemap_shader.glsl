#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
out vec3 TexCoords;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	TexCoords = a_Position.xyz;
	gl_Position = u_Projection * u_View * u_Model * a_Position;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec3 TexCoords;
uniform samplerCube skybox;

void main()
{
	o_Color = texture(skybox, TexCoords);
	// o_Color = vec4(0.8f, 0.3f, 0.3f, 1.0f);
}