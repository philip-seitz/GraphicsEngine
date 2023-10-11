#include "Application.h"
#include "Renderer.h"


Application::Application():
    m_ClearColor(0.1f, 0.1f, 0.1f, 1.0f)

{

}
Application::Application(const int& width, const int& height, const char* title):
	m_ClearColor(0.1f, 0.1f, 0.1f, 1.0f)
{

}

// Run() and OnEvent() have to be implemented in each seperate "scene"
Application::~Application()
{
	
}

// callback function for resizing window
void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
