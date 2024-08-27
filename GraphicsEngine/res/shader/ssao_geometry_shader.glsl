#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Normal;

layout(std140) uniform u_Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};

uniform mat4 u_Model;
// what i will do here is send both normal matrices. 
// when rendering meshes, the normal matrix for world space is sent automatically. all other code/shaders use this normal matrix version
// for this ssao shader i will additionally send a normal matrix in view space (manually)
uniform mat3 u_Normal_Matrix_View;
uniform mat3 u_Normal_Matrix;	// not actually needed 		

out vec3 v_Normal;
out vec3 v_FragmentPosition;
out vec2 v_TexCoord;


void main()
{
	gl_Position = u_Projection * u_View * u_Model * a_Position;
	v_FragmentPosition = vec3(u_View * u_Model * a_Position);		// in view space
	v_Normal = u_Normal_Matrix_View * a_Normal;					
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 460 core
layout(location = 0) out vec3 g_Position;
layout(location = 1) out vec3 g_Normal;
layout(location = 2) out vec4 g_AlbedoSpec;

// inputs should be in view space
in vec3 v_FragmentPosition;
in vec3 v_Normal;
in vec2 v_TexCoord;

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

uniform Material u_Material;

void main()
{
	g_Position = v_FragmentPosition;
	g_Normal = normalize(v_Normal);
	// g_AlbedoSpec.rgb = texture(u_Material.texture_diffuse1, v_TexCoord).rgb;
	g_AlbedoSpec.rgb = vec3(0.95);		// white-ish color for entire scene (looks kinda cool, and makes ao pop more)
}
