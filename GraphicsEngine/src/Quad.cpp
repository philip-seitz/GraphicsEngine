#include "Quad.h"

Quad::Quad()
{
	std::string diffusePath = "res/textures/wall/brickwall.jpg";
	std::string specularPath = "res/textures/placeholder_spec.png";
	std::string normalPath = "res/textures/wall/brickwall_normal.jpg";

	std::string diffType = "texture_diffuse";
	std::string specType = "texture_specular";
	std::string normType = "texture_normal";

	// diffuse texture
	unsigned int id_diff = 0;
	TextureFromFile(diffusePath, id_diff);					// texture settings here 
	Texture tex_diff(diffusePath, diffType, id_diff);		// storage of ids etc
	m_Textures.push_back(tex_diff);
	// specular texture
	unsigned int id_spec = 0;
	TextureFromFile(diffusePath, id_spec);
	Texture tex_spec(diffusePath, specType, id_spec);
	m_Textures.push_back(tex_spec);
	// normal map
	unsigned int id_normal = 0;
	TextureFromFile(normalPath, id_normal);
	Texture tex_normal(normalPath, normType, id_normal);
	m_Textures.push_back(tex_normal);
}

void Quad::Draw(Shader* shader, Model* base_model)
{
	for (int i = 0; i < m_Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].GetID());
		std::string type;
		if (m_Textures[i].GetType() == "texture_diffuse")
		{
			type = "texture_diffuse";
		}
		else if (m_Textures[i].GetType() == "texture_specular")
		{
			type = "texture_specular";
		}
		else if (m_Textures[i].GetType() == "texture_normal")
		{
			type = "texture_normal";
		}
		// shader->SetUniformMat3f("u_Normal_Matrix", m_Instance.NormalMatrix());
		shader->SetUniform1i(("u_Material." + type + "1").c_str(), i);
	}
	m_Instance.Draw(*base_model, shader);
}

void Quad::DrawInstanced(Shader* shader, Model* base_model, unsigned int& instanceNumber)
{
	m_Instance.DrawInstanced(*base_model, shader, instanceNumber);
}

void Quad::SetLightingUniforms(Shader* shader, const int& idx)
{
}
