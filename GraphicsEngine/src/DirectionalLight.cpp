#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
	// default directional light settings
	glm::vec3 color = { 1.0f, 1.0f, 1.0f };		// white 
	m_Ambient = 0.2f * color;
	m_Diffuse = 0.5f * color;
	m_Specular = 0.5f * color;
	m_Direction = { -1.0f, -4.0f, 0.0f };		// ~sun light
}

// NOTE: there isn't an actual light ojbect you can see, just the effect the light has on other objects
void DirectionalLight::SetLightingUniforms(Shader* shader)
{
	shader->SetUniform3f("u_DirLight.direction", m_Direction);
	shader->SetUniform3f("u_DirLight.ambient", m_Ambient);
	shader->SetUniform3f("u_DirLight.diffuse", m_Diffuse);
	shader->SetUniform3f("u_DirLight.specular", m_Specular);
}
