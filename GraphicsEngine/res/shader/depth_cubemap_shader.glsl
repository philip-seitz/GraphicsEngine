#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;

uniform mat4 u_Model;

void main()
{
	gl_Position = u_Model * a_Position;
}


#shader geometry
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;
uniform mat4 u_LightView[6];
out vec4 fragPosition;

void main()
{
	for (int face = 0; face < 6; face++)
	{
		gl_Layer = face;
		for (int i = 0; i < 3; i++)
		{
			fragPosition = gl_in[i].gl_Position;
			gl_Position = u_LightView[face] * fragPosition;
			EmitVertex();
		}
		EndPrimitive();
	}
}
 
#shader fragment
#version 460 core
layout(location = 0) out vec4 o_Color;
in vec4 fragPosition;

uniform vec3 u_LightPosition;
uniform float u_FarPlane;

void main()
{
	float linearDepth = length(fragPosition.xyz - u_LightPosition)/u_FarPlane;		// normalized (0...1)

	gl_FragDepth = linearDepth;		// manually set depth for every fragment (instead of automatically -> ndc depth values etc.)
}
