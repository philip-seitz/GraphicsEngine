#pragma once
#include "Application.h"

class LightingScene : public Application
{
public:
	LightingScene();
	LightingScene(const int& width, const int& height, const char* title);
	~LightingScene();

	void Run();	
	void OnUpdate();	

private:
	glm::vec4 m_ClearColor;

};
