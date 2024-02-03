#include "PointLight.h"
#include "ModelInstance.h"


PointLight::PointLight()
{
	// default point light settings
	m_Constant = 1.0f;
	m_Linear = 0.045;
	m_Quadratic = 0.0075;
	m_Color = { 1.0f, 1.0f, 1.0f };		// white 
	m_Ambient = 0.2f * m_Color;
	m_Diffuse = 0.5f * m_Color;
	m_Specular = 0.5f * m_Color;
	m_Instance.scale = glm::vec3(0.25f);

}

PointLight::PointLight(glm::vec3 lightColor)
{
	// default point light settings
	m_Constant = 1.0f;
	m_Linear = 0.045;
	m_Quadratic = 0.0075;
	m_Color = lightColor;		// white 
	m_Ambient = 0.2f * m_Color;
	m_Diffuse = 0.5f * m_Color;
	m_Specular = 0.5f * m_Color;
	m_Instance.scale = glm::vec3(0.25f);
}


void PointLight::Draw(Shader* shader_source, Model* base_model)
{
	// shader_source->SetUniform3f("u_LightColor", {1.0f, 1.0f, 1.0f});
	m_Instance.Draw(*base_model, shader_source);
}

void PointLight::SetLightingUniforms(Shader* shader, const int& idx)
{
	shader->SetUniform3f("u_PointLight[" + std::to_string(idx) + "].position", m_Instance.position);
	shader->SetUniform3f("u_PointLight[" + std::to_string(idx) + "].ambient", m_Ambient);
	shader->SetUniform3f("u_PointLight[" + std::to_string(idx) + "].diffuse", m_Diffuse);
	shader->SetUniform3f("u_PointLight[" + std::to_string(idx) + "].specular", m_Specular);

	shader->SetUniform1f("u_PointLight[" + std::to_string(idx) + "].constant", m_Constant);
	shader->SetUniform1f("u_PointLight[" + std::to_string(idx) + "].linear", m_Linear);
	shader->SetUniform1f("u_PointLight[" + std::to_string(idx) + "].quadratic", m_Quadratic);
}
