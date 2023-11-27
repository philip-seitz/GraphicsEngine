#include "Point.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Point::Point()
{
	m_color = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_position = { 0.0f, 0.0f, 0.0f };
}

void Point::Draw(Shader* shader, Model* base_model)
{
	shader->SetUniformMat4f("u_Model", ModelMatrix());
	glBindVertexArray(base_model->va);
	glDrawArrays(GL_POINTS, 0, 1);
}

glm::mat4 Point::ModelMatrix()
{
	glm::mat4x4 combinedRotation(0);
	glm::mat4x4 translation(0);
	glm::mat4x4 scaling(0);

	combinedRotation = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f),
		glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
			glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
				glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
	translation = glm::translate(glm::mat4(1.0f), m_position);
	scaling = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	return translation * combinedRotation * scaling;

}
