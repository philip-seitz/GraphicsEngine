#pragma once

#include <vector>
#include "Shader.h"
#include "Model.h"
#include "ModelInstance.h"
#include "Texture.h"

class Quad
{
public:

	Quad();
	void Draw(Shader* shader, Model* base_model);
	void DrawInstanced(Shader* shader, Model* base_model, unsigned int& instanceNumber);
	// void DrawOutline(Shader* shader, Shader* outline, Model* base_model);
	void SetLightingUniforms(Shader* shader, const int& idx);

	ModelInstance m_Instance;		// stores postion, rotation etc.
	std::vector<Texture> m_Textures;


private:


};
