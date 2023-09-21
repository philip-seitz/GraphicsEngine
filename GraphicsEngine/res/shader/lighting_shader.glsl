#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 3) in vec3 a_Normal;

out vec4 v_Color;

uniform vec4 u_Light_Position;
uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat3 u_Normal_Matrix;

uniform vec4 u_Object_Color;
uniform vec4 u_Light_Color;
uniform vec4 u_Camera_Position;

void main()
{
	int shinyness = 32;
	float ambientStrength = 0.1f;
	float specularStrength = 0.5f;
	float diffuseStrength = 1.0f;

	gl_Position = u_Projection * u_View * u_Model * a_Position;

	// Gouraud lighting model (color per vertex with fragment interpolation)
	vec3 normal = u_Normal_Matrix * a_Normal;
	vec3 fragmentPosition = vec3(u_View * u_Model * a_Position);
	vec3 lightPosition = vec3(u_View * u_Light_Position);		// light pos already in world space

	// diffuse calculation
	vec3 light_direction = -normalize(fragmentPosition - lightPosition);	// from object to light source (easier calc)
	normal = normalize(normal);
	float diffuseValue = max(dot(normal, light_direction), 0.0f);		// values between 1 and 0; 1 if directly lit, 0 if not lit at all

	// specular calculation
	vec3 view_direction = -normalize(fragmentPosition - vec3(0.0f, 0.0f , 0.0f));
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specularValue = pow(max(dot(view_direction, reflection_direction), 0.0f), shinyness);

	// final color
	vec4 ambient = ambientStrength * u_Light_Color;
	vec4 diffuse = diffuseStrength * diffuseValue * u_Light_Color;
	vec4 specular = specularStrength * specularValue * u_Light_Color;

	v_Color = (ambient + diffuse + specular) * u_Object_Color;

}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec4 v_Color;

void main()
{
	vec4 color = v_Color;
	color.w = 1.0f;
	o_Color = color;
}