#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
out vec4 v_Color;
out vec2 v_TexCoord;
out vec4 v_EyeSpacePosition;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	gl_Position = u_Projection*u_View*a_Position;
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;

	// Disable perspective division (homogeneous cords to position)
	// gl_Position /= gl_Position.w;

	v_EyeSpacePosition = u_View * a_Position;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec4 v_Color;
in vec2 v_TexCoord;
in vec4 v_EyeSpacePosition;

// fog parameters
vec4 fogColor = vec4( 0.1f, 0.1f, 0.1f, 1.0f );
float linearStart = 15.0f;
float linearEnd = 60.0f;
float density = 0.05f;
bool fogEnabled = true;
float fogFactor = 1.0f;

uniform bool u_Use_Texture;
uniform sampler2D u_Tex_Index;
void main()
{
	vec4 color;
	if (u_Use_Texture)
	{
		vec4 texColor = texture(u_Tex_Index, v_TexCoord);
		color = texColor * v_Color;
	}
	else
	{
		color = v_Color;
	}
	
	if (fogEnabled)
	{
		float fogCoordinate = abs(v_EyeSpacePosition.z / v_EyeSpacePosition.w);

		// linear fog
		// fogFactor = (linearEnd - fogCoordinate) / (linearEnd - linearStart);

		// exponential fog
		// fogFactor = exp(-density * fogCoordinate);

		// exponential squared fog
		fogFactor = exp(-pow(density * fogCoordinate, 2.0));

		fogFactor = 1 - clamp(fogFactor, 0.0, 1.0);
		color = mix(color, fogColor, fogFactor);
	}

	if (u_Use_Texture)
	{
		vec4 texColor = texture(u_Tex_Index, v_TexCoord);
		o_Color = texColor*color;
	}

	o_Color = color;
}