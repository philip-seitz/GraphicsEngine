#pragma once
#include "Shader.h"
#include <iostream>


class UniformBufferObject
{
public:
	UniformBufferObject();
	void InitBuffer(unsigned int size, const std::string& uniform_name, unsigned int bindingPointNumber, std::vector<Shader*> shader_list);
	void SetMat4f(unsigned int offset, glm::mat4 data);
	void SetVec3f(unsigned int offset, glm::vec3 data);
	void SetVec4f(unsigned int offset, glm::vec4 data);
	unsigned int uboID;

private:




};
