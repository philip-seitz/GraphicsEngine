#pragma once
#include "Model.h"
#include "Shader.h"

enum CubemapType;

class Cubemap
{
public:
	Cubemap();
	void Init();
	void InitDepthCubemap();
	void Draw(const Model& base_model, Shader* shader);
	glm::mat4x4 ModelMatrix();


	unsigned int cubemapID;
	int width, height, nrChannels;
	unsigned char* data;
	CubemapType type;

	glm::vec3 angles;
	glm::vec3 position;
	glm::vec3 scale;		// 1x1x1 cube


private:

};