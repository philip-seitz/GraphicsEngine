#include "SpotLight.h"

SpotLight::SpotLight():
	m_Direction(0.0f, 0.0f, -1.0f),
	m_InnerCutoff(glm::cos(glm::radians(5.5f))),
	m_OuterCutoff(glm::cos(glm::radians(16.5f)))
{
	// default directional light settings
	m_Constant = 1.0f;
	m_Linear = 0.027f;
	m_Quadratic = 0.0028;
	glm::vec3 color = { 1.0f, 1.0f, 1.0f };		// white 
	m_Ambient = 0.2f * color;
	m_Diffuse = 0.5f * color;
	m_Specular = 0.5f * color;
	m_Instance.scale = glm::vec3(0.1f);
}

void SpotLight::Draw(Shader* shader, Model* base_model)
{
	// primitve Cube does't have normals -> no NormalMatrix needed
	m_Instance.Draw(*base_model, shader);
}

void SpotLight::SetLightingUniforms(Shader* shader)
{
	shader->SetUniform3f("u_SpotLight.position", m_Instance.position);
	shader->SetUniform3f("u_SpotLight.ambient", m_Ambient);
	shader->SetUniform3f("u_SpotLight.diffuse", m_Diffuse);
	shader->SetUniform3f("u_SpotLight.specular", m_Specular);
	shader->SetUniform1f("u_SpotLight.constant", m_Constant);
	shader->SetUniform1f("u_SpotLight.linear", m_Linear);
	shader->SetUniform1f("u_SpotLight.quadratic", m_Quadratic);
	shader->SetUniform1f("u_SpotLight.innerCutoff", m_InnerCutoff);
	shader->SetUniform1f("u_SpotLight.outerCutoff", m_OuterCutoff);
	// can be linked to the camera or not
	shader->SetUniform3f("u_SpotLight.direction", m_Direction);
}

