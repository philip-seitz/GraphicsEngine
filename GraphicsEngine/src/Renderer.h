#include <vector>
#include <glm/glm.hpp>

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
	static void DrawTriangles();

	static void Calculate(const float& dt);

	static void Clear();

private:
	// stats o.ä. zum Debuggen

};