#shader vertex
#version 330 core
layout(location = 0) in vec4 a_Position;
layout(location = 3) in vec3 a_Normal;

out vec3 v_Normal;
out vec3 v_FragmentPosition;		// what coordinates in world space does this each fragment correspond to

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

uniform mat3 u_Normal_Matrix;



void main()
{
	gl_Position = u_Projection * u_View * u_Model * a_Position;
	v_Normal = u_Normal_Matrix * a_Normal;
	v_FragmentPosition = vec3(u_Model * a_Position);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
in vec3 v_Normal;
in vec3 v_FragmentPosition;

uniform vec4 u_Object_Color;
uniform vec4 u_Light_Color;
uniform vec4 u_Light_Position;
uniform vec4 u_Camera_Position;

void main()
{
	float ambientStrength = 0.1f;
	vec3 light_direction = -normalize(v_FragmentPosition - vec3(u_Light_Position));	// from object to light source (easier calc)
	vec3 normal = normalize(v_Normal);
	float diffuseAmount = max(dot(normal, light_direction), 0.0f);		// values between 1 and 0; 1 if directly lit, 0 if not lit at all
	float specularStrength = 0.5f;
	vec3 view_direction = -normalize(v_FragmentPosition - vec3(u_Camera_Position));
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specularValue = pow(max(dot(view_direction, reflection_direction), 0.0f), 32);

	vec4 ambient = ambientStrength * u_Light_Color;
	vec4 diffuse = diffuseAmount * u_Light_Color;
	vec4 specular = specularValue * specularStrength * u_Light_Color;

	vec4 color = (ambient + diffuse + specular) * u_Object_Color;
	color.w = 1.0f;
	o_Color = color;
}