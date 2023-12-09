#include "ModelSceneInstance.h"

ModelSceneInstance::ModelSceneInstance():
	m_position(0.0f, 0.0f, 0.0f),
	m_angles(0.0f, 0.0f, 0.0f),
	m_scale(1.0f, 1.0f, 1.0f)
{


}

void ModelSceneInstance::Draw(Shader* shader, ModelScene& base_model)
{
	shader->SetUniformMat4f("u_Model", ModelMatrix());
	shader->SetUniformMat3f("u_Normal_Matrix", NormalMatrix());

	base_model.Draw(shader);

}


glm::mat4x4 ModelSceneInstance::ModelMatrix()
{
	glm::mat4x4 combinedRotation(0);
	glm::mat4x4 translation(0);
	glm::mat4x4 scaling(0);

	combinedRotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_angles.x),
		glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
			glm::radians(m_angles.y), glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
				glm::radians(m_angles.z), glm::vec3(0.0, 0.0, 1.0));
	translation = glm::translate(glm::mat4(1.0f), m_position);
	scaling = glm::scale(glm::mat4(1.0f), m_scale);

	return translation * combinedRotation * scaling;
}

glm::mat3x3 ModelSceneInstance::NormalMatrix()
{
	return glm::inverse(ModelMatrix());
}

void ModelSceneInstance::DrawOutline(ModelScene& base_model, Shader* shader, Shader* outline)
{
	
	glm::vec3 temp = m_scale;
	float border = 0.01f;
	m_scale = { m_scale.x * (m_scale.x + border) / (m_scale.x), m_scale.y * (m_scale.y + border) / (m_scale.y), m_scale.z * (m_scale.z + border) / (m_scale.z) };
	outline->SetUniformMat4f("u_Model", ModelMatrix());

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0x00);		// disable writing to stencil buffer
	base_model.Draw(outline);

	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);		// enable writing to stencil buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	// if stenicil and depth pass
	m_scale = temp;
}

