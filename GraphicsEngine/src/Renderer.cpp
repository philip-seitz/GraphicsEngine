#include "Renderer.h"
#include <GL/glew.h>
#include "Shader.h"
#include <iostream>
#include "Util.h"

// constants for vertices and textures
const unsigned int max_vertices = 20000;
const unsigned int max_indices = max_vertices;
const unsigned int max_texture_slots = 32;				// checken

// constants for grid
const unsigned int grid_size = 10.0f;
const float grid_line_distance = 1.0f;
const unsigned int max_grid_lines = CalculateNumberOfLines(float(grid_size), float(grid_line_distance)); 

// constants for the camera maybe	(add to camera class or smth)
const float yFOV = 30.0f;
const float aspectRatio = 1080.0f / 720.0f;
const float zNear = 1.5f;
const float zFar = -2.5f;

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

// should be in camera class later on
glm::vec3 camera_position = { 0.0f, 0.0f, 0.0f };

struct RendererData
{
	unsigned int va = 0;
	unsigned int vb = 0;
	unsigned int ib = 0;

	unsigned int g_va = 0;
	unsigned int g_vb = 0;
	unsigned int g_ib = 0;

	unsigned int TriCount = 0;
	unsigned int TriToRenderCount = 0;

	unsigned int LineCount = 0;
	unsigned int LineToRenderCount = 0;

	Shader* shader = nullptr;

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
	
	glm::mat4x4 projectionMatrix = glm::perspective(yFOV * 3.14f / 180.0f, aspectRatio, zNear, zFar);
};


static RendererData sData;

void Renderer::Init()
{
	sData.shader = new Shader("res/shader/vert_frag.shader");
	glUseProgram(sData.shader->GetRendererID());
	sData.shader->SetUniform4f("u_Color", 1.0f, 0.2f, 0.1f, 1.0f);
	std::cout << max_grid_lines << std::endl;
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
	delete[] sData.LineBuffer;
	delete[] sData.GridLineIndices;
	delete[] sData.LineToRenderBuffer;

	glDeleteVertexArrays(1, &sData.vb);

	glDeleteProgram(sData.shader->GetRendererID());
	delete sData.shader;
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
		if (i < limit * 1/4)
		{
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
	mesh.push_back({ {{0.25f, -0.25f, -0.25f}}, {{-0.25f, -0.25f, -0.25f}}, {{-0.25f, 0.25f, -0.25f}} });
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

void Renderer::LoadMesh()
{
	std::vector <float>vert;
	std::vector<unsigned int> ind;
	glm::vec4 darkCol = { 0.0f, 0.0f, 0.0f, 0.0f };
	std::vector<glm::vec3> mesh;
	std::vector< glm::highp_uvec3> indices;
	ReadVertexData("res/objects/utah_teapot.obj", vert, ind);
	// put the data into the triangle structures...
	int vertexNumber = vert.size() / 3;
	int triNumber = ind.size() / 3;


	for (int i = 0; i < vertexNumber; i++)
	{
		glm::vec3 pos;
		pos = { vert[0 + i * 3], vert[1 + i * 3], vert[2 + i * 3], };
		mesh.push_back(pos);
	}

	for (int i = 0; i < triNumber; i++)
	{
		glm::highp_uvec3 idx;
		idx = { ind[0 + i * 3], ind[1 + i * 3], ind[2 + i * 3], };
		indices.push_back(idx);
	}

	for (int i = 0; i < triNumber; i++)
	{
		*(sData.TriBufferPointer) = Triangle{ {mesh[indices[i][0] -1], darkCol}, {mesh[indices[i][1] - 1], darkCol}, {mesh[indices[i][2] - 1], darkCol} };
		*(sData.TriIndicesPointer) = { {indices[i][0], indices[i][1], indices[i][2]}};
		sData.TriBufferPointer++;
		sData.TriIndicesPointer++;
		sData.TriCount++;
	}

	std::cout << "hi";
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

	glDrawElements(GL_LINES, 2*sData.LineToRenderCount, GL_UNSIGNED_INT, nullptr);
}

void Renderer::Calculate(const float& dt)
{
	// Reset bc Calculate is contineously called while the app is running
	sData.TriToRenderCount = 0;
	sData.TriToRenderBufferPointer = sData.TriToRenderBuffer;
	sData.TriToRenderIndicesPointer = sData.TriToRenderIndices;

	sData.LineToRenderCount = 0;
	sData.LineToRenderBufferPointer = sData.LineToRenderBuffer;
	sData.GridLineToRenderIndicesPointer = sData.GridLineToRenderIndices;

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

	translationZ = translationZ + 0.8 * dt;

	glm::mat4x4 combinedRotation(0);
	glm::mat4x4 projPipeline(0);
	combinedRotation = glm::rotate(glm::mat4(1.0f), angleX,
		glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0f), 0.4f, glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
			0.0f, glm::vec3(0.0, 0.0, 1.0));
	glm::mat4x4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -30.0f));

	// apply the projection matrix to all vertices
	for (int i = 0; i < sData.TriCount; i++)
	{
		glm::vec3 p0 = sData.TriBuffer[i].p0.position;
		glm::vec3 p1 = sData.TriBuffer[i].p1.position;
		glm::vec3 p2 = sData.TriBuffer[i].p2.position;

		glm::vec4 c0 = sData.TriBuffer[i].p0.color;
		glm::vec4 c1 = sData.TriBuffer[i].p1.color;
		glm::vec4 c2 = sData.TriBuffer[i].p2.color;

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
		float tmp = glm::dot(normal, (p0_t - camera_position)); 

		// Triangles are drawn if its normal is pointing towards the 'camera' (back face culling) 
		// texas teapot does not have extra inside surface/inside normals -> not showing with backface culling
		if (tmp < 0.0f)
		{
			// Perspective Projection (World -> View Space)
			p0_h = sData.projectionMatrix * p0_h;
			p1_h = sData.projectionMatrix * p1_h;
			p2_h = sData.projectionMatrix * p2_h;

			glm::vec3 p0_proj = { p0_h.x / p0_h.w, p0_h.y / p0_h.w, p0_h.z / p0_h.w };
			glm::vec3 p1_proj = { p1_h.x / p1_h.w, p1_h.y / p1_h.w, p1_h.z / p1_h.w };
			glm::vec3 p2_proj = { p2_h.x / p2_h.w, p2_h.y / p2_h.w, p2_h.z / p2_h.w };

			*(sData.TriToRenderBufferPointer) = Triangle{ {p0_proj, c0 }, {p1_proj, c1}, {p2_proj, c2} };
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

		grid_a = sData.projectionMatrix * grid_a;
		grid_b = sData.projectionMatrix * grid_b;

		glm::vec3 ga_proj = glm::vec3({ grid_a.x / grid_a.w, grid_a.y / grid_a.w, grid_a.z / grid_a.w });
		glm::vec3 gb_proj = glm::vec3({ grid_b.x / grid_b.w, grid_b.y / grid_b.w, grid_b.z / grid_b.w });

		sData.LineToRenderBufferPointer->p0.position = ga_proj;
		sData.LineToRenderBufferPointer->p1.position = gb_proj;
		sData.LineToRenderBufferPointer->p0.color = { WHITE };
		sData.LineToRenderBufferPointer->p1.color = { RED };
		sData.GridLineToRenderIndicesPointer->indices = glm::highp_uvec2(2*i, 2*i + 1 );

		sData.LineToRenderBufferPointer++;
		sData.GridLineToRenderIndicesPointer++;
		sData.LineToRenderCount++;
	}

}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
