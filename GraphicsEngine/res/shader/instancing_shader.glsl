#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 3) in vec2 a_Offset;

out vec4 v_Color;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

// uniform vec2 offsets[100];

void main()
{
	// vec4 pos = a_Position + vec4(offsets[gl_InstanceID], 0.0, 0.0);
	vec4 pos = a_Position + vec4(a_Offset, 0.0, 0.0);
	gl_Position = pos;
	v_Color = a_Color;
}

#shader fragment
#version 460 core
layout(location = 0) out vec4 o_Color;
in vec4 v_Color;

void main()
{
	// o_Color = vec4(0.9f, 0.3f, 0.3f, 1.0f);
	o_Color = v_Color;
}
