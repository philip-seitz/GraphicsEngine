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
	unsigned int va = 0;
	unsigned int vb = 0;
	unsigned int ib = 0;

	unsigned int g_va = 0;
	unsigned int g_vb = 0;
	unsigned int g_ib = 0;

	Shader* shader = nullptr;
	Texture* texture = nullptr;


	unsigned int TriCount = 0;
	unsigned int TriToRenderCount = 0;

	// DEBUG
	unsigned int TriClipCount = 0;

	unsigned int LineCount = 0;
	unsigned int LineToRenderCount = 0;

	Triangle* TriBuffer = nullptr;
	Triangle* TriToRenderBuffer = nullptr;
	Triangle* TriBufferPointer = nullptr;
	Triangle* TriToRenderBufferPointer = nullptr;

	TriangleIndices* TriIndices = nullptr;
	TriangleIndices* TriToRenderIndices = nullptr;
	TriangleIndices* TriIndicesPointer = nullptr;
	TriangleIndices* TriToRenderIndicesPointer = nullptr;
	Line* LineBuffer = nullptr;
	Line* LineToRenderBuffer = nullptr;
	Line* LineBufferPointer = nullptr;
	Line* LineToRenderBufferPointer = nullptr;

	LineIndices* GridLineIndices = nullptr;
	LineIndices* GridLineToRenderIndices = nullptr;
	LineIndices* GridLineIndicesPointer = nullptr;
	LineIndices* GridLineToRenderIndicesPointer = nullptr;
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

	// Structures for the triangles that are to be drawn
	sData.TriToRenderBuffer = new Triangle[max_vertices / 3];
	sData.TriToRenderIndices = new TriangleIndices[max_vertices/3];
	sData.TriToRenderBufferPointer = sData.TriToRenderBuffer;
	sData.TriToRenderIndicesPointer = sData.TriToRenderIndices;

	// structures for grid lines
	sData.LineBuffer = new Line[max_grid_lines];
	sData.GridLineIndices = new LineIndices[max_grid_lines];
	sData.LineBufferPointer = sData.LineBuffer;
	sData.GridLineIndicesPointer = sData.GridLineIndices;

	// stuctures for grid lines that are to be drawn
	sData.LineToRenderBuffer = new Line[max_grid_lines];
	sData.GridLineToRenderIndices = new LineIndices[max_grid_lines];
	sData.LineToRenderBufferPointer = sData.LineToRenderBuffer;
	sData.GridLineToRenderIndicesPointer = sData.GridLineToRenderIndices;

	// setup for triangles
	glCreateVertexArrays(1, &sData.va);
	glBindVertexArray(sData.va);

	glCreateBuffers(1, &sData.vb);
	glBindBuffer(GL_ARRAY_BUFFER, sData.vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * max_vertices, nullptr, GL_DYNAMIC_DRAW);

	glCreateBuffers(1, &sData.ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * max_indices, nullptr, GL_DYNAMIC_DRAW);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * max_grid_lines, nullptr, GL_DYNAMIC_DRAW);

	glCreateBuffers(1, &sData.g_ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.g_ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * max_grid_lines*2, nullptr, GL_DYNAMIC_DRAW);

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
	delete[] sData.TriToRenderBuffer;
	delete[] sData.TriToRenderIndices;


	delete[] sData.LineBuffer; 
	delete[] sData.GridLineIndices;
	delete[] sData.LineToRenderBuffer;
	delete[] sData.GridLineToRenderIndices;

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

