#include "Renderer.h"
#include <GL/glew.h>
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include <iostream>
#include "Util.h"

// constants for vertices and textures
const unsigned int max_vertices = 4000000;
const unsigned int max_indices = max_vertices;
const unsigned int max_texture_slots = 32;				// checken

// constants for grid
const unsigned int grid_size = 1000.0f;
const float grid_line_distance = 1.0f;
const unsigned int max_grid_lines = CalculateNumberOfLines(float(grid_size), float(grid_line_distance)); 

// Model Rotation and Translation 
float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
float angleScalerX = 0.2f;
float angleScalerY = 0.4f;
float angleScalerZ = 3.0f;

float translationZ = 0.0f;

glm::vec3 lightDirection = { 0.0f, 0.0f, 1.0f };
glm::vec4 brightColor = { 0.8f, 0.1f, 0.4f, 1.0f };
glm::vec4 darkColor = { 0.8f, 0.1f, 0.4f, 0.4f };

Camera camera(true);

struct RendererData
{
	int cubeNumber = 0;
	int lightNumber = 0;

	glm::vec4 light_color = { 1.0f, 1.0f, 1.0f, 1.0f};
	glm::vec4 object_color = { 0.8f, 0.2f, 0.2f, 1.0f };

	// Models
	Model mclaren;
	Model cube;
	Model triangle;
	Model quad;

	// Other objects
	Model primitive_cube;

	ModelInstance* iMclaren = nullptr;
	ModelInstance* iCube = nullptr;
	ModelInstance* iQuad = nullptr;
	ModelInstance* iTriangle = nullptr;
	ModelInstance* iLight = nullptr;

	// Grid
	Grid grid_xz;

	GridInstance iGrid;

	// shaders and textures
	Shader* shader = nullptr;
	Shader* light_source_shader = nullptr;
	Shader* lighting_shader = nullptr;

	Texture* texture_0 = nullptr;
	Texture* texture_1 = nullptr;
	Window* window = nullptr;
};

static RendererData sData;

void Renderer::Init(Window* window)
{
	sData.window = window;
	sData.light_source_shader = new Shader("res/shader/light_source_shader.glsl");
	sData.lighting_shader = new Shader("res/shader/lighting_shader.glsl");
	sData.shader = new Shader("res/shader/vert_frag.glsl");
	glUseProgram(sData.lighting_shader->GetRendererID());
	glUseProgram(sData.light_source_shader->GetRendererID());
	glUseProgram(sData.shader->GetRendererID());
	
	sData.texture_0 = new Texture("res/textures/flappy.png", 0, GL_CLAMP_TO_EDGE);
	sData.texture_1 = new Texture("res/textures/cp2077.png", 1, GL_CLAMP_TO_EDGE);
	sData.texture_0->Bind();
	sData.texture_1->Bind();	
}

void Renderer::Shutdown()
{
	delete sData.shader;
	delete sData.light_source_shader;
	delete sData.lighting_shader;

	delete sData.texture_0;
	delete sData.texture_1;

	delete[] sData.iCube;
}

// For Batch Rendering (implement when needed -> dynamic draw)
// -----------------------------------------------------------
void Renderer::BeginBatch()
{
}

void Renderer::EndBatch()
{
}

void Renderer::Flush()
{
}
// ------------------------------------------------------------


void Renderer::CreatePrimitiveCube()
{
	sData.primitive_cube.InitSimpleCube();		// instance of primitive cube can now be created
}

void Renderer::SpawnPrimitiveCube(const int& number, const glm::vec3& pos)
{
	if (number == 1)
	{
		sData.iLight = new ModelInstance[1];
		sData.iLight[0].position = pos;
		sData.iLight[0].scale = glm::vec3(0.2f);
		sData.lightNumber = number;
	}
	else if (number > 1)
	{
		// creates square grid -> number of instances is adjusted to fit that shape
		sData.lightNumber = ModelGridLayout(&sData.iLight, number, pos);
	}
}

