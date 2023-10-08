#pragma once
#include <glm/glm.hpp>
#include "Model.h"
#include "Shader.h"

class ModelInstance
{
public:
	ModelInstance();
	glm::mat4x4 ModelMatrix();
	glm::mat3x3 NormalMatrix();
	void Draw(const Model& base_model, Shader* shader);
	void DrawOutline(const Model& base_model, Shader* shader, Shader* outline);

	bool hasOutline;
	glm::vec3 position;
	glm::vec3 angles;
	glm::vec3 scale ;
	glm::vec3 color;

private:


};
