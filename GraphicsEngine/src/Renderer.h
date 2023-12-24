#pragma once
#include <vector>
#include "Window.h"
#include "Shader.h"
#include "SceneGraph.h"
#include "PointLight.h"
#include "Util.h"
#include "ModelInstance.h"
#include <glm/glm.hpp>

class Renderer
{
public:
	// static class (keine Instanz)

	static void Init(Window* window);
	static void Shutdown();
	static void BeginBatch();
	static void EndBatch();
	static void Flush();
	static int	ModelGridLayout(ModelInstance** inst, const int& number, const glm::vec3& pos);
	static void RenderSceneGraph(SceneGraph* scene_graph);
	static void RenderToDepthMap(SceneGraph* scene_graph);
	static void RenderSceneWithDepthMap(SceneGraph* scene_graph);
	static void Clear();

private:
	// stats o.ä. zum Debuggen
};