#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 2) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
	gl_Position = a_Position;
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec2 v_TexCoord;
uniform sampler2D tex;

const float tc_offset = 1.0f / 400.0f;		// offset for texture coordinate

void main()
{
	vec2 offsets[9] = vec2[](
		vec2(-tc_offset, tc_offset),
		vec2(0.0f, tc_offset),
		vec2(tc_offset, tc_offset),
		vec2(-tc_offset, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(tc_offset, 0.0f),
		vec2(-tc_offset, -tc_offset),
		vec2(0.0f, -tc_offset),
		vec2(tc_offset, -tc_offset));

	// funky kernel
	/*int kernel[9] = int[](
		2,   2,   2,
		2, -15,   2,
		2,   2,   2);*/

	// sharpen kernel
	/*int kernel[9] = int[](
		-1, -1, -1,
		-1, 9, -1,
		-1, -1, -1);*/

	// blur kernel
	/*float kernel[9] = float[](
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
		2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f);*/

	// edge detection kernel
	/*int kernel[9] = int[](
		1, 1, 1,
		1, -8, 1,
		1, 1, 1);*/

	// LaPlacian kernel
	/*int kernel[9] = int[](
		0, -1, 0,
		-1, 5, -1,
		0, -1, 0);*/

	// inversion
	// o_Color = vec4(vec3(1.0f) - texture(tex, v_TexCoord).xyz, 1.0f);	

	// gray scaling
	/*vec4 color = texture(tex, v_TexCoord);
	float avg = (color.r + color.g + color.b) / 3.0;
	o_Color = vec4(vec3(avg), 1.0f);*/
	
	// more accurate gray scaling
	//vec4 color = texture(tex, v_TexCoord);
	//float avg = color.r * 0.213 + color.g * 0.715 + color.b * 0.072;		// weighted average
	//o_Color = vec4(vec3(avg), 1.0f);

	// using kernels
	/*vec4 textureSamples[9];
	for (int i = 0; i < 9; i++)
	{
		textureSamples[i] = texture(tex, offsets[i] + v_TexCoord);
	}

	vec3 sum = vec3(0.0f);
	for (int i = 0; i < 9; i++)
	{
		sum += textureSamples[i].rgb*kernel[i];
	}

	o_Color = vec4(sum, 1.0f);*/

	// no preprocessing
	o_Color = texture(tex, v_TexCoord);

}
