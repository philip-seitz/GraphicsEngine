#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in vec3 a_Normal;

uniform mat4 u_Model;
out VS_OUT
{
	vec2 texCoords;
	vec4 color;
	vec3 normal;
}vs_out;

layout(std140) uniform u_Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};

void main()
{
	gl_Position = u_Projection * u_View * u_Model * a_Position;
	vs_out.color = a_Color;
	vs_out.texCoords = a_TexCoords;
	mat3 normalMatrix = mat3(transpose(inverse(u_View * u_Model)));			// to view space without translation + non uniform scale fix
	vs_out.normal = normalMatrix * a_Normal;		
}

#shader geometry
#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;
uniform float u_Time;

out vec4 g_Color;
out vec2 v_TexCoords;

const float magnitude = 0.4;

in VS_OUT
{
	vec2 texCoords;
	vec4 color;
	vec3 normal;
}gs_in[];

vec3 GetNormal()
{
	// TODO: why does this work??? (vertex winding order etc.)
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

vec4 Explode(vec4 pos, vec3 normal)
{
	float magnitude = 2.0f;
	vec3 direction = normal * magnitude * (sin(u_Time) + 1.0) / 2.0f;
	return pos + vec4(direction, 0.0f);
}

void GenerateLine(int index)
{
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * magnitude;
	EmitVertex();
	EndPrimitive();
}

void create_house(vec4 position)
{
	g_Color = gs_in[0].color;
	gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0);
	EmitVertex();
	EndPrimitive();
}

void MovingExplosion()
{
	vec3 normal = GetNormal();
	gl_Position = Explode(gl_in[0].gl_Position, normal);
	v_TexCoords = gs_in[0].texCoords;
	EmitVertex();

	gl_Position = Explode(gl_in[1].gl_Position, normal);
	v_TexCoords = gs_in[1].texCoords;
	EmitVertex();

	gl_Position = Explode(gl_in[2].gl_Position, normal);
	v_TexCoords = gs_in[2].texCoords;
	EmitVertex();
	EndPrimitive();
}

void main()
{
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
	// MovingExplosion();
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec2 v_TexCoords;
in vec4 g_Color;
struct Material
{
	sampler2D texture_diffuse1;			// also used as ambient
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
	// o_Color = texture(u_Material.texture_diffuse1, v_TexCoords);
	o_Color = vec4(1.0f, 0.3f, 0.3f, 1.0f);
}