#include "ModelScene.h"
#include "Util.h"


void TextureFromFile(const std::string& path, unsigned int& id);

ModelScene::ModelScene()
{
}

ModelScene::ModelScene(const std::string& path)
{
	LoadModel(path);
}

// move
void ModelScene::Draw(Shader* shader)
{
	/*shader->SetUniformMat4f("u_Model", ModelMatrix());
	shader->SetUniformMat3f("u_Normal_Matrix", glm::mat3x3(glm::inverse(ModelMatrix())));	*/  
	for (int i = 0; i < m_Meshes.size(); i++)
	{
		m_Meshes[i].Draw(shader);
	}
}

void ModelScene::DrawInstanced(Shader* shader, unsigned int& numberInst)
{
	for (int i = 0; i < m_Meshes.size(); i++)
	{
		m_Meshes[i].DrawInstanced(shader, numberInst);
	}
}

void ModelScene::AddInstArrayAttribMat4(glm::mat4* matrices, unsigned int& numberMatrices)
{
	glGenBuffers(1, &ivb);
	glBindBuffer(GL_ARRAY_BUFFER, ivb);
	glBufferData(GL_ARRAY_BUFFER, numberMatrices * sizeof(glm::mat4), matrices, GL_STATIC_DRAW);		// TODO: might not work like this...

	for (int i = 0; i < m_Meshes.size(); i++)
	{
		glBindVertexArray(m_Meshes[i].m_va);
		std::size_t size_vec4 = sizeof(glm::vec4);

		// add 4x4 matrix as 4 different vec4 attribs
		glEnableVertexAttribArray(4);		// mesh has 4 other attribs (pos, color, tex_coords, normal)
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4*size_vec4, (const void*)0);
		glEnableVertexAttribArray(5);		
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4*size_vec4, (const void*)(size_vec4));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * size_vec4, (const void*)(2*size_vec4));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * size_vec4, (const void*)(3 * size_vec4));

		glVertexAttribDivisor(4, 1);		// update for every new instance
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);

		glBindVertexArray(0);
	}

}

void ModelScene::LoadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(	path, 
											aiProcess_Triangulate |	
											aiProcess_FlipUVs |				// triangulate mesh and flip textures on load
											aiProcess_CalcTangentSpace);	// calculate smooth/averaged tangent and bitangent vecotors per vertex

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	std::cout << "LOADING DONE" << std::endl;
	m_Directory = path.substr(0, path.find_last_of('/')+1);	// directory of filepath
	ProcessNode(scene->mRootNode, scene);
}

void ModelScene::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		unsigned int id = node->mMeshes[i];
		aiMesh* mesh = scene->mMeshes[id];
		m_Meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh ModelScene::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;


	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		// ===== vertex positions (loc 0) ====
		Vertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		// ==== vertex colors	(loc 1) ====
		
		// check if vertex has vertex colors
		// if(mesh->HasVertexColors(i))				// TODO: check if this works!
		/*{
			vertex.color.r = mesh->mColors[i]->r;
			vertex.color.g = mesh->mColors[i]->g;
			vertex.color.b = mesh->mColors[i]->b;
			vertex.color.a = mesh->mColors[i]->a;
		}*/
		vertex.color = { 0.0f, 0.0f, 0.0f, 0.0f };

		
		// ==== texture coordinates	(loc 2) ====
		if (mesh->mTextureCoords[0])			// TODO: might not work -> use mesh->mTextureCoords[0] instead!
		{
			vertex.texture_cords.x = mesh->mTextureCoords[0][i].x;
			vertex.texture_cords.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.texture_cords = { 0.0f, 0.0f };

		// ==== normals (loc 3) ====
		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		// ==== tangents (loc 4) ====
		vertex.tangent.x = mesh->mTangents[i].x;
		vertex.tangent.y = mesh->mTangents[i].y;
		vertex.tangent.z = mesh->mTangents[i].z;

		// ==== bitangents (loc 5) ==== 
		// normal and tangent might suffice to calc bitangent...
		vertex.bitangent.x = mesh->mBitangents[i].x;
		vertex.bitangent.y = mesh->mBitangents[i].y;
		vertex.bitangent.z = mesh->mBitangents[i].z;

		vertices.push_back(vertex);
	}

	// indices
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)	// iterate over indices of primtives (face) i.e. quad or triangle
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}

	// materials
	if (mesh->mMaterialIndex >= 0)
	{
		unsigned int matID = mesh->mMaterialIndex;
		aiMaterial* mat = scene->mMaterials[matID];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());		// insert at the end 

		std::vector<Texture> specularMaps = loadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> normalMaps = loadMaterialTextures(mat, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> ModelScene::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (int i = 0; i < material->GetTextureCount(type); i++)
	{
		bool textureLoaded = false;
		aiString str;		// path to Texture relative to model directory (m_Directory)
		material->GetTexture(type, i, &str);
		std::string texPath = (m_Directory+ str.C_Str()).c_str();		// path to texture 

		// check if Texture is already loaded
		for (int j = 0; j < m_TexturesLoaded.size(); j++)
		{
			
			if (std::strcmp(m_TexturesLoaded[j].GetPath().data(), texPath.c_str()) == 0)	// compare texture path strings; 0 -> equal
			{
				// texture already loaded -> copy
				textureLoaded = true;
				textures.push_back(m_TexturesLoaded[j]);
				break;
			}
		}

		if (!textureLoaded)
		{
			unsigned int id = 0;
			Texture tex(texPath, typeName, id);
			tex.Init();
			textures.push_back(tex);
			m_TexturesLoaded.push_back(tex);
		}
	}
	return textures;
}

