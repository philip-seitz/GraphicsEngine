#shader vertex
#version 460 core
layout(location = 0) in vec4 a_Position;
out vec3 v_FragmentPosition;

uniform mat4 u_Projection;
uniform mat4 u_View;


void main()
{
	v_FragmentPosition = a_Position.xyz;					// fragment position as sample direction
	gl_Position = u_Projection * u_View * a_Position;		// to capture each cube face from the inside
}

#shader fragment
#version 460 core
layout(location = 0) out vec4 o_Color;
in vec3 v_FragmentPosition;
uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265359;

void main()
{
	vec3 normal = normalize(v_FragmentPosition);

	vec3 irradiance = vec3(0.0);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, normal);
	up = cross(normal, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += texture(u_EnvironmentMap, sampleVec).rgb * cos(theta) * sin(theta);

			nrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0 / float(nrSamples));


	o_Color = vec4(irradiance, 1.0);
}