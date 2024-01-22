#pragma once
#include "Util.h"

class Model
{
public:
	Model();
	~Model();

	void InitMeshFromData(const std::string& filepath);
	void LoadMesh(const std::string& filepath);

	void InitCube(const bool& useTexture = true);
	void LoadCube();

	void InitSimpleCube();		// only vertex positions
	void LoadSimpleCube();
	void InitTriangle();
	void LoadTriangle();
	void InitQuad(float side = 1.0f, glm::vec3 center = {0.0f, 0.0f, 0.0f});
	void AddInstancedArray(glm::vec2* offsets);
	void LoadQuad(float side = 1.0f, glm::vec3 center = { 0.0f, 0.0f, 0.0f });
	void InitPoint();
	void LoadPoint();

	unsigned int TriCount;
	Triangle* TriBuffer;
	Triangle* TriBufferPointer;
	TriangleIndices* TriIndices;
	TriangleIndices* TriIndicesPointer;
	Vertex* VertexBuffer;

	unsigned int va;
	unsigned int vb;
	unsigned int ib;
	unsigned int ivb;	// ivb = instanced vertex buffer (for instanced array)
private:

};
