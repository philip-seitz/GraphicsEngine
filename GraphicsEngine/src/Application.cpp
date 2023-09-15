#include "Application.h"
#include "Renderer.h"

Application::Application(const int& width, const int& height, const char* title)
	:m_window(width, height, title), m_ClearColor(0.1f, 0.1f, 0.1f, 1.0f)
{

}

Application::~Application()
{
	
}

void Application::Run()
{
    OnAttach();

    float lastTime = glfwGetTime();
   
    // render loop
    while (!glfwWindowShouldClose(m_window.GetWindow()))
    {
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;
        Renderer::Clear();

        OnUpdate(dt);

        Renderer::DrawGrid();
        Renderer::DrawModels();
        
        glfwSwapBuffers(m_window.GetWindow());
        glfwPollEvents();

        // check for window termination 
        if (glfwGetKey(m_window.GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(m_window.GetWindow(), true);

        }
        if (glfwGetKey(m_window.GetWindow(), GLFW_KEY_SPACE == GLFW_PRESS))
        {
        }

    }
    Renderer::Shutdown();
}

void Application::OnAttach()
{
	if (!m_window.Init())
		std::exit(0);

	Renderer::Init();
}

void Application::OnUpdate(const float& dt)
{
    // caclulations etc
	glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    Renderer::Calculate(dt, m_window);
}

void Application::OnRender()
{
}

// callback function for resizing window
void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
