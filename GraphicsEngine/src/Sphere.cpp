#include "Sphere.h"
#include "Util.h"

Sphere::Sphere()
{
	const std::string diffusePath = "res/textures/placeholder.png";
	std::string specularPath = "res/textures/placeholder_spec.png";
	std::string diffType = "texture_diffuse";
	std::string specType = "texture_specular";

	// diffuse texture
	unsigned int id_diff = 0;
	Texture tex_diff(diffusePath, diffType, id_diff);
	tex_diff.Init();
	m_Textures.push_back(tex_diff);
	// specular texture
	unsigned int id_spec = 0;
	Texture tex_spec(diffusePath, specType, id_spec);
	tex_spec.Init();
	m_Textures.push_back(tex_spec);
}

void Sphere::Draw(Shader* shader, Model* base_model)
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
