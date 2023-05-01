#include "Renderer.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Shader.h"


const unsigned int max_vertices = 1000;
const unsigned int max_indices = max_vertices;
const unsigned int max_texture_slots = 32;				// checken

struct Vertex
{
	glm::vec3 position;
};

struct Triangle
{
	Vertex p0, p1, p2;
	
};

struct TriangleIndices
{
	glm::highp_uvec3 indices;
};


struct RendererData
{
	unsigned int va = 0;
	unsigned int vb = 0;
	unsigned int ib = 0;

	unsigned int IndexCount = 0;

	Shader* shader = nullptr;
	Triangle* TriBuffer = nullptr;
	TriangleIndices* TriIndices = nullptr;
};

static RendererData sData;

void Renderer::Init()
{
	sData.shader = new Shader("res/shader/vert_frag.shader");
	glUseProgram(sData.shader->GetRendererID());
	sData.shader->SetUniform4f("u_Color", 1.0f, 0.2f, 0.1f, 1.0f);

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

	delete[] sData.TriBuffer;
	glDeleteBuffers(1, &sData.vb);

	delete[] sData.TriIndices;
	glDeleteBuffers(1, &sData.ib);

	glDeleteVertexArrays(1, &sData.vb);
	delete[] sData.shader;

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

void Renderer::DrawTriangle()										// mit Parametern
{
	// Dreiecke befüllen
	sData.TriBuffer = new Triangle[333];
	sData.TriIndices = new TriangleIndices[333];
	
	sData.TriBuffer[0].p0 = { {-0.5f, 0.0f, 0.0f} };
	sData.TriBuffer[0].p1 = { {0.5f, 0.0f, 0.0f} };
	sData.TriBuffer[0].p2 = { {0.0f, 0.5f, 0.0f} };

	sData.TriIndices[0] = { {0, 1, 2} };


	glBindBuffer(GL_ARRAY_BUFFER, sData.vb);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Triangle), sData.TriBuffer);
	
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.ib);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 6 * sizeof(TriangleIndices), sData.TriIndices);

	glBindVertexArray(sData.va);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);

}
