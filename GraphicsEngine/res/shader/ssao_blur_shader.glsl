#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Normal;

out vec2 v_TexCoord;

void main()
{
	gl_Position = a_Position;
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 460 core
layout(location = 0) out float o_Color;		// color attachment 0
in vec2 v_TexCoord;

uniform sampler2D u_TextureSSAO;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(u_TextureSSAO, 0));
	float result = 0.0;
	for (int x = -2; x < 2; x++)
	{
		for (int y = -2; y < 2; y++)
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(u_TextureSSAO, v_TexCoord + offset).r;
		}
	}
	o_Color = result / (4.0 * 4.0);
}