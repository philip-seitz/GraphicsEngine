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
float angleScalerZ = 0.6f;

float translationZ = 0.0f;

glm::vec3 lightDirection = { 0.0f, 0.0f, 1.0f };
glm::vec4 brightColor = { 0.8f, 0.1f, 0.4f, 1.0f };
glm::vec4 darkColor = { 0.8f, 0.1f, 0.4f, 0.4f };

Camera camera;

struct RendererData
{
	// mesh/models and or cubes
	unsigned int va = 0;
	unsigned int vb = 0;
	unsigned int ib = 0;

	unsigned int TriCount = 0;
	Triangle* TriBuffer = nullptr;
	Triangle* TriBufferPointer = nullptr;
	TriangleIndices* TriIndices = nullptr;
	TriangleIndices* TriIndicesPointer = nullptr;

	// grid
	unsigned int g_va = 0;
	unsigned int g_vb = 0;
	unsigned int g_ib = 0;

	unsigned int LineCount = 0;
	Line* LineBuffer = nullptr;
	Line* LineBufferPointer = nullptr;
	LineIndices* GridLineIndices = nullptr;
	LineIndices* GridLineIndicesPointer = nullptr;

	// shaders and textures
	Shader* shader = nullptr;
	Texture* texture = nullptr;

	// TODO: Refactor OpenGL objects and settings into Class for example
};

static RendererData sData;

void Renderer::Init()
{
	sData.shader = new Shader("res/shader/vert_frag.glsl");
	glUseProgram(sData.shader->GetRendererID());
	
	sData.texture = new Texture("res/textures/cp2077.png");
	sData.texture->Bind();

	// Structures for all triangles
	sData.TriBuffer = new Triangle[max_vertices / 3];
	sData.TriIndices = new TriangleIndices[max_vertices];
	sData.TriBufferPointer = sData.TriBuffer;
	sData.TriIndicesPointer = sData.TriIndices;

	// structures for grid lines
	sData.LineBuffer = new Line[max_grid_lines];
	sData.GridLineIndices = new LineIndices[max_grid_lines];
	sData.LineBufferPointer = sData.LineBuffer;
	sData.GridLineIndicesPointer = sData.GridLineIndices;

	// initialize models and grid
	LoadMesh();
	AddGrid();

	// setup for triangles
	glCreateVertexArrays(1, &sData.va);
	glBindVertexArray(sData.va);

	glCreateBuffers(1, &sData.vb);
	glBindBuffer(GL_ARRAY_BUFFER, sData.vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * max_vertices, sData.TriBuffer, GL_STATIC_DRAW);

	glCreateBuffers(1, &sData.ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * max_indices, sData.TriIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texture_cords));

	// setup for grid lines
	glCreateVertexArrays(1, &sData.g_va);
	glBindVertexArray(sData.g_va);

	glCreateBuffers(1, &sData.g_vb);
	glBindBuffer(GL_ARRAY_BUFFER, sData.g_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * max_grid_lines, sData.LineBuffer, GL_STATIC_DRAW);

	glCreateBuffers(1, &sData.g_ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.g_ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * max_grid_lines*2, sData.GridLineIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));
}

void Renderer::Shutdown()
{
	glDeleteBuffers(1, &sData.vb);
	glDeleteBuffers(1, &sData.ib);
	delete[] sData.TriBuffer;
	delete[] sData.TriIndices;


	delete[] sData.LineBuffer; 
	delete[] sData.GridLineIndices;

	glDeleteVertexArrays(1, &sData.vb);

	glDeleteProgram(sData.shader->GetRendererID());
	delete sData.shader;

	delete sData.texture;
}

void Renderer::BeginBatch()
{
}

void Renderer::EndBatch()
{
}

void Renderer::Flush()
{
}

void Renderer::AddGrid()
{
	Line z_line = { {{0.0f, 0.0f, int(grid_size)/2.0f}}, {{0.0f, 0.0f, -float(grid_size)/2.0f}} };
	Line x_line = { {{float(grid_size)/2.0f, 0.0f, 0.0f}}, {{-float(grid_size)/2.0f, 0.0f, 0.0f}} };

	glm::vec3 interval_z = { 0.0f, 0.0f, grid_line_distance };
	glm::vec3 interval_x = { grid_line_distance, 0.0f, 0.0f };

	int j = 0;
	int k = 0;
	int l = 0;
	int limit = max_grid_lines - 2;		// minus inner x and z line plus outer lines
	for (int i = 0; i < limit; i++)
	{
		sData.LineBufferPointer->p0.color = { GREY };
		sData.LineBufferPointer->p1.color = { GREY };

		if (i < limit * 1/4)
		{
			if (i == 0)
			{
				sData.LineBufferPointer->p0.color = { BLUE };
				sData.LineBufferPointer->p1.color = { BLUE };
			}
			sData.LineBufferPointer->p0.position = z_line.p0.position + interval_x * float(i);
			sData.LineBufferPointer->p1.position = z_line.p1.position + interval_x * float(i);
		}

		else if (i < limit * 2/4)
		{
			sData.LineBufferPointer->p0.position = z_line.p0.position - interval_x * float(j);
			sData.LineBufferPointer->p1.position = z_line.p1.position - interval_x * float(j);
			j++;
		}

		else if (i < limit * 3/4)
		{
			if (k == 0)
			{
				sData.LineBufferPointer->p0.color = { RED };
				sData.LineBufferPointer->p1.color = { RED };
			}
			sData.LineBufferPointer->p0.position = x_line.p0.position + interval_z * float(k);
			sData.LineBufferPointer->p1.position = x_line.p1.position + interval_z * float(k);
			k++;
		}

		else 
		{
			sData.LineBufferPointer->p0.position = x_line.p0.position - interval_z * float(l);
			sData.LineBufferPointer->p1.position = x_line.p1.position - interval_z * float(l);
			l++;
		}
		*(sData.GridLineIndicesPointer) = { {2*i, 2*i + 1} };
		sData.LineCount++;
		sData.GridLineIndicesPointer++;
		sData.LineBufferPointer++;
	}
}


void Renderer::AddTriangle(const float& x, const float& y, const float& size)
{
	// init and setting data
	std::vector<Triangle> mesh;

	// one triangle
	mesh.push_back({ {{  x, y, 0.0f}}, {{x + size, y, 0.0f}}, {{x + size, y + size, 0.0f}} });

	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		*(sData.TriBufferPointer) = mesh[i];
		*(sData.TriIndicesPointer) = { {0 + 3 * (sData.TriCount), 1 + 3 * (sData.TriCount), 2 + 3 * (sData.TriCount)} };
		sData.TriBufferPointer++;
		sData.TriIndicesPointer++;
		sData.TriCount++;
	}
}

void Renderer::AddQuad(const float& x, const float& y, const float& size)
{
	// init and setting data
	std::vector<Triangle> mesh;

	// two triangles = 1 quad
	mesh.push_back({ {{  x, y, 0.0f}}, {{x + size, y, 0.0f}}, {{x + size, y + size, 0.0f}} });
	mesh.push_back({ {{  x + size, y + size, 0.0f}}, {{x, y + size, 0.0f}}, {{x, y, 0.0f}} });

	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		*(sData.TriBufferPointer) = mesh[i];
		*(sData.TriIndicesPointer) = {{0 + 3 * (sData.TriCount), 1 + 3 * (sData.TriCount), 2 + 3 * (sData.TriCount)}};
		sData.TriBufferPointer++;
		sData.TriIndicesPointer++;
		sData.TriCount++;
	}
}

void Renderer::AddCube(const float& x, const float& y, const float& z, const float& size)
{
	std::vector<Triangle> mesh;

	//// NORTH (good)
	mesh.push_back({ {{0.25f, -0.25f, -0.25f}}, { {-0.25f, -0.25f, -0.25f}}, {{-0.25f, 0.25f, -0.25f}} });
	mesh.push_back({ {{0.25f, -0.25f, -0.25f}}, {{  -0.25f, 0.25f, -0.25f}}, {{0.25f, 0.25f, -0.25f}} });

	// SOUTH (good)
	mesh.push_back({ {{-0.25f, -0.25f, 0.25f}}, {{  0.25f, -0.25f, 0.25f}}, {{0.25f, 0.25f, 0.25f}} });
	mesh.push_back({ {{-0.25f, -0.25f, 0.25f}}, {{  0.25f, 0.25f, 0.25f}}, {{-0.25f, 0.25f, 0.25f}} });

	// EAST (good)
	mesh.push_back({ {{0.25f, -0.25f, 0.25f}}, {{  0.25f, -0.25f, -0.25f}}, {{0.25f, 0.25f, -0.25f}} });
	mesh.push_back({ {{0.25f, -0.25f, 0.25f}}, {{  0.25f, 0.25f, -0.25f}}, {{0.25f, 0.25f, 0.25f}} });

	// WEST (good)
	mesh.push_back({ {{-0.25f, -0.25f, -0.25f}}, {{  -0.25f, -0.25f, 0.25f}}, {{-0.25f, 0.25f, 0.25f}} });
	mesh.push_back({ {{-0.25f, -0.25f, -0.25f}}, {{  -0.25f, 0.25f, 0.25f}}, {{-0.25f, 0.25f, -0.25f}} });

	// BOTTOM 
	mesh.push_back({ {{-0.25f, -0.25f, -0.25f}}, {{  0.25f, -0.25f, -0.25f}}, {{0.25f, -0.25f, 0.25f}} });
	mesh.push_back({ {{-0.25f, -0.25f, -0.25f}}, {{  0.25f, -0.25f, 0.25f}}, {{-0.25f, -0.25f, 0.25f}} });

	// TOP 
	mesh.push_back({ {{-0.25f, 0.25f, 0.25f}}, {{  0.25f, 0.25f, 0.25f}}, {{0.25f, 0.25f, -0.25f}} });
	mesh.push_back({ {{-0.25f, 0.25f, 0.25f}}, {{  0.25f, 0.25f, -0.25f}}, {{-0.25f, 0.25f, -0.25f}} });

	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		*(sData.TriBufferPointer) = mesh[i];
		*(sData.TriIndicesPointer) = { {0 + 3 * (sData.TriCount), 1 + 3 * (sData.TriCount), 2 + 3 * (sData.TriCount)} };
		sData.TriBufferPointer++;
		sData.TriIndicesPointer++;
		sData.TriCount++;
	}
}

void Renderer::AddCubeTexture(const glm::vec3& center, const float& side)
{
	std::vector<Triangle> mesh;

	glm::vec3 front_center = center + glm::vec3(0.0f, 0.0f, side/2.0f);
	glm::vec3 back_center = center - glm::vec3(0.0f, 0.0f, side / 2.0f);
	glm::vec3 right_center = center + glm::vec3(side / 2.0f, 0.0f, 0.0f);
	glm::vec3 left_center = center - glm::vec3(side / 2.0f, 0.0f, 0.0f);
	glm::vec3 top_center = center + glm::vec3(0.0f, side / 2.0f, 0.0f);
	glm::vec3 bottom_center = center - glm::vec3(0.0f, side / 2.0f, 0.0f);


	// NORTH (back)
	mesh.push_back({{back_center + glm::vec3(side/2.0f, -side/2.0f, 0.0f),		{WHITE}, {1.0f, 0.0f}},
					{back_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f), {WHITE}, {0.0f, 0.0f}},
					{back_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f),	{WHITE}, {0.0f, 1.0f}}});

	mesh.push_back({{back_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f),	{WHITE}, {1.0f, 0.0f}}, 
					{back_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f),	{WHITE}, {0.0f, 1.0f}}, 
					{back_center + glm::vec3(side / 2.0f,  side / 2.0f, 0.0f),	{WHITE}, {1.0f, 1.0f}} });

	// SOUTH (front)
	mesh.push_back({{front_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f),	{WHITE}, {0.0f, 0.0f}}, 
					{front_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f),		{WHITE}, {1.0f, 0.0f}},
					{front_center + glm::vec3(side / 2.0f, side / 2.0f, 0.0f),		{WHITE}, {1.0f, 1.0f}} });

	mesh.push_back({{front_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f),	{WHITE}, {0.0f, 0.0f}}, 
					{front_center + glm::vec3(side / 2.0f, side / 2.0f, 0.0f),		{WHITE}, {1.0f, 1.0f}}, 
					{front_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f),		{WHITE}, {0.0f, 1.0f}} });

	// EAST (right)
	mesh.push_back({{right_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}}, 
					{right_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f), {WHITE}, {1.0f, 0.0f}}, 
					{right_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}} });

	mesh.push_back({{right_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}}, 
					{right_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}}, 
					{right_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

	// WEST (left)
	mesh.push_back({{left_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}}, 
					{left_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f), {WHITE}, {1.0f, 0.0f}}, 
					{left_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({{left_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}}, 
					{left_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}}, 
					{left_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

	// BOTTOM 
	mesh.push_back({{bottom_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}}, 
					{bottom_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {1.0f, 0.0f}},
					{bottom_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({{bottom_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}}, 
					{bottom_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}}, 
					{bottom_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

	// TOP 
	mesh.push_back({{top_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}}, 
					{top_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {1.0f, 0.0f}}, 
					{top_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({{top_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}}, 
					{top_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}}, 
					{top_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		*(sData.TriBufferPointer) = mesh[i];
		*(sData.TriIndicesPointer) = { {0 + 3 * (sData.TriCount), 1 + 3 * (sData.TriCount), 2 + 3 * (sData.TriCount)} };
		sData.TriBufferPointer++;
		sData.TriIndicesPointer++;
		sData.TriCount++;
	}
}

void Renderer::LoadMesh()
{
	std::vector <float>vert;
	std::vector<unsigned int> ind;
	glm::vec4 color = { PURPLE };
	std::vector<glm::vec3> mesh;
	std::vector< glm::highp_uvec3> indices;
	ReadVertexData("res/objects/McLaren_Senna.obj", vert, ind);
	// put the data into the triangle structures...
	int vertexNumber = vert.size() / 3;
	int triNumber = ind.size() / 3;

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
		*(sData.TriBufferPointer) = Triangle{ {mesh[indices[i][0] -1], color}, {mesh[indices[i][1] - 1], color}, {mesh[indices[i][2] - 1], color} };
		*(sData.TriIndicesPointer) = { {0 + 3 * (sData.TriCount), 1 + 3 * (sData.TriCount), 2 + 3 * (sData.TriCount)} };
		sData.TriBufferPointer++;
		sData.TriIndicesPointer++;
		sData.TriCount++;
	}
}

void Renderer::DrawTriangles()								
{
	glBindVertexArray(sData.va);

	// === Code for Dynamic Draw ===
	// glBindBuffer(GL_ARRAY_BUFFER, sData.vb);
	// glBufferSubData(GL_ARRAY_BUFFER, 0, sData.TriCount * sizeof(Triangle), sData.TriBuffer);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.ib);
	// glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sData.TriCount * sizeof(TriangleIndices), sData.TriIndices);

	// === Enable wireframe drawing === 
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Blending 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// === Enable point smoothing and set the point size ====
	// glEnable(GL_POINT_SMOOTH);
	// glPointSize(10.0f);

	// enable textures [0 -> no; 1 -> yes]
	sData.shader->SetUniform1i("u_Use_Texture", 0);

	// Draw the triangles' outlines
	glDrawElements(GL_TRIANGLES, 3 * sData.TriCount, GL_UNSIGNED_INT, nullptr);

	// === Draw the points === 
	// glDrawArrays(GL_POINTS, 0, 3 * sData.TriToRenderCount);
}

void Renderer::DrawGrid()
{
	glBindVertexArray(sData.g_va);

	// === Code for Dynamic Draw ===
	// glBindBuffer(GL_ARRAY_BUFFER, sData.g_vb);
	// glBufferSubData(GL_ARRAY_BUFFER, 0, sData.LineCount * sizeof(Line), sData.LineBuffer);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.g_ib);
	// glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sData.LineCount * sizeof(LineIndices), sData.GridLineIndices);

	// improve appearence of the grid (anti z-fighting and thinner lines)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glLineWidth(1.0f);

	// enable textures [0 -> no; 1 -> yes]
	sData.shader->SetUniform1i("u_Use_Texture", 0);
	glDrawElements(GL_LINES, 2*sData.LineCount, GL_UNSIGNED_INT, nullptr);
}

void Renderer::Calculate(const float& dt, Window& window)
{
	static bool space_press = false;

	glm::mat4x4 model(0);
	glm::mat4x4 combinedRotation(0);
	glm::mat4x4 translation(0);
	if ((angleX + angleScalerX * dt) >= 2.0f * 3.14f)
		angleX = 0.0f;
	else
		angleX = angleX + angleScalerX * dt;

	if (angleY + angleScalerY * dt >= 2.0f * 3.14f)
		angleY = 0.0f;
	else
		angleY = angleY + angleScalerY * dt;

	if (angleZ + angleScalerZ * dt >= 2.0f * 3.14f)
		angleZ = 0.0f;
	else
		angleZ = angleZ + angleScalerZ * dt;

	translationZ = translationZ + 0.01 * dt;

	
	camera.ManageInputs(window, dt);
	if (glfwGetKey(window.GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		space_press = true;
		
	}
	if (glfwGetKey(window.GetWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE && space_press)
	{
		Renderer::AddCubeTexture(camera.Position() + camera.LookDirection() * 7.0f, 3.0f);
		space_press = false;
	}

	combinedRotation = glm::rotate(	glm::mat4(1.0f), 0.0f, 
									glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0f), 
									0.0f, glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
									0.0f, glm::vec3(0.0, 0.0, 1.0));
	translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	model = translation * combinedRotation;

	sData.shader->SetUniformMat4f("u_Model", model);
	sData.shader->SetUniformMat4f("u_Projection", camera.ProjectionMatrix());
	sData.shader->SetUniformMat4f("u_View", camera.LookAtMatrix());
	sData.shader->SetUniform1i("u_Tex_Index", 0);
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
