#include "Sphere.h"
#include "Util.h"

Sphere::Sphere()
{
	const std::string diffusePath = "res/textures/placeholder.png";
	std::string specularPath = "res/textures/placeholder_spec.png";
	std::string metallicnessPath = "res/materials/rusted_iron/rustediron2_metallicness.png";
	std::string roughnessPath = "res/materials/rusted_iron/rustediron2_roughness.png";
	std::string albedoPath = "res/materials/rusted_iron/rustediron2_albedo.png";
	std::string normalPath = "res/materials/rusted_iron/rustediron2_normal.png";
	std::string aoPath = "res/materials/rusted_iron/rustediron2_ao.png";

	std::string diffType = "texture_diffuse";
	std::string specType = "texture_specular";
	std::string metalType = "texture_metallicness";
	std::string roughType = "texture_roughness";
	std::string albedoType = "texture_albedo";
	std::string normalType = "texture_normal";
	std::string aoType = "texture_ao";

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
	// metallicness texture 
	unsigned int id_metal = 0;
	Texture tex_metal(metallicnessPath, metalType, id_metal);
	tex_metal.Init();
	m_Textures.push_back(tex_metal);
	// roughness texture 
	unsigned int id_rough = 0;
	Texture tex_rough(roughnessPath, roughType, id_rough);
	tex_rough.Init();
	m_Textures.push_back(tex_rough);
	// albedo texture
	unsigned int id_albedo = 0;
	Texture tex_albedo(albedoPath, albedoType, id_albedo);
	tex_albedo.Init();
	m_Textures.push_back(tex_albedo);
	// normal texture
	unsigned int id_normal = 0;
	Texture tex_normal(normalPath, normalType, id_normal);
	tex_normal.Init();
	m_Textures.push_back(tex_normal);
	// ao texture
	unsigned int id_ao = 0;
	Texture tex_ao(aoPath, aoType, id_ao);
	tex_ao.Init();
	m_Textures.push_back(tex_ao);
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
		else if (m_Textures[i].GetType() == "texture_metallicness")
		{
			type = "texture_metallicness";
		}
		else if (m_Textures[i].GetType() == "texture_roughness")
		{
			type = "texture_roughness";
		}
		else if (m_Textures[i].GetType() == "texture_albedo")
		{
			type = "texture_albedo";
		}
		else if (m_Textures[i].GetType() == "texture_normal")
		{
			type = "texture_normal";
		}
		else if (m_Textures[i].GetType() == "texture_ao")
		{
			type = "texture_ao";
		}
		
		shader->SetUniform1i(("u_Material." + type + "1").c_str(), i);
	}
	shader->SetUniformMat3f("u_Normal_Matrix", m_Instance.NormalMatrix());
	m_Instance.Draw(*base_model, shader);
}
