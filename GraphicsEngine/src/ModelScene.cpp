#include "ModelScene.h"
#include "stb_image/stb_image.h"


void TextureFromFile(const std::string& path, unsigned int& id);

ModelScene::ModelScene()
{
}

ModelScene::ModelScene(const std::string& path)
{
	LoadModel(path);
}

void ModelScene::Draw(Shader* shader)
{
	shader->SetUniformMat4f("u_Model", ModelMatrix());
	shader->SetUniformMat3f("u_Normal_Matrix", glm::mat3x3(glm::inverse(ModelMatrix())));	  
	for (int i = 0; i < m_Meshes.size(); i++)
	{
		m_Meshes[i].Draw(shader);
	}
}

glm::mat4x4 ModelScene::ModelMatrix()
{
	glm::mat4x4 combinedRotation(0);
	glm::mat4x4 translation(0);
	glm::mat4x4 scaling(0);
	glm::vec3 angles{ 0.0f, 0.0f, 0.0f };
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

	combinedRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angles.x),
		glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
			glm::radians(angles.y), glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
				glm::radians(angles.z), glm::vec3(0.0, 0.0, 1.0));
	translation = glm::translate(glm::mat4(1.0f), position);
	scaling = glm::scale(glm::mat4(1.0f), scale);

	return translation * combinedRotation * scaling;
}

void ModelScene::LoadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate|aiProcess_FlipUVs);	// triangulate mesh and flip textures on load

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
		// vertex positions
		Vertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		// vertex colors
		// check if vertex has vertex colors
		// if(mesh->HasVertexColors(i))				// TODO: check if this works!
		/*{
			vertex.color.r = mesh->mColors[i]->r;
			vertex.color.g = mesh->mColors[i]->g;
			vertex.color.b = mesh->mColors[i]->b;
			vertex.color.a = mesh->mColors[i]->a;
		}*/
		vertex.color = { 0.0f, 0.0f, 0.0f, 0.0f };

		// normals
		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		// texture coordinates
		if (mesh->mTextureCoords[0])			// TODO: might not work -> use mesh->mTextureCoords[0] instead!
		{
			vertex.texture_cords.x = mesh->mTextureCoords[0][i].x;
			vertex.texture_cords.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.texture_cords = { 0.0f, 0.0f };

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
			TextureFromFile(texPath, id);
			Texture tex(texPath, typeName, id);
			textures.push_back(tex);
			m_TexturesLoaded.push_back(tex);
		}
	}
	return textures;
}


void TextureFromFile(const std::string& path, unsigned int& id)
{
	// TODO: implement when ready
	stbi_set_flip_vertically_on_load(1);		// TODO: might be flipped with assimp
	int width;
	int height;
	int bpp;

	unsigned char* buffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);
	// glActiveTexture(texture_slot);		// texture slot that should be used
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// texture filtering 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		// mipmaps switching only used when down scaling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (buffer)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		// generate mipmap
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);		// unbind
	stbi_image_free(buffer);
}