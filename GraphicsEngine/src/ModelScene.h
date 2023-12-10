#pragma once
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelScene
{
public:
	ModelScene();
	ModelScene(const std::string& path);
	void Draw(Shader* shader);
	void DrawInstanced(Shader* shader, unsigned int& numberInst);
	void AddInstArrayAttribMat4(glm::mat4* matrices, unsigned int& numberMatrices);
	// glm::mat4x4 ModelMatrix();

	
private:
	std::vector<Mesh> m_Meshes;
	std::string m_Directory;
	std::vector<Texture> m_TexturesLoaded;

	// for buffer of model matrices for example
	unsigned int ivb;	// ivb = instanced vertex buffer (for instanced array)

	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);

};