#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Normal;

out vec3 v_Normal;
out vec3 v_FragmentPosition;		// what coordinates in world space does this each fragment correspond to
out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat3 u_Normal_Matrix;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * a_Position;
	v_Normal = u_Normal_Matrix * a_Normal;
	v_FragmentPosition = vec3(u_View * u_Model * a_Position);
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
struct Material 
{
	sampler2D diffuse;		// also used as ambient
	sampler2D specular;
	sampler2D emissive;
	float shininess;
};

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 v_Normal;
in vec3 v_FragmentPosition;
in vec3 v_LightPosition;
in vec2 v_TexCoord;

uniform mat4 u_View;
uniform vec3 u_Object_Color;
uniform vec3 u_Light_Color;
uniform Material u_Material;
uniform Light u_Light;

void main()
{
	vec3 diffuseMap = texture(u_Material.diffuse, v_TexCoord).xyz;
	vec3 specularMap = texture(u_Material.specular, v_TexCoord).xyz;
	vec3 emissionMap = texture(u_Material.emissive, v_TexCoord).xyz;

	// diffuse calculation
	vec3 light_direction = -normalize(v_FragmentPosition - vec3(u_View * vec4(u_Light.position, 1.0f)));	// from object to light source (easier calc)
	vec3 normal = normalize(v_Normal);
	float diffuseValue = max(dot(normal, light_direction), 0.0f);		// values between 1 and 0; 1 if directly lit, 0 if not lit at all

	// specular calculation
	vec3 view_direction = -normalize(v_FragmentPosition - vec3(0.0f, 0.0f , 0.0f));
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specularValue = pow(max(dot(view_direction, reflection_direction), 0.0f), u_Material.shininess);

	// final color
	vec3 ambient = u_Light.ambient * diffuseMap;
	vec3 diffuse = u_Light.diffuse * (diffuseValue * diffuseMap + emissionMap/(1.0f + diffuseValue));
	vec3 specular = u_Light.specular * (specularValue * specularMap);

	vec4 color = vec4(ambient + diffuse + specular, 1.0f);
	o_Color = color;
}