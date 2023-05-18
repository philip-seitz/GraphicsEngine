#include <vector>
#include <glm/glm.hpp>

#define RED 1.0f, 0.0f, 0.0f, 1.0f
#define GREEN 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE 0.0f, 0.0f, 1.0f, 1.0f
#define PURPLE 0.6f, 0.0f, 0.8f, 1.0f
#define ORANGE 0.9f, 0.3f, 0.3f, 1.0f
#define B_BLUE 0.0f, 0.5f, 0.9f, 1.0f
#define B_BLUE_SHADE 0.0f, 0.5f, 0.9f, 0.8f
#define B_BLUE_DARK 0.0f, 0.5f, 0.9f, 0.4f

struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
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

};

struct TriangleIndices
{
	glm::highp_uvec3 indices;
};


class Renderer
{
public:
	// static class (keine Instanz)

	static void Init();
	static void Shutdown();


	static void BeginBatch();
	static void EndBatch();
	static void Flush();

	static void AddTriangle(const float& x, const float& y, const float& size);						// point bottom left (in 2D)
	static void AddQuad(const float& x, const float& y, const float& size);							// point bootom left (in 2D)
	static void AddCube(const float& x, const float& y, const float& z, const float& size);			// point front, bottom left (in 3D)
	static void LoadMesh();																			// load gemoetry from obj. file
	static void DrawTriangles();

	static void Calculate(const float& dt);

	static void Clear();

private:
	// stats o.ä. zum Debuggen

};