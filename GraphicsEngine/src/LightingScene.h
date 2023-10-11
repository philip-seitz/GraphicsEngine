#pragma once
#include "Application.h"
#include "SceneGraph.h"

class LightingScene : public Application
{
public:
	LightingScene();
	LightingScene(const int& width, const int& height, const char* title);
	~LightingScene();

	void Run();	
	void OnUpdate();	
	void OnRender();
	void OnEvent(float& dt);
	Window m_window;

private:
	glm::vec4 m_ClearColor;
	SceneGraph m_SceneGraph;

};
