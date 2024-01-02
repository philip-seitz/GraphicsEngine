#include "UniformBufferObject.h"
#include "vendor/glm/gtc/type_ptr.hpp"
#include <GL/glew.h>

UniformBufferObject::UniformBufferObject():
	uboID(0)
{
}

void UniformBufferObject::InitBuffer(unsigned int size, const std::string& uniform_name, unsigned int bindingPointNumber, std::vector<Shader*> shader_list)
{
	unsigned int uniformBlockID = 0;
	// Binding Point
	for (int i = 0; i < shader_list.size(); i++)
	{
		uniformBlockID = glGetUniformBlockIndex(shader_list[i]->GetRendererID(), uniform_name.c_str());
		glUniformBlockBinding(shader_list[i]->GetRendererID(), uniformBlockID, bindingPointNumber);
	}

	// Buffer
	glGenBuffers(1, &uboID);
	glBindBuffer(GL_UNIFORM_BUFFER, uboID);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);					// might have to be dynamic draw
	glBindBuffer(GL_UNIFORM_BUFFER, 0);												// unbind

	glBindBufferRange(GL_UNIFORM_BUFFER, bindingPointNumber, uboID, 0, size);		// use entire buffer memory and set to binding point number
}

void UniformBufferObject::SetMat4f(unsigned int offset, glm::mat4 data)
{
	// setup Uniform Buffer Object (each frame)
	glBindBuffer(GL_UNIFORM_BUFFER, uboID);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), glm::value_ptr(data));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void UniformBufferObject::SetVec3f(unsigned int offset, glm::vec3 data)
{
	// setup Uniform Buffer Object (each frame)
	glBindBuffer(GL_UNIFORM_BUFFER, uboID);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), glm::value_ptr(data));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void UniformBufferObject::SetVec4f(unsigned int offset, glm::vec4 data)
{
	// setup Uniform Buffer Object (each frame)
	glBindBuffer(GL_UNIFORM_BUFFER, uboID);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec4), glm::value_ptr(data));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}