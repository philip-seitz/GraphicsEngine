#pragma once
#include "Shader.h"
#include "Model.h"
#include "ModelInstance.h"

class SpotLight
{
public:
	SpotLight();

	void Draw(Shader* shader, Model* base_model);
	void SetLightingUniforms(Shader* shader);

	// spot light settings
	glm::vec3 m_Direction;
	float m_InnerCutoff;
	float m_OuterCutoff;

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
