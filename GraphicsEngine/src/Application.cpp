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
    while (!glfwWindowShouldClose(m_window.GetWindow()))
    {
        Renderer::Clear();

        Renderer::DrawTriangle();

        OnUpdate();


        glfwSwapBuffers(m_window.GetWindow());
        glfwPollEvents();
    }
    Renderer::Shutdown();

}

void Application::OnAttach()
{
    // init and setting data
    std::vector<Triangle> mesh;


    mesh.push_back({ {{  0.0f, 0.0f, 0.0f}}, {{0.5f, 0.0f, 0.0f}}, {{0.5f, 0.5f, 0.0f}} });
    mesh.push_back({ {{  0.5f, 0.5f, 0.0f}}, {{0.0f, 0.5f, 0.0f}}, {{0.0f, 0.0f, 0.0f}} });

	if (!m_window.Init())
		std::exit(0);

	Renderer::Init(mesh);

}

void Application::OnUpdate()
{
    // caclulations etc
	glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);

    // get all the data 


}

void Application::OnRender()
{

}
