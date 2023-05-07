#include "Renderer.h"
#include <GL/glew.h>
#include "Shader.h"


const unsigned int max_vertices = 1000;
const unsigned int max_indices = max_vertices;
const unsigned int max_texture_slots = 32;				// checken

// constants for the camera maybe	(add to camera class or smth)
const float yFOV = 40.0f;
const float aspectRatio = 1080.0f / 720.0f;
const float zNear = 1.5f;
const float zFar = -2.5f;

float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
float angleScalerX = 0.2f;
float angleScalerY = 0.4f;
float angleScalerZ = 0.6f;

float translationZ = 0.0f;

struct RendererData
{
	unsigned int va = 0;
	unsigned int vb = 0;
	unsigned int ib = 0;

	unsigned int TriCount = 0;
	unsigned int TriToRenderCount = 0;

	Shader* shader = nullptr;

	Triangle* TriBuffer = nullptr;
	Triangle* TriToRenderBuffer = nullptr;
	Triangle* TriBufferPointer = nullptr;
	Triangle* TriToRenderBufferPointer = nullptr;

	TriangleIndices* TriIndices = nullptr;
	TriangleIndices* TriToRenderIndices = nullptr;
	TriangleIndices* TriIndicesPointer = nullptr;
	TriangleIndices* TriToRenderIndicesPointer = nullptr;

	glm::mat4x4 projectionMatrix = glm::perspective(yFOV*3.14f/180.0f, aspectRatio, zNear, zFar);

};

static RendererData sData;

void Renderer::Init()
{
	sData.shader = new Shader("res/shader/vert_frag.shader");
	glUseProgram(sData.shader->GetRendererID());
	sData.shader->SetUniform4f("u_Color", 1.0f, 0.2f, 0.1f, 1.0f);

	// Structures for all triangles
	sData.TriBuffer = new Triangle[333];
	sData.TriIndices = new TriangleIndices[333];
	sData.TriBufferPointer = sData.TriBuffer;
	sData.TriIndicesPointer = sData.TriIndices;


	// Structures for the triangles that are to be drawn
	sData.TriToRenderBuffer = new Triangle[333];
	sData.TriToRenderIndices = new TriangleIndices[333];
	sData.TriToRenderBufferPointer = sData.TriToRenderBuffer;
	sData.TriToRenderIndicesPointer = sData.TriToRenderIndices;

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

}

void Renderer::Shutdown()
{
	glDeleteBuffers(1, &sData.vb);
	glDeleteBuffers(1, &sData.ib);
	delete[] sData.TriBuffer;
	delete[] sData.TriIndices;

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

void Renderer::DrawTriangles()										
{
	
	glBindBuffer(GL_ARRAY_BUFFER, sData.vb);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sData.TriToRenderCount * sizeof(Triangle), sData.TriToRenderBuffer);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.ib);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sData.TriToRenderCount * sizeof(TriangleIndices), sData.TriToRenderIndices);

	glBindVertexArray(sData.va);
	// Enable wireframe drawing
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// Enable point smoothing and set the point size
	// glEnable(GL_POINT_SMOOTH);
	// glPointSize(10.0f);

	// Draw the triangles' outlines
	glDrawElements(GL_TRIANGLES, 3 * sData.TriToRenderCount, GL_UNSIGNED_INT, nullptr);

	// Draw the points
	// glDrawArrays(GL_POINTS, 0, 3 * sData.TriToRenderCount);

}

void Renderer::Calculate(const float& dt)
{
	// Reset bc Calculate is called while the app is running
	sData.TriToRenderCount = 0;
	sData.TriToRenderBufferPointer = sData.TriToRenderBuffer;
	sData.TriToRenderIndicesPointer = sData.TriToRenderIndices;

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
		glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
			0.0f, glm::vec3(0.0, 0.0, 1.0));
	glm::mat4x4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));

	// apply the projection matrix to all vertices
	for (int i = 0; i < sData.TriCount; i++)
	{

		glm::vec3 p0 = sData.TriBuffer[i].p0.position;
		glm::vec3 p1 = sData.TriBuffer[i].p1.position;
		glm::vec3 p2 = sData.TriBuffer[i].p2.position;

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


		float tmp = glm::dot(normal, (p0_t - glm::vec3(0.0f)));

		if (tmp < 0.0f)
		{
			// Perspective Projection (World -> View Space)
			p0_h = sData.projectionMatrix * p0_h;
			p1_h = sData.projectionMatrix * p1_h;
			p2_h = sData.projectionMatrix * p2_h;

			glm::vec3 p0_proj = { p0_h.x / p0_h.w, p0_h.y / p0_h.w, p0_h.z / p0_h.w };
			glm::vec3 p1_proj = { p1_h.x / p1_h.w, p1_h.y / p1_h.w, p1_h.z / p1_h.w };
			glm::vec3 p2_proj = { p2_h.x / p2_h.w, p2_h.y / p2_h.w, p2_h.z / p2_h.w };

			*(sData.TriToRenderBufferPointer) = Triangle{ p0_proj, p1_proj, p2_proj };
			*(sData.TriToRenderIndicesPointer) = TriangleIndices{ {0 + 3 * (sData.TriToRenderCount), 1 + 3 * (sData.TriToRenderCount), 2 + 3 * (sData.TriToRenderCount)} };
			sData.TriToRenderIndicesPointer++;
			sData.TriToRenderBufferPointer++;
			sData.TriToRenderCount++;
			// if the triangle normal and the view direction are in opposite directions (crossproduct)
			// in order for that to work all surfaces (triangles) have to be setup in a counter clockwise manner
		}
	}

}


void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