void Renderer::CreateCube()
{
	sData.cube.InitCube();		// instance of cube can now be created / spawned
}

void Renderer::SpawnCube(const int& number, const glm::vec3& pos)
{
	if (number == 1)
	{
		sData.iCube = new ModelInstance[1];
		sData.iCube[0].position = pos;
		sData.iCube[0].scale = glm::vec3(1.0f);
		sData.iCube[0].angles.x = 0.0f;
		sData.cubeNumber = number;
	}
	else if (number > 1)
	{
		// creates square grid -> number of instances is adjusted to fit that shape
		sData.cubeNumber = ModelGridLayout(&sData.iCube, number, pos);
	}
}

int Renderer::ModelGridLayout(ModelInstance** inst, const int& number, const glm::vec3& pos)
{
	// place multiple objects in grid

	float gap = 4.0f;
	int width = glm::sqrt(number);				// model grid width
	int height = width;							// model grid height
	int num = width * height;

	*inst = new ModelInstance[num];

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			
			int idx = j + width * i;
			(*(inst)+idx)->position = glm::vec3{ pos.x + j * gap, pos.y, pos.z + i * gap };
		}
	}
	return num;
}

void Renderer::DrawModels()
{

	// Light Source
	sData.light_source_shader->SetUniformMat4f("u_Projection", camera.ProjectionMatrix());
	sData.light_source_shader->SetUniformMat4f("u_View", camera.LookAtMatrix());
	
	int len_light = sData.lightNumber;
	for (int i = 0; i < len_light; i++)
	{
		sData.iLight[i].Draw(sData.primitive_cube, sData.light_source_shader);
	}

	// Lighting Object
	
	sData.lighting_shader->SetUniformMat4f("u_Projection", camera.ProjectionMatrix());
	sData.lighting_shader->SetUniformMat4f("u_View", camera.LookAtMatrix());
	sData.lighting_shader->SetUniform4f("u_Light_Color", sData.light_color.x, sData.light_color.y, sData.light_color.z, sData.light_color.w);
	sData.lighting_shader->SetUniform4f("u_Object_Color", sData.object_color.x, sData.object_color.y, sData.object_color.z, sData.object_color.w);

	sData.lighting_shader->SetUniform4f("u_Light_Position", sData.iLight[0].position.x, sData.iLight[0].position.y, sData.iLight[0].position.z, 1.0f);
	sData.lighting_shader->SetUniform4f("u_Camera_Position", camera.Position().x, camera.Position().y, camera.Position().z, 1.0f);

	int len_obj = sData.cubeNumber;
	for (int i = 0; i < len_obj; i++)
	{
		sData.lighting_shader->SetUniformMat3f("u_Normal_Matrix", sData.iCube[i].NormalMatrix());
		sData.iCube[i].Draw(sData.cube, sData.lighting_shader);
	}
}

void Renderer::CreateGrid()
{
	sData.grid_xz.InitGridxz();

}

void Renderer::SpawnGrid()
{
	// eventual implementation for multiple grids
}

void Renderer::DrawGrid()
{
	sData.shader->SetUniformMat4f("u_Projection", camera.ProjectionMatrix());
	sData.shader->SetUniformMat4f("u_View", camera.LookAtMatrix());
	sData.iGrid.Draw(sData.grid_xz);
}

void Renderer::Calculate(const float& dt)
{
	static bool space_press = false;
	static float texture_mix = 0.3f;
	static float time = 0.0f;

	camera.ManageInputs(*sData.window, dt);
	if (glfwGetKey(sData.window->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		space_press = true;
		
	}
	if (glfwGetKey(sData.window->GetWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE && space_press)
	{
		// Renderer::AddCubeTexture(camera.Position() + camera.LookDirection() * 7.0f, 3.0f);
		space_press = false;
	}	
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Model/Grid Loading and Initialization
// -------------------------------------------------------------------------------

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
	float side = 2.0f;

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

void Model::InitQuad()
{
	LoadQuad();

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

void Model::LoadQuad()
{
	// init and setting data
	std::vector<Triangle> mesh;

	glm::vec3 center = { 0.0f, 0.0f, 0.0f };
	float side = 4.0f;

	// two triangles = 1 quad
	mesh.push_back({ {{  center.x - side / 2, center.y - side / 2, 0.0f}, {WHITE}, {0.0f, 0.0f}}, {{center.x + side / 2, center.y - side / 2, 0.0f}, {WHITE}, {1.0f, 0.0f}}, {{center.x + side / 2, center.y + side / 2, 0.0f}, {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({ {{  center.x + side / 2, center.y + side / 2, 0.0f}, {WHITE}, {1.0f, 1.0f}}, {{center.x - side / 2, center.y + side / 2, 0.0f}, {WHITE}, {0.0f, 1.0f}}, {{center.x - side / 2, center.y - side / 2, 0.0f}, {WHITE}, {0.0f, 0.0f}} });

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

Model::~Model()
{
	glDeleteBuffers(1, &vb);
	glDeleteBuffers(1, &ib);
	delete[] TriBuffer;
	delete[] TriIndices;

	glDeleteVertexArrays(1, &vb);
}

// Model Instances 
// -------------------------------------------------------------------------------------------

glm::mat4x4 ModelInstance::ModelMatrix()
{
	glm::mat4x4 combinedRotation(0);
	glm::mat4x4 translation(0);
	glm::mat4x4 scaling(0);
	combinedRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angles.x),
		glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
			glm::radians(angles.y), glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
				glm::radians(angles.z), glm::vec3(0.0, 0.0, 1.0));
	translation = glm::translate(glm::mat4(1.0f), position);
	scaling = glm::scale(glm::mat4(1.0f), scale);

	return translation * combinedRotation * scaling;
}

glm::mat3x3 ModelInstance::NormalMatrix()
{
	return glm::transpose(glm::inverse(ModelMatrix()));
}

void ModelInstance::Draw(const Model& base_model, Shader* shader)
{
	glBindVertexArray(base_model.va);

	// Blending 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// enable textures [0 -> no; 1 -> yes]
	// shader->SetUniform1i("u_Use_Texture", 1);
	shader->SetUniformMat4f("u_Model", ModelMatrix());

	// draw all vertices 
	glDrawElements(GL_TRIANGLES, 3 * base_model.TriCount, GL_UNSIGNED_INT, nullptr);

	// === Draw the points === 
	// glDrawArrays(GL_POINTS, 0, 3 * sData.TriToRenderCount);
}

// Grid Objects 
// -------------------------------------------------------------------------------------------

void Grid::InitGridxz()
{
	LoadGridxz();

	// setup for grid lines
	glCreateVertexArrays(1, &g_va);
	glBindVertexArray(g_va);

	glCreateBuffers(1, &g_vb);
	glBindBuffer(GL_ARRAY_BUFFER, g_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * max_grid_lines, LineBuffer, GL_STATIC_DRAW);

	glCreateBuffers(1, &g_ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * max_grid_lines * 2, GridLineIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

}

void Grid::LoadGridxz()
{
	LineBuffer = new Line[max_grid_lines];
	GridLineIndices = new LineIndices[max_grid_lines];
	LineBufferPointer = LineBuffer;
	GridLineIndicesPointer = GridLineIndices;

	Line z_line = { {{0.0f, 0.0f, int(grid_size) / 2.0f}}, {{0.0f, 0.0f, -float(grid_size) / 2.0f}} };
	Line x_line = { {{float(grid_size) / 2.0f, 0.0f, 0.0f}}, {{-float(grid_size) / 2.0f, 0.0f, 0.0f}} };

	glm::vec3 interval_z = { 0.0f, 0.0f, grid_line_distance };
	glm::vec3 interval_x = { grid_line_distance, 0.0f, 0.0f };

	int j = 0;
	int k = 0;
	int l = 0;
	int limit = max_grid_lines - 2;		// minus inner x and z line plus outer lines
	for (int i = 0; i < limit; i++)
	{
		LineBufferPointer->p0.color = { GREY };
		LineBufferPointer->p1.color = { GREY };

		if (i < limit * 1 / 4)
		{
			if (i == 0)
			{
				LineBufferPointer->p0.color = { BLUE };
				LineBufferPointer->p1.color = { BLUE };
			}
			LineBufferPointer->p0.position = z_line.p0.position + interval_x * float(i);
			LineBufferPointer->p1.position = z_line.p1.position + interval_x * float(i);
		}

		else if (i < limit * 2 / 4)
		{
			LineBufferPointer->p0.position = z_line.p0.position - interval_x * float(j);
			LineBufferPointer->p1.position = z_line.p1.position - interval_x * float(j);
			j++;
		}

		else if (i < limit * 3 / 4)
		{
			if (k == 0)
			{
				LineBufferPointer->p0.color = { RED };
				LineBufferPointer->p1.color = { RED };
			}
			LineBufferPointer->p0.position = x_line.p0.position + interval_z * float(k);
			LineBufferPointer->p1.position = x_line.p1.position + interval_z * float(k);
			k++;
		}

		else
		{
			LineBufferPointer->p0.position = x_line.p0.position - interval_z * float(l);
			LineBufferPointer->p1.position = x_line.p1.position - interval_z * float(l);
			l++;
		}

		*(GridLineIndicesPointer) = { {2 * i, 2 * i + 1} };
		LineCount++;
		GridLineIndicesPointer++;
		LineBufferPointer++;
	}
}

Grid::~Grid()
{
	delete[] LineBuffer;
	delete[] GridLineIndices;
}

glm::mat4x4 GridInstance::ModelMatrix()
{
	return glm::mat4x4();
}

// Grid Instances 
// -------------------------------------------------------------------------------------------

void GridInstance::Draw(const Grid& base_model)
{
	glBindVertexArray(base_model.g_va);
	glm::mat4x4 combinedRotation(0);
	glm::mat4x4 translation(0);
	glm::mat4x4 scaling(0);

	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	combinedRotation =	glm::rotate(glm::mat4(1.0f), glm::radians(angles.x), glm::vec3(1.0, 0.0, 0.0)) * 
						glm::rotate(glm::mat4(1.0f), glm::radians(angles.y), glm::vec3(0.0, 1.0, 0.0)) * 
						glm::rotate(glm::mat4(1.0f), glm::radians(angles.z), glm::vec3(0.0, 0.0, 1.0));
	translation = glm::translate(glm::mat4(1.0f), position);
	scaling = glm::scale(glm::mat4(1.0f), scale);

	// improve appearence of the grid (anti z-fighting and thinner lines)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(1.0f);

	// enable textures [0 -> no; 1 -> yes]
	// sData.shader->SetUniform1i("u_Use_Texture", 0);
	sData.shader->SetUniformMat4f("u_Model", translation * combinedRotation * scaling);
	glDrawElements(GL_LINES, 2 * base_model.LineCount, GL_UNSIGNED_INT, nullptr);
}

// Callback functions 
// -------------------------------------------------------------------------------------------

// mouse input callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	const static float sens = 0.2f;
	static float lastx = 400;
	static float lasty = 300;
	static bool init = false;

	if (!init)
	{
		lastx = xpos;
		lasty = ypos;
		init = true;
	}

	float xoffset = (xpos - lastx) * sens;
	float yoffset = (lasty - ypos) * sens;

	camera.UpdateFromMouseCallback(xoffset, yoffset);

	lastx = xpos;
	lasty = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.UpdateFromScrollCallback((float)xoffset, (float)yoffset);
}