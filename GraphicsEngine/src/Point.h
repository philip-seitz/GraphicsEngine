#pragma once
#include <vector>
#include "Shader.h"
#include "Model.h"
#include "ModelInstance.h"
#include <glm/glm.hpp>

class Point
{
public:
	Point();
	void Draw(Shader* shader, Model* base_model);
	glm::mat4 ModelMatrix();
	glm::vec4 m_color;
	glm::vec3 m_position;

private:




};