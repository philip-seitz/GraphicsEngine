#include "Application.h"
#include "Renderer.h"


Application::Application():
    m_window(1000, 500, "App"), m_ClearColor(0.1f, 0.1f, 0.1f, 1.0f)

{

}
Application::Application(const int& width, const int& height, const char* title)
	:m_window(width, height, title), m_ClearColor(0.1f, 0.1f, 0.1f, 1.0f)
{

}

// Run() and OnEvent() have to be implemented in each seperate "scene"
Application::~Application()
{
	
}

void Application::OnRender()
{
    Renderer::DrawGrid();
    Renderer::DrawModels();
    glfwSwapBuffers(m_window.GetWindow());
}

void Application::OnEvent()
{
    glfwPollEvents();

    // check for window termination 
    if (glfwGetKey(m_window.GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_window.GetWindow(), true);
    }
}

// callback function for resizing window
void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
