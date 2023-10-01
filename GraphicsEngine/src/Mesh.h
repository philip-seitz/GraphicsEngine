#pragma once

#include <iostream>
#include <vector>
#include "Texture.h"
#include "Shader.h"
#include "Util.h"

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader* shader);

private:
	unsigned int m_va, m_vb, m_ib;
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<Texture> m_Textures;
	void SetupMesh();
};