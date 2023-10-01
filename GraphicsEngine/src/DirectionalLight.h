#pragma once
#include <glm/glm.hpp>
#include "Shader.h"

class DirectionalLight
{
public:
	DirectionalLight();
	void SetLightingUniforms(Shader* shader);
	// color values
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;
	glm::vec3 m_Direction;

private:

};