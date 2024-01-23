#include "Model.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define RED 0.8f, 0.3f, 0.3f, 1.0f
#define GREEN 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE 0.3f, 0.3f, 0.7f, 1.0f
#define PURPLE 0.6f, 0.0f, 0.8f, 1.0f
#define ORANGE 0.9f, 0.3f, 0.3f, 1.0f
#define B_BLUE 0.0f, 0.5f, 0.9f, 1.0f
#define B_BLUE_SHADE 0.0f, 0.3f, 0.7f, 1.0f
#define B_BLUE_DARK 0.0f, 0.1f, 0.5f, 1.0f
#define WHITE 1.0f, 1.0f, 1.0f, 1.0f
#define GREY 0.3f, 0.3f, 0.3f, 0.1f 


Model::Model() :
	TriCount(0),
	va(0),
	vb(0),
	ib(0),
	ivb(0),
	TriBuffer(nullptr),
	TriBufferPointer(nullptr),
	TriIndices(nullptr),
	TriIndicesPointer(nullptr),
	VertexBuffer(nullptr)
{
}

Model::~Model()
{
	glDeleteBuffers(1, &vb);
	glDeleteBuffers(1, &ib);
	delete[] TriBuffer;
	delete[] TriIndices;
	delete[] VertexBuffer;

	glDeleteVertexArrays(1, &vb);
}


void Model::InitMeshFromData(const std::string& filepath)
{
	LoadMesh(filepath);

	// vertex array object setup
	glCreateVertexArrays(1, &va);
	glBindVertexArray(va);

	glCreateBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * TriCount, TriBuffer, GL_STATIC_DRAW);

	glCreateBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TriCount * 3, TriIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texture_cords));
}

void Model::InitSimpleCube()
{
	LoadSimpleCube();
	// vertex array object setup
	glCreateVertexArrays(1, &va);
	glBindVertexArray(va);

	glCreateBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * TriCount, TriBuffer, GL_STATIC_DRAW);

	glCreateBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TriCount * 3, TriIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

}
void Model::LoadSimpleCube()
{
	std::vector<Triangle> mesh;

	glm::vec3 center = { 0.0f, 0.0f, 0.0f };
	float side = 1.0f;

	glm::vec3 front_center = center + glm::vec3(0.0f, 0.0f, side / 2.0f);
	glm::vec3 back_center = center - glm::vec3(0.0f, 0.0f, side / 2.0f);
	glm::vec3 right_center = center + glm::vec3(side / 2.0f, 0.0f, 0.0f);
	glm::vec3 left_center = center - glm::vec3(side / 2.0f, 0.0f, 0.0f);
	glm::vec3 top_center = center + glm::vec3(0.0f, side / 2.0f, 0.0f);
	glm::vec3 bottom_center = center - glm::vec3(0.0f, side / 2.0f, 0.0f);


	// NORTH (back)
	mesh.push_back({ {back_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f)},
					{back_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f)},
					{back_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f)} });

	mesh.push_back({ {back_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f)},
					{back_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f)},
					{back_center + glm::vec3(side / 2.0f,  side / 2.0f, 0.0f)} });

	// SOUTH (front)
	mesh.push_back({ {front_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f)},
					{front_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f)},
					{front_center + glm::vec3(side / 2.0f, side / 2.0f, 0.0f)} });

	mesh.push_back({ {front_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f)},
					{front_center + glm::vec3(side / 2.0f, side / 2.0f, 0.0f)},
					{front_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f)} });

	// EAST (right)
	mesh.push_back({ {right_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f)},
					{right_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f)},
					{right_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f)} });

	mesh.push_back({ {right_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f)},
					{right_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f)},
					{right_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f)} });

	// WEST (left)
	mesh.push_back({ {left_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f)},
					{left_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f)},
					{left_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f)} });

	mesh.push_back({ {left_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f)},
					{left_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f)},
					{left_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f)} });

	// BOTTOM 
	mesh.push_back({ {bottom_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f)},
					{bottom_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f)},
					{bottom_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f)} });

	mesh.push_back({ {bottom_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f)},
					{bottom_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f)},
					{bottom_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f)} });

	// TOP 
	mesh.push_back({ {top_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f)},
					{top_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f)},
					{top_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f)} });

	mesh.push_back({ {top_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f)},
					{top_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f)},
					{top_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f)} });

	// allocating buffers
	int triNumber = mesh.size();
	TriBuffer = new Triangle[triNumber];
	TriIndices = new TriangleIndices[triNumber];
	TriBufferPointer = TriBuffer;
	TriIndicesPointer = TriIndices;

	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		mesh[i].SetNormal();
		*(TriBufferPointer) = mesh[i];
		*(TriIndicesPointer) = { {0 + 3 * (TriCount), 1 + 3 * (TriCount), 2 + 3 * (TriCount)} };
		TriBufferPointer++;
		TriIndicesPointer++;
		TriCount++;
	}
}

void Model::InitCube(const bool& useTexture)
{
	LoadCube();

	// vertex array object setup
	glCreateVertexArrays(1, &va);
	glBindVertexArray(va);

	glCreateBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * TriCount, TriBuffer, GL_STATIC_DRAW);

	glCreateBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TriCount * 3, TriIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texture_cords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
}

void Model::LoadCube()
{
	std::vector<Triangle> mesh;

	glm::vec3 center = { 0.0f, 0.0f, 0.0f };
	float side = 2.0f;

	glm::vec3 front_center = center + glm::vec3(0.0f, 0.0f, side / 2.0f);
	glm::vec3 back_center = center - glm::vec3(0.0f, 0.0f, side / 2.0f);
	glm::vec3 right_center = center + glm::vec3(side / 2.0f, 0.0f, 0.0f);
	glm::vec3 left_center = center - glm::vec3(side / 2.0f, 0.0f, 0.0f);
	glm::vec3 top_center = center + glm::vec3(0.0f, side / 2.0f, 0.0f);
	glm::vec3 bottom_center = center - glm::vec3(0.0f, side / 2.0f, 0.0f);


	// NORTH (back)
	mesh.push_back({ {back_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f),		{WHITE}, {0.0f, 0.0f}},
					{back_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f), {WHITE}, {1.0f, 0.0f}},
					{back_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f),	{WHITE}, {1.0f, 1.0f}} });

	mesh.push_back({ {back_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f),	{WHITE}, {0.0f, 0.0f}},
					{back_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f),	{WHITE}, {1.0f, 1.0f}},
					{back_center + glm::vec3(side / 2.0f,  side / 2.0f, 0.0f),	{WHITE}, {0.0f, 1.0f}} });

	// SOUTH (front)
	mesh.push_back({ {front_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f),	{WHITE}, {0.0f, 0.0f}},
					{front_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f),		{WHITE}, {1.0f, 0.0f}},
					{front_center + glm::vec3(side / 2.0f, side / 2.0f, 0.0f),		{WHITE}, {1.0f, 1.0f}} });

	mesh.push_back({ {front_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f),	{WHITE}, {0.0f, 0.0f}},
					{front_center + glm::vec3(side / 2.0f, side / 2.0f, 0.0f),		{WHITE}, {1.0f, 1.0f}},
					{front_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f),		{WHITE}, {0.0f, 1.0f}} });

	// EAST (right)
	mesh.push_back({ {right_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}},
					{right_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f), {WHITE}, {1.0f, 0.0f}},
					{right_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}} });

	mesh.push_back({ {right_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}},
					{right_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}},
					{right_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

	// WEST (left)
	mesh.push_back({ {left_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}},
					{left_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f), {WHITE}, {1.0f, 0.0f}},
					{left_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({ {left_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}},
					{left_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}},
					{left_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

	// BOTTOM 
	mesh.push_back({ {bottom_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}},
					{bottom_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {1.0f, 0.0f}},
					{bottom_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({ {bottom_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}},
					{bottom_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}},
					{bottom_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

	// TOP 
	mesh.push_back({ {top_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}},
					{top_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {1.0f, 0.0f}},
					{top_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({ {top_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}},
					{top_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}},
					{top_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

	// allocating buffers
	int triNumber = mesh.size();
	TriBuffer = new Triangle[triNumber];
	TriIndices = new TriangleIndices[triNumber];
	TriBufferPointer = TriBuffer;
	TriIndicesPointer = TriIndices;

	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		mesh[i].SetNormal();
		*(TriBufferPointer) = mesh[i];
		*(TriIndicesPointer) = { {0 + 3 * (TriCount), 1 + 3 * (TriCount), 2 + 3 * (TriCount)} };
		TriBufferPointer++;
		TriIndicesPointer++;
		TriCount++;
	}
}

void Model::InitTriangle()
{
	LoadTriangle();

	// vertex array object setup
	glCreateVertexArrays(1, &va);
	glBindVertexArray(va);

	glCreateBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * TriCount, TriBuffer, GL_STATIC_DRAW);

	glCreateBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TriCount * 3, TriIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texture_cords));
}

void Model::LoadTriangle()
{
	// init and setting data
	std::vector<Triangle> mesh;

	glm::vec3 center = { 0.0f, 0.0f, 0.0f };
	float side = 4.0f;

	// one triangle
	mesh.push_back({ {{  center.x - side / 2, center.y - side / 2, 0.0f}, {WHITE}, {0.0f, 0.0f}}, {{center.x + side / 2, center.y - side / 2, 0.0f}, {WHITE}, {1.0f, 0.0f}}, {{center.x, center.y + side / 2, 0.0f}, {WHITE}, {0.5f, 1.0f}} });

	// allocating buffers
	int triNumber = mesh.size();
	TriBuffer = new Triangle[triNumber];
	TriIndices = new TriangleIndices[triNumber];
	TriBufferPointer = TriBuffer;
	TriIndicesPointer = TriIndices;

	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		mesh[i].SetNormal();
		*(TriBufferPointer) = mesh[i];
		*(TriIndicesPointer) = { {0 + 3 * (TriCount), 1 + 3 * (TriCount), 2 + 3 * (TriCount)} };
		TriBufferPointer++;
		TriIndicesPointer++;
		TriCount++;
	}
}



void Model::AddInstancedArray(glm::vec2* offsets)
{
	glBindVertexArray(va);
	if (offsets != nullptr)
	{
		glCreateBuffers(1, &ivb);
		glBindBuffer(GL_ARRAY_BUFFER, ivb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &(*offsets)[0], GL_STATIC_DRAW);		// TODO: might not work like this
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, ivb);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
		glVertexAttribDivisor(3, 1);		// update attrib for every new instance (in vertex shader)
	}

}


void Model::InitQuad(float side, glm::vec3 center)
{
	LoadQuad(side, center);

	// vertex array object setup
	glCreateVertexArrays(1, &va);
	glBindVertexArray(va);

	glCreateBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * TriCount, TriBuffer, GL_STATIC_DRAW);

	glCreateBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TriCount * 3, TriIndices, GL_STATIC_DRAW);

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

void Model::LoadQuad(float side, glm::vec3 center)
{
	// init and setting data
	std::vector<Triangle> mesh;

	// two triangles = 1 quad
	mesh.push_back({ {{  center.x - side / 2, center.y - side / 2, center.z}, {WHITE}, {0.0f, 0.0f}}, {{center.x + side / 2, center.y - side / 2, center.z}, {WHITE}, {1.0f, 0.0f}}, {{center.x + side / 2, center.y + side / 2, center.z}, {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({ {{  center.x + side / 2, center.y + side / 2, center.z}, {WHITE}, {1.0f, 1.0f}}, {{center.x - side / 2, center.y + side / 2, center.z}, {WHITE}, {0.0f, 1.0f}}, {{center.x - side / 2, center.y - side / 2, center.z}, {WHITE}, {0.0f, 0.0f}} });

	// indices to match notation in book
	
	// calculate tangent and bitangent for normal mapping
	for (int i = 0; i < 2; i++)
	{
		glm::vec3 p1 = mesh[i].p0.position;
		glm::vec3 p2 = mesh[i].p1.position;
		glm::vec3 p3 = mesh[i].p2.position;

		glm::vec2 uv1 = mesh[i].p0.texture_cords;
		glm::vec2 uv2 = mesh[i].p1.texture_cords;
		glm::vec2 uv3 = mesh[i].p2.texture_cords;

		glm::vec3 e1 = p2 - p1;
		glm::vec3 e2 = p3 - p1;
		float du1 = uv2.x - uv1.x;
		float dv1 = uv2.y - uv1.y;
		float du2 = uv3.x - uv1.x;
		float dv2 = uv3.y - uv1.y;

		float f = 1.0 / (du1 * dv2 - du2 * dv1);
		glm::vec3 t = { 0.0f, 0.0f, 0.0f };
		t.x = f * (dv2 * e1.x - dv1 * e2.x);
		t.y = f * (dv2 * e1.y - dv1 * e2.y);
		t.z = f * (dv2 * e1.z - dv1 * e2.z);
		glm::vec3 bt = { 0.0f, 0.0f, 0.0f };
		bt.x = f * (-du2 * e1.x + du1 * e2.x);
		bt.y = f * (-du2 * e1.y + du1 * e2.y);
		bt.z = f * (-du2 * e1.y + du1 * e2.z);
		
		mesh[i].p0.tangent = t;
		mesh[i].p1.tangent = t;
		mesh[i].p2.tangent = t;

		mesh[i].p0.bitangent = bt;
		mesh[i].p1.bitangent = bt;
		mesh[i].p2.bitangent = bt;
	}

	// allocating buffers
	int triNumber = mesh.size();
	TriBuffer = new Triangle[triNumber];
	TriIndices = new TriangleIndices[triNumber];
	TriBufferPointer = TriBuffer;
	TriIndicesPointer = TriIndices;

	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		mesh[i].SetNormal();
		*(TriBufferPointer) = mesh[i];
		*(TriIndicesPointer) = { {0 + 3 * (TriCount), 1 + 3 * (TriCount), 2 + 3 * (TriCount)} };
		TriBufferPointer++;
		TriIndicesPointer++;
		TriCount++;
	}
}

void Model::InitPoint()
{
	LoadPoint();

	// vertex array object setup
	glCreateVertexArrays(1, &va);
	glBindVertexArray(va);

	glCreateBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex), VertexBuffer, GL_STATIC_DRAW);

	/*glCreateBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TriCount * 3, TriIndices, GL_STATIC_DRAW);*/

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));
}

void Model::LoadPoint()
{
	Vertex vertex;
	vertex.position = { 0.0f, 0.0f, 0.0f };
	vertex.color = { 1.0f, 0.0f, 0.0f, 1.0f };			// red

	VertexBuffer = new Vertex[1];
	VertexBuffer[0] = vertex;
}

void Model::LoadMesh(const std::string& filepath)
{
	std::vector <float>vert;
	std::vector<unsigned int> ind;
	glm::vec4 color = { PURPLE };
	std::vector<glm::vec3> mesh;
	std::vector< glm::highp_uvec3> indices;
	ReadVertexData(filepath, vert, ind);
	// put the data into the triangle structures...
	int vertexNumber = vert.size() / 3;			// number of unique vertex positions
	int triNumber = ind.size() / 3;

	// allocating buffers (CPU)
	TriBuffer = new Triangle[triNumber];
	TriIndices = new TriangleIndices[triNumber];
	TriBufferPointer = TriBuffer;
	TriIndicesPointer = TriIndices;

	for (int i = 0; i < vertexNumber; i++)
	{
		glm::vec3 pos;
		pos = { vert[0 + i * 3], vert[1 + i * 3], vert[2 + i * 3] };
		mesh.push_back(pos);
	}

	for (int i = 0; i < triNumber; i++)
	{
		glm::highp_uvec3 idx;
		idx = { ind[0 + i * 3], ind[1 + i * 3], ind[2 + i * 3] };
		indices.push_back(idx);
	}

	for (int i = 0; i < triNumber; i++)
	{
		*(TriBufferPointer) = Triangle{ {mesh[indices[i][0] - 1], color}, {mesh[indices[i][1] - 1], color}, {mesh[indices[i][2] - 1], color} };
		TriBufferPointer->SetNormal();
		*(TriIndicesPointer) = { {0 + 3 * (TriCount), 1 + 3 * (TriCount), 2 + 3 * (TriCount)} };
		TriBufferPointer++;
		TriIndicesPointer++;
		TriCount++;
	}
}
