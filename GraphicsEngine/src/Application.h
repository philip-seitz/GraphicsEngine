#include "Shader.h"
#include "Window.h"

class Application
{
public:
	Application(const int& width, const int& height, const char* title);
	~Application();

	void Run();
	void OnAttach();
	void OnUpdate(const float& dt);
	void OnRender();

public:
	Window m_window;
private:

	glm::vec4 m_ClearColor;
};