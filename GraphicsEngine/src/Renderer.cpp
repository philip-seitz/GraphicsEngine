#include "Renderer.h"
#include <GL/glew.h>
#include "Shader.h"


const unsigned int max_vertices = 1000;
const unsigned int max_indices = max_vertices;
const unsigned int max_texture_slots = 32;				// checken

struct RendererData
{
	unsigned int va = 0;
	unsigned int vb = 0;
	unsigned int ib = 0;

	unsigned int IndexCount = 0;
	unsigned int TriCount = 0;

	Shader* shader = nullptr;
	Triangle* TriBuffer = nullptr;
	TriangleIndices* TriIndices = nullptr;
};

static RendererData sData;

void Renderer::Init(const std::vector<Triangle>& tris)
{
	sData.shader = new Shader("res/shader/vert_frag.shader");
	glUseProgram(sData.shader->GetRendererID());
	sData.shader->SetUniform4f("u_Color", 1.0f, 0.2f, 0.1f, 1.0f);

	sData.TriBuffer = new Triangle[333];
	sData.TriIndices = new TriangleIndices[333];

	for (unsigned int i = 0; i < tris.size(); i++) 
		{
			sData.TriBuffer[i] = tris[i];
			sData.TriIndices[i] = { {0 +3*i, 1 + 3*i, 2 + 3*i} };
			sData.TriCount++;
		}

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

	glDeleteProgram(sData.shader->GetRendererID());
	// shader wird von destruktor deleted

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
	
	
	glBindBuffer(GL_ARRAY_BUFFER, sData.vb);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sData.TriCount * sizeof(Triangle), sData.TriBuffer);
	
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.ib);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sData.TriCount * sizeof(TriangleIndices), sData.TriIndices);

	glBindVertexArray(sData.va);
	glDrawElements(GL_TRIANGLES, 3 * sData.TriCount, GL_UNSIGNED_INT, nullptr);

}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);

}
