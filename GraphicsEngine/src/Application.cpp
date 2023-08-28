#include "Application.h"
#include "Renderer.h"

Application::Application(const int& width, const int& height, const char* title)
	:m_window(width, height, title), m_ClearColor(0.1f, 0.1f, 0.1f, 0.5f)
{
	
}

Application::~Application()
{
	
}

void Application::Run()
{
    OnAttach();
    // Renderer::AddQuad(0.4f, 0.0f, 0.2f);
    // Renderer::AddQuad(-0.2f, 0.0f, 0.2f);
    // Renderer::AddQuad(0.05f, -0.25f, 0.3f);
    // Renderer::AddTriangle(0.05f, 0.25f, 0.3f);

    float lastTime = glfwGetTime();
    // Renderer::AddCubeTexture();
    // Renderer::AddCube(0.05f, -0.25f, 0.3f, 0.0f);             // parameters dont matter yet!
    Renderer::LoadMesh();
    Renderer::AddGrid();

    while (!glfwWindowShouldClose(m_window.GetWindow()))
    {
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;
        Renderer::Clear();

        OnUpdate(dt);

        Renderer::DrawGrid();
        Renderer::DrawTriangles();
        
        glfwSwapBuffers(m_window.GetWindow());
        glfwPollEvents();
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
