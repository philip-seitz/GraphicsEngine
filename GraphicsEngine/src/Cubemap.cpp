#include "Cubemap.h"
#include "stb_image/stb_image.h"
#include <GL/glew.h>

enum CubemapType
{
	not_set = 0, standard, depth
};


Cubemap::Cubemap():
	cubemapID(0),
	width(0),
	height(0),
	nrChannels(0),
	data(nullptr),
	type(not_set)
{
	angles = { 0.0f, 0.0f, 0.0f };
	position = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 1.0f };		// 1x1x1 cube
}

void Cubemap::Init()
{
	glGenTextures(1, &cubemapID);
	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

	type = standard;
	std::vector<std::string> texture_faces
	{
		"res/textures/skybox/right.jpg",
		"res/textures/skybox/left.jpg",
		"res/textures/skybox/top.jpg",
		"res/textures/skybox/bottom.jpg",
		"res/textures/skybox/front.jpg",
		"res/textures/skybox/back.jpg"
	};

	for (int i = 0; i < texture_faces.size(); i++)
	{
		stbi_set_flip_vertically_on_load(0);		// set to 1 originally. cubemap textures are not inverted (unlike other textures)
		data = stbi_load(texture_faces[i].c_str(), &width, &height, &nrChannels, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

void Cubemap::InitDepthCubemap()
{
	glGenTextures(1, &cubemapID);
	glActiveTexture(GL_TEXTURE0 + 11);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

	type = depth;
	width = 1024;
	height = 1024;

	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::Draw(const Model& base_model, Shader* shader)
{
	glBindVertexArray(base_model.va);
	// glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);		// not necessary for cubemap (not switching between cubemaps yet)
	shader->SetUniform1i("u_Skybox", 10);
	shader->SetUniformMat4f("u_Model", ModelMatrix());
	glDrawElements(GL_TRIANGLES, 3 * base_model.TriCount, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(base_model.va);
	// draw all vertices 
	glDrawElements(GL_TRIANGLES, 3 * base_model.TriCount, GL_UNSIGNED_INT, nullptr);
}

glm::mat4x4 Cubemap::ModelMatrix()
{
	glm::mat4x4 combinedRotation(0);
	glm::mat4x4 translation(0);
	glm::mat4x4 scaling(0);

	combinedRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angles.x),
		glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
			glm::radians(angles.y), glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
				glm::radians(angles.z), glm::vec3(0.0, 0.0, 1.0));
	translation = glm::translate(glm::mat4(1.0f), position);
	scaling = glm::scale(glm::mat4(1.0f), scale);

	return translation * combinedRotation * scaling;
}