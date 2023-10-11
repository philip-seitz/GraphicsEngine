#pragma once
#include "Shader.h"
#include "Window.h"

class Application
{
public:
	Application();
	Application(const int& width, const int& height, const char* title);
	~Application();

	virtual void Run() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnEvent(float& dt) = 0;


private:

	glm::vec4 m_ClearColor;
};