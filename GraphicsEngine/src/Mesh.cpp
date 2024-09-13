#include "Mesh.h"
#include "Util.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	m_Vertices = vertices;
	m_Indices = indices;
	m_Textures = textures;

	SetupMesh();
}

void Mesh::Draw(Shader* shader)
{
	unsigned int diffuseNumber = 1;
	unsigned int specularNumber = 1;
	unsigned int normalNumber = 1;

	unsigned int metallicnessNumber = 1;
	unsigned int roughnessNumber = 1;
	unsigned int albedoNumber = 1;
	unsigned int aoNumber = 1;

	for (int i = 0; i < m_Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE20 + i);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].GetID());
		std::string name = m_Textures[i].GetType();
		std::string number;
		if (name == "texture_diffuse")
		{
			number = std::to_string(diffuseNumber);
			diffuseNumber++;
		}
		else if (name == "texture_specular")
		{
			number = std::to_string(specularNumber);
			specularNumber++;
		}
		else if (name == "texture_normal")
		{
			number = std::to_string(normalNumber);
			normalNumber++;
		}

		// PBR materials
		else if (name == "texture_metallicness")
		{
			number = std::to_string(metallicnessNumber);
			metallicnessNumber++;
		}

		else if (name == "texture_roughness")
		{
			number = std::to_string(roughnessNumber);
			roughnessNumber++;
		}
		else if (name == "texture_albedo")
		{
			number = std::to_string(albedoNumber);
			albedoNumber++;
		}
		else if (name == "texture_ao")
		{
			number = std::to_string(aoNumber);
			aoNumber++;
		}

		// sampler for texture corresponding to diff or spec slot
		shader->SetUniform1i(("u_Material." + name + number).c_str(), i + 20);
	}

	// glActiveTexture(GL_TEXTURE0);
	// draw
	glBindVertexArray(m_va);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr);
}

void Mesh::DrawInstanced(Shader* shader, unsigned int& numberInstances)
{
	unsigned int diffuseNumber = 1;
	unsigned int specularNumber = 1;
	unsigned int metallicnessNumber = 1;
	unsigned int roughnessNumber = 1;
	unsigned int albedoNumber = 1;
	unsigned int normalNumber = 1;
	unsigned int aoNumber = 1;

	for (int i = 0; i < m_Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].GetID());
		std::string name = m_Textures[i].GetType();
		std::string number;
		if (name == "texture_diffuse")
		{
			number = std::to_string(diffuseNumber);
			diffuseNumber++;
		}
		else if (name == "texture_specular")
		{
			number = std::to_string(specularNumber);
			specularNumber++;
		}

		// PBR materials
		else if (name == "texture_metallicness")
		{
			number = std::to_string(metallicnessNumber);
			metallicnessNumber++;
		}

		else if (name == "texture_roughness")
		{
			number = std::to_string(roughnessNumber);
			roughnessNumber++;
		}
		else if (name == "texture_albedo")
		{
			number = std::to_string(albedoNumber);
			albedoNumber++;
		}
		else if (name == "texture_normal")
		{
			number = std::to_string(normalNumber);
			normalNumber++;
		}
		else if (name == "texture_ao")
		{
			number = std::to_string(aoNumber);
			aoNumber++;
		}
		
		// sampler for texture corresponding to diff or spec slot
		shader->SetUniform1i(("u_Material." + name + number).c_str(), i);
	}

	// draw
	glBindVertexArray(m_va);
	glDrawElementsInstanced(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr, numberInstances);

}

void Mesh::SetupMesh()
{
	// vertex array object setup
	glCreateVertexArrays(1, &m_va);
	glBindVertexArray(m_va);

	glCreateBuffers(1, &m_vb);
	glBindBuffer(GL_ARRAY_BUFFER, m_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_Vertices.size(), &m_Vertices[0], GL_STATIC_DRAW);

	glCreateBuffers(1, &m_ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texture_cords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));


	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tangent));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, bitangent));


}
