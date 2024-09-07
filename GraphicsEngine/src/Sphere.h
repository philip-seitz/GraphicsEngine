#pragma once

#include "Shader.h"
#include "Model.h"
#include "ModelInstance.h"
#include "Texture.h"

class Sphere
{
public:

	Sphere();
	void Draw(Shader* shader, Model* base_model);

	ModelInstance m_Instance;
	std::vector<Texture> m_Textures;


private:


};
