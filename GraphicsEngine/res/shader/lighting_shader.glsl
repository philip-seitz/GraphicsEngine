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
	v_FragmentPosition = vec3(u_Model * a_Position);
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
	vec4 vector;		// position or direction

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// attenuation coefficients
	float constant;
	float linear;
	float quadratic;

	// spot light parameters 
	vec3 direction;
	float innerCutoff;
	float outerCutoff;
};

in vec3 v_Normal;
in vec3 v_FragmentPosition;
in vec3 v_LightPosition;
in vec2 v_TexCoord;

uniform mat4 u_View;
uniform vec3 u_Object_Color;
uniform vec3 u_Light_Color;
uniform vec3 u_CameraPosition;
uniform Material u_Material;
uniform Light u_Light;

bool isFlashLight = false;

void main()
{
	vec4 color;		

	vec3 diffuseMap = texture(u_Material.diffuse, v_TexCoord).xyz;
	vec3 specularMap = texture(u_Material.specular, v_TexCoord).xyz;
	vec3 emissionMap = texture(u_Material.emissive, v_TexCoord).xyz;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;

	vec3 light_direction;
	float attenuation_factor = 1.0f;
	
	if(u_Light.vector.w == 1.0f)	// point or spot light
	{
		light_direction = -normalize(v_FragmentPosition - u_Light.vector.xyz);	// from object to light source (easier calc)
		// light attenuation
		float d = distance(u_Light.vector.xyz, v_FragmentPosition);
		attenuation_factor = 1.0f/(u_Light.constant + u_Light.linear*d + u_Light.quadratic*d*d);
	}
	else	// directional light
	{
		light_direction = -normalize(u_Light.vector.xyz);	// direction vector is not transformed (no w component)
	}
	// diffuse calculation
	vec3 normal = normalize(v_Normal);
	float diffuseValue = max(dot(normal, light_direction), 0.0f);		// values between 1 and 0; 1 if directly lit, 0 if not lit at all

	// specular calculation
	vec3 view_direction = -normalize(v_FragmentPosition - u_CameraPosition);
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specularValue = pow(max(dot(view_direction, reflection_direction), 0.0f), u_Material.shininess);

	// spot Light
	if(isFlashLight)
	{
		vec3 spotDirection = -u_Light.direction;
		float theta = dot(light_direction, spotDirection);
		float intensity = (theta - u_Light.outerCutoff)/(u_Light.innerCutoff - u_Light.outerCutoff);
		clamp(intensity, 0.0f, 1.0f);

		diffuse = u_Light.diffuse * attenuation_factor * intensity * (diffuseValue * diffuseMap);
		specular = u_Light.specular * attenuation_factor * intensity * (specularValue * specularMap);
		ambient = u_Light.ambient * attenuation_factor * diffuseMap;
		emission = emissionMap;		// for glow effects

		color = vec4(ambient + diffuse + specular, 1.0f); 
	}
	// point light
	else 
	{
		// final color
		ambient = u_Light.ambient * attenuation_factor * diffuseMap;
		diffuse = u_Light.diffuse * attenuation_factor * (diffuseValue * diffuseMap);
		specular = u_Light.specular * attenuation_factor * (specularValue * specularMap);
		emission = emissionMap;		// for glow effects
		color = vec4(ambient + diffuse + specular, 1.0f); 
	}

	o_Color = color;
}
