#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;
out vec3 v_FragmentPosition;

layout(std140) uniform u_Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};

void main()
{
	v_FragmentPosition = a_Position.xyz;

	mat4 rotView = mat4(mat3(u_View));
	vec4 clipPos = u_Projection * rotView * a_Position;
	gl_Position = clipPos.xyww;			// set z to w -> depth will always be w/w = 1.0
}

#shader fragment
#version 460 core
layout(location = 0) out vec4 o_Color;
in vec3 v_FragmentPosition;
uniform samplerCube u_EnvironmentMap;

void main()
{
	vec3 envColor = textureLod(u_EnvironmentMap, v_FragmentPosition, 1.2).rgb;
	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor, vec3(1.0 / 2.2));
	o_Color = vec4(envColor, 1.0);
}