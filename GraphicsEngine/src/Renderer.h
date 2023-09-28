#pragma once
#include <vector>
#include "Window.h"
#include "Shader.h"
#include <glm/glm.hpp>

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

struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texture_cords;
	glm::vec3 normal;
};

struct Triangle
{

	Vertex p0, p1, p2;
	void SetColor(const float& r, const float& g, const float& b, const float& a)
	{
		p0.color = {r, g, b, a};
		p1.color = { r, g, b, a };
		p2.color = { r, g, b, a };

	}

	void SetColor(glm::vec4 color)
	{
		p0.color = color;
		p1.color = color;
		p2.color = color;
	}

	void SetNormal()
	{
		glm::vec3 p01 = p1.position - p0.position;
		glm::vec3 p02 = p2.position - p0.position;
		glm::vec3 normal = glm::normalize(glm::cross(p01, p02));

		p0.normal = normal;
		p1.normal = normal;
		p2.normal = normal;
	}
};

struct TriangleIndices
{
	glm::highp_uvec3 indices;
};

struct Line
{
	Vertex p0, p1;
	void SetColor(const float& r, const float& g, const float& b, const float& a)
	{
		p0.color = { r, g, b, a };
		p1.color = { r, g, b, a };
	}

	void SetColor(glm::vec4 color)
	{
		p0.color = color;
		p1.color = color;
	}

	void SetColor(glm::vec4 color1, glm::vec4 color2)
	{
		p0.color = color1;
		p1.color = color2;
	}

};

struct LineIndices
{
	glm::highp_uvec2 indices;
};

struct Model
{
	unsigned int TriCount = 0;
	Triangle* TriBuffer = nullptr;
	Triangle* TriBufferPointer = nullptr;
	TriangleIndices* TriIndices = nullptr;
	TriangleIndices* TriIndicesPointer = nullptr;

	unsigned int va = 0;
	unsigned int vb = 0;
	unsigned int ib = 0;

	void InitMeshFromData(const std::string& filepath);
	void LoadMesh(const std::string& filepath);

	void InitCube(const bool& useTexture = true);
	void LoadCube();

	void InitSimpleCube();		// only vertex positions
	void LoadSimpleCube();
	void InitTriangle();
	void LoadTriangle();
	void InitQuad();
	void LoadQuad();
	~Model();

};

struct ModelInstance
{
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 angles = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	glm::vec3 color = { 0.5f, 0.5f, 0.5f };

	glm::mat4x4 ModelMatrix();
	glm::mat3x3 NormalMatrix();
	void Draw(const Model& base_model, Shader* shader);
};

struct Grid
{
	unsigned int LineCount = 0;
	
	Line* LineBuffer = nullptr;
	Line* LineBufferPointer = nullptr;
	LineIndices* GridLineIndices = nullptr;
	LineIndices* GridLineIndicesPointer = nullptr;

	unsigned int g_va = 0;
	unsigned int g_vb = 0;
	unsigned int g_ib = 0;

	void InitGridxz();
	void LoadGridxz();

	~Grid();
};

struct GridInstance
{
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 angles = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4x4 ModelMatrix();
	void Draw(const Grid& base_model);

};


class Renderer
{
public:
	// static class (keine Instanz)

	static void Init(Window* window);
	static void Shutdown();

	static void BeginBatch();
	static void EndBatch();
	static void Flush();

	// static void CreateModelFromData(const std::string& filepath);
	static void CreatePrimitiveCube();
	static void SpawnPrimitiveCube(const int& number, const glm::vec3& pos);

	static void CreateCube();
	static void SpawnCube(const int& number, const glm::vec3& pos);

	static void CreateGrid();
	static void SpawnGrid();

	static int ModelGridLayout(ModelInstance** inst, const int& number, const glm::vec3& pos);
	static void DrawModels();
	static void DrawGrid();

	static void Calculate(const float& dt);
	static void Clear();

private:
	// stats o.ä. zum Debuggen
};