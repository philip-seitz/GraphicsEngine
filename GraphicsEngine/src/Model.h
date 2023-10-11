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
	void InitQuad(float side = 4.0f);
	void LoadQuad(float side = 4.0f);

	unsigned int TriCount;
	Triangle* TriBuffer;
	Triangle* TriBufferPointer;
	TriangleIndices* TriIndices;
	TriangleIndices* TriIndicesPointer;

	unsigned int va;
	unsigned int vb;
	unsigned int ib;

private:

};
