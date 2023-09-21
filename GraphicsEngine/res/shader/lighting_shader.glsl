#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 3) in vec3 a_Normal;

out vec3 v_Normal;
out vec3 v_FragmentPosition;		// what coordinates in world space does this each fragment correspond to
out vec3 v_LightPosition;

uniform vec4 u_Light_Position;
uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat3 u_Normal_Matrix;



void main()
{
	gl_Position = u_Projection * u_View * u_Model * a_Position;
	v_Normal = u_Normal_Matrix * a_Normal;
	v_FragmentPosition = vec3(u_View * u_Model * a_Position);
	v_LightPosition = vec3(u_View * u_Model * u_Light_Position);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec3 v_Normal;
in vec3 v_FragmentPosition;
in vec3 v_LightPosition;

uniform vec4 u_Object_Color;
uniform vec4 u_Light_Color;
// uniform vec4 u_Light_Position;
uniform vec4 u_Camera_Position;

void main()
{
	int shinyness = 32;
	float ambientStrength = 0.1f;
	float specularStrength = 0.5f;
	float diffuseStrength = 2.0f;

	// diffuse calculation
	vec3 light_direction = -normalize(v_FragmentPosition - v_LightPosition);	// from object to light source (easier calc)
	vec3 normal = normalize(v_Normal);
	float diffuseValue = max(dot(normal, light_direction), 0.0f);		// values between 1 and 0; 1 if directly lit, 0 if not lit at all

	// specular calculation
	vec3 view_direction = -normalize(v_FragmentPosition - vec3(0.0f, 0.0f , 0.0f));
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specularValue = pow(max(dot(view_direction, reflection_direction), 0.0f), shinyness);

	// final color
	vec4 ambient = ambientStrength * u_Light_Color;
	vec4 diffuse = diffuseStrength * diffuseValue * u_Light_Color;
	vec4 specular = specularStrength * specularValue * u_Light_Color;

	vec4 color = (ambient + diffuse + specular) * u_Object_Color;
	color.w = 1.0f;
	o_Color = color;
}