#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
out vec3 TexCoords;

uniform mat4 u_Model;
// uniform mat4 u_Projection;
// uniform mat4 u_View;

// uniform block to replace single uniforms
layout(std140) uniform u_Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};

void main()
{
	TexCoords = a_Position.xyz;

	// remove translation part of view matrix (camera should always be in the middle of the cube)
    mat3 view3x3 = mat3(u_View);
    mat4 viewNoTranslation = mat4(view3x3[0], 0.0,
        view3x3[1], 0.0,
        view3x3[2], 0.0,
        0.0, 0.0, 0.0, 0.0);

	vec4 pos = u_Projection * viewNoTranslation * u_Model * a_Position;
	gl_Position = pos.xyww;			// set z to w -> depth will always be w/w = 1.0
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec3 TexCoords;
uniform samplerCube u_Skybox;

void main()
{
	o_Color = texture(u_Skybox, TexCoords);
	// o_Color = vec4(0.8f, 0.3f, 0.3f, 1.0f);
}