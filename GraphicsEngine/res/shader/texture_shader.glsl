#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 2) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat3 u_Normal_Matrix;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * a_Position;
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;

struct Material
{
	sampler2D texture_diffuse1;		// also used as ambient
	sampler2D texture_diffuse2;
	sampler2D texture_diffuse3;

	sampler2D texture_specular1;
	sampler2D texture_specular2;
	sampler2D texture_specular3;

	// float shininess;
};

in vec2 v_TexCoord;

uniform mat4 u_View;
uniform Material u_Material;

void main()
{
	vec4 diffuseMap = texture(u_Material.texture_diffuse1, v_TexCoord);
	vec4 color = diffuseMap;
	color.w = 1.0f;
	o_Color = color;
}
