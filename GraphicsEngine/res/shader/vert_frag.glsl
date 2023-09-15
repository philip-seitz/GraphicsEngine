#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
out vec4 v_Color;
out vec2 v_TexCoord;
out vec4 v_EyeSpacePosition;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * a_Position;
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;

	// Disable perspective division (homogeneous cords to position)
	// gl_Position /= gl_Position.w;

	v_EyeSpacePosition = u_View * u_Model * a_Position;
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
uniform float u_Texture_Mix;
uniform sampler2D u_Texture_0;
uniform sampler2D u_Texture_1;

void main()
{
	vec4 color = v_Color;
	float fogCoordinate = abs(v_EyeSpacePosition.z / v_EyeSpacePosition.w);

	// linear fog
	// fogFactor = (linearEnd - fogCoordinate) / (linearEnd - linearStart);

	// exponential fog
	// fogFactor = exp(-density * fogCoordinate);

	// exponential squared fog
	fogFactor = exp(-pow(density * fogCoordinate, 2.0));
	fogFactor = 1 - clamp(fogFactor, 0.0, 1.0);

	if (u_Use_Texture)
	{
		vec4 transparent_texture = texture(u_Texture_0, v_TexCoord);
		vec4 background_texture = texture(u_Texture_1, v_TexCoord);
		background_texture.a = 1.0f;
		vec4 texColor = mix(background_texture, transparent_texture, u_Texture_Mix);
		texColor.a = 1.0f;
		if (fogEnabled)
		{
			o_Color = mix(texColor, fogColor, fogFactor);
		}
		else
		{
			o_Color = texColor;
		}
	}

	else
	{
		if (fogEnabled)
		{
			o_Color = mix(color, fogColor, fogFactor);
		}
		else
		{
			o_Color = color;
		}
	}

}