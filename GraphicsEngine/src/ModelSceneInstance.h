#pragma once
#include "glm/glm.hpp"
#include "ModelScene.h"
#include "Shader.h"

class ModelSceneInstance
{
public:
	ModelSceneInstance();
	void Draw(Shader* shader, ModelScene& base_model);
	glm::mat4x4 ModelMatrix();
	glm::mat3x3 NormalMatrix();
	void DrawOutline(ModelScene& base_model, Shader* shader, Shader* outline);

	glm::vec3 m_position;
	glm::vec3 m_angles;
	glm::vec3 m_scale;

private:




};
