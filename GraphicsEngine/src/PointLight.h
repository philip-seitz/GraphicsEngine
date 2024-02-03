#pragma once
#include <glm/glm.hpp>

#include "Shader.h"
#include "ModelInstance.h"
#include "Model.h"
class PointLight
{

public:

	PointLight();
	PointLight(glm::vec3 lightColor);
	void Draw(Shader* shader, Model* base_model);
	void SetLightingUniforms(Shader* shader, const int& idx);
	

	glm::vec3 m_Color;
	// color values
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;
	// attenuation factors
	float m_Constant;
	float m_Linear;
	float m_Quadratic;
	// position is stored in ModelInstance (only one position)
	ModelInstance m_Instance;

private:

};
