#include "Cube.h"
#include "Util.h"
Cube::Cube()
{
	const std::string diffusePath = "res/textures/placeholder.png";
	std::string specularPath = "res/textures/placeholder_spec.png";
	std::string diffType = "texture_diffuse";
	std::string specType = "texture_specular";

	// diffuse texture
	unsigned int id_diff = 0;
	TextureFromFile(diffusePath, id_diff);
	Texture tex_diff(diffusePath, diffType, id_diff);
	m_Textures.push_back(tex_diff);
	// specular texture
	unsigned int id_spec = 0;
	TextureFromFile(diffusePath, id_spec);
	Texture tex_spec(diffusePath, specType, id_spec);
	m_Textures.push_back(tex_spec);
}

void Cube::Draw(Shader* shader, Model* base_model)
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
		shader->SetUniformMat3f("u_Normal_Matrix", m_Instance.NormalMatrix());
		shader->SetUniform1i(("u_Material." + type + "1").c_str(), i);
	}
	m_Instance.Draw(*base_model, shader);
}

void Cube::DrawOutline(Shader* shader, Shader* outline, Model* base_model)
{

	// set stencil for outline
	//glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0x00);	// disable writing to stencil buffer
	m_Instance.DrawOutline(*base_model, shader, outline);
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);		// enable writing to stencil buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	// if stenicil and depth pass
}

void Cube::SetLightingUniforms(Shader* shader, const int& idx)
{
}