void Renderer::AddCubeTexture()
{
	std::vector<Triangle> mesh;

	//// NORTH (good)
	mesh.push_back({ {{0.25f, -0.25f, -0.25f}, {WHITE}, {1.0f, 0.0f}}, { {-0.25f, -0.25f, -0.25f}, {WHITE}, {0.0f, 0.0f}}, {{-0.25f, 0.25f, -0.25f}, {WHITE}, {0.0f, 1.0f}} });
	mesh.push_back({ {{0.25f, -0.25f, -0.25f}, {WHITE}, {1.0f, 0.0f}}, {{  -0.25f, 0.25f, -0.25f}, {WHITE}, {0.0f, 1.0f}}, {{0.25f, 0.25f, -0.25f}, {WHITE}, {1.0f, 1.0f}} });

	// SOUTH (good)
	mesh.push_back({ {{-0.25f, -0.25f, 0.25f}, {WHITE}, {0.0f, 0.0f}}, {{  0.25f, -0.25f, 0.25f}, {WHITE}, {1.0f, 0.0f}}, {{0.25f, 0.25f, 0.25f}, {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({ {{-0.25f, -0.25f, 0.25f}, {WHITE}, {0.0f, 0.0f}}, {{  0.25f, 0.25f, 0.25f}, {WHITE}, {1.0f, 1.0f}}, {{-0.25f, 0.25f, 0.25f}, {WHITE}, {0.0f, 1.0f}} });

	// EAST (good)
	mesh.push_back({ {{0.25f, -0.25f, 0.25f}, {WHITE}, {0.0f, 0.0f}}, {{  0.25f, -0.25f, -0.25f}, {WHITE}, {1.0f, 0.0f}}, {{0.25f, 0.25f, -0.25f}, {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({ {{0.25f, -0.25f, 0.25f}, {WHITE}, {0.0f, 0.0f}}, {{  0.25f, 0.25f, -0.25f}, {WHITE}, {1.0f, 1.0f}}, {{0.25f, 0.25f, 0.25f}, {WHITE}, {0.0f, 1.0f}} });

	// WEST (good)
	mesh.push_back({ {{-0.25f, -0.25f, -0.25f}, {WHITE}, {0.0f, 0.0f}}, {{  -0.25f, -0.25f, 0.25f}, {WHITE}, {1.0f, 0.0f}}, {{-0.25f, 0.25f, 0.25f}, {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({ {{-0.25f, -0.25f, -0.25f}, {WHITE}, {0.0f, 0.0f}}, {{  -0.25f, 0.25f, 0.25f}, {WHITE}, {1.0f, 1.0f}}, {{-0.25f, 0.25f, -0.25f}, {WHITE}, {0.0f, 1.0f}} });

	// BOTTOM 
	mesh.push_back({ {{-0.25f, -0.25f, -0.25f}, {WHITE}, {0.0f, 0.0f}}, {{  0.25f, -0.25f, -0.25f}, {WHITE}, {1.0f, 0.0f}}, {{0.25f, -0.25f, 0.25f}, {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({ {{-0.25f, -0.25f, -0.25f}, {WHITE}, {0.0f, 0.0f}}, {{  0.25f, -0.25f, 0.25f}, {WHITE}, {1.0f, 1.0f}}, {{-0.25f, -0.25f, 0.25f}, {WHITE}, {0.0f, 1.0f}} });

	// TOP 
	mesh.push_back({ {{-0.25f, 0.25f, 0.25f}, {WHITE}, {0.0f, 0.0f}}, {{  0.25f, 0.25f, 0.25f}, {WHITE}, {1.0f, 0.0f}}, {{0.25f, 0.25f, -0.25f}, {WHITE}, {1.0f, 1.0f}} });
	mesh.push_back({ {{-0.25f, 0.25f, 0.25f}, {WHITE}, {0.0f, 0.0f}}, {{  0.25f, 0.25f, -0.25f}, {WHITE}, {1.0f, 1.0f}}, {{-0.25f, 0.25f, -0.25f}, {WHITE}, {0.0f, 1.0f}} });


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
	glm::vec4 darkCol = { 0.0f, 0.0f, 0.0f, 0.0f };
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
		*(sData.TriBufferPointer) = Triangle{ {mesh[indices[i][0] -1], darkCol}, {mesh[indices[i][1] - 1], darkCol}, {mesh[indices[i][2] - 1], darkCol} };
		*(sData.TriIndicesPointer) = { {indices[i][0], indices[i][1], indices[i][2]}};
		sData.TriBufferPointer++;
		sData.TriIndicesPointer++;
		// std::cout << i << std::endl;
		sData.TriCount++;
	}
}

void Renderer::DrawTriangles()								
{
	glBindVertexArray(sData.va);
	glBindBuffer(GL_ARRAY_BUFFER, sData.vb);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sData.TriToRenderCount * sizeof(Triangle), sData.TriToRenderBuffer);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.ib);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sData.TriToRenderCount * sizeof(TriangleIndices), sData.TriToRenderIndices);

	// Enable wireframe drawing
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Blending 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable point smoothing and set the point size
	// glEnable(GL_POINT_SMOOTH);
	// glPointSize(10.0f);

	// enable textures [0 -> no; 1 -> yes]
	sData.shader->SetUniform1i("u_Use_Texture", 0);

	// Draw the triangles' outlines
	glDrawElements(GL_TRIANGLES, 3 * sData.TriToRenderCount, GL_UNSIGNED_INT, nullptr);

	// Draw the points
	// glDrawArrays(GL_POINTS, 0, 3 * sData.TriToRenderCount);
}

void Renderer::DrawGrid()
{
	glBindVertexArray(sData.g_va);
	glBindBuffer(GL_ARRAY_BUFFER, sData.g_vb);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sData.LineToRenderCount * sizeof(Line), sData.LineToRenderBuffer);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.g_ib);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sData.LineToRenderCount * sizeof(LineIndices), sData.GridLineToRenderIndices);

	// improve appearence of the grid (anti z-fighting and thinner lines)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glLineWidth(1.0f);

	// enable textures [0 -> no; 1 -> yes]
	sData.shader->SetUniform1i("u_Use_Texture", 0);

	glDrawElements(GL_LINES, 2*sData.LineToRenderCount, GL_UNSIGNED_INT, nullptr);
}

void Renderer::Calculate(const float& dt, Window& window)
{
	// Reset bc Calculate is contineously called while the app is running
	sData.TriToRenderCount = 0;
	sData.TriToRenderBufferPointer = sData.TriToRenderBuffer;
	sData.TriToRenderIndicesPointer = sData.TriToRenderIndices;

	sData.LineToRenderCount = 0;
	sData.LineToRenderBufferPointer = sData.LineToRenderBuffer;
	sData.GridLineToRenderIndicesPointer = sData.GridLineToRenderIndices;

	glm::mat4x4 model(0);
	glm::mat4x4 view(0);
	glm::mat4x4 perspective(0);

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

	glm::mat4x4 combinedRotation(0);
	combinedRotation = glm::rotate(glm::mat4(1.0f), 0.0f, // 180.0f*3.14f*180.0f, (d2 map rotation)
		glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
			0.0f, glm::vec3(0.0, 0.0, 1.0));
	glm::mat4x4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	// apply the projection matrix to all vertices
	for (int i = 0; i < sData.TriCount; i++)
	{
		glm::vec3 p0 = sData.TriBuffer[i].p0.position;
		glm::vec3 p1 = sData.TriBuffer[i].p1.position;
		glm::vec3 p2 = sData.TriBuffer[i].p2.position;

		glm::vec4 c0 = sData.TriBuffer[i].p0.color;
		glm::vec4 c1 = sData.TriBuffer[i].p1.color;
		glm::vec4 c2 = sData.TriBuffer[i].p2.color;

		glm::vec2 t0 = sData.TriBuffer[i].p0.texture_cords;
		glm::vec2 t1 = sData.TriBuffer[i].p1.texture_cords;
		glm::vec2 t2 = sData.TriBuffer[i].p2.texture_cords;

		// Rotation
		glm::vec4 p0_h = combinedRotation * glm::vec4(p0, 1.0f);
		glm::vec4 p1_h = combinedRotation * glm::vec4(p1, 1.0f);
		glm::vec4 p2_h = combinedRotation * glm::vec4(p2, 1.0f);

		// Translation
		p0_h = translation * p0_h;
		p1_h = translation * p1_h;
		p2_h = translation * p2_h;

		glm::vec3 p0_t(p0_h);
		glm::vec3 p1_t(p1_h);
		glm::vec3 p2_t(p2_h);

		glm::vec3 normal = glm::cross((p1_t - p0_t), (p2_t - p0_t));
		normal = glm::normalize(normal);
		float tmp = glm::dot(normal, glm::normalize(p0_t - camera.Position())); 

		// Triangles are drawn if its normal is pointing towards the 'camera' (back face culling) 
		// texas teapot does not have extra inside surface/inside normals -> not showing with backface culling
		if (tmp < 0.0f)
		{
			*(sData.TriToRenderBufferPointer) = Triangle{ {p0_t, c0, t0 }, {p1_t, c1, t1}, {p2_t, c2, t2} };
			*(sData.TriToRenderIndicesPointer) = TriangleIndices{ {0 + 3 * (sData.TriToRenderCount), 1 + 3 * (sData.TriToRenderCount), 2 + 3 * (sData.TriToRenderCount)} };

			// Shading relative to light direction 
			sData.TriToRenderBufferPointer->SetColor(GetColorShade(glm::dot(lightDirection, normal), glm::vec4{WHITE}));

			sData.TriToRenderIndicesPointer++;
			sData.TriToRenderBufferPointer++;
			sData.TriToRenderCount++;
		}
	}

	// separate transformations for grid lines
	for (int i = 0; i < sData.LineCount; i++)
	{
		glm::vec4 grid_a = glm::vec4(sData.LineBuffer[i].p0.position, 1.0f);
		glm::vec4 grid_b = glm::vec4(sData.LineBuffer[i].p1.position, 1.0f);

		grid_a = combinedRotation * grid_a;
		grid_b = combinedRotation * grid_b;

		grid_a = translation * grid_a;
		grid_b = translation * grid_b;

		sData.LineToRenderBufferPointer->p0.position = grid_a;
		sData.LineToRenderBufferPointer->p1.position = grid_b;

		sData.LineToRenderBufferPointer->p0.color = sData.LineBuffer[i].p0.color;
		sData.LineToRenderBufferPointer->p1.color = sData.LineBuffer[i].p1.color;
		// sData.LineToRenderBufferPointer->p0.color = { 0.3f, 0.3f, 0.3f, 1.0f };
		// sData.LineToRenderBufferPointer->p1.color = { 0.3f, 0.3f, 0.3f, 1.0f };
		sData.GridLineToRenderIndicesPointer->indices = glm::highp_uvec2(2*i, 2*i + 1 );

		sData.LineToRenderBufferPointer++;
		sData.GridLineToRenderIndicesPointer++;
		sData.LineToRenderCount++;
	}

	sData.shader->SetUniformMat4f("u_Projection", camera.ProjectionMatrix());
	sData.shader->SetUniformMat4f("u_View", camera.LookAtMatrix());
	sData.shader->SetUniform1i("u_Tex_Index", 0);
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
