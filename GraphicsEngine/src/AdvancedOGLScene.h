#pragma once
#include "Application.h"
#include "SceneGraph.h"

class AdvancedOGLScene : public Application
{
public:
	AdvancedOGLScene();
	AdvancedOGLScene(const int& width, const int& height, const char* title);
	~AdvancedOGLScene();

	void Run();
	void OnUpdate();
	void OnRender();
	void OnEvent(float& dt);

private:
	glm::vec4 m_ClearColor;
	SceneGraph m_SceneGraph;

};
