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

	static void Init(const std::vector<Triangle>& tris);
	static void Shutdown();


	static void BeginBatch();
	static void EndBatch();
	static void Flush();


	static void DrawTriangle();
	static void Clear();

private:
	// stats o.ä. zum Debuggen

};