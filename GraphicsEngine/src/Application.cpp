#include "Application.h"

Application::Application(const int& width, const int& height, const char* title)
	:m_window(width, height, title), m_ClearColor(0.1f, 0.1f, 0.1f, 1.0f)
{
	
}

Application::~Application()
{
	
}


void Application::OnAttach()
{


}

void Application::OnUpdate()
{
	glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);

}

void Application::OnRender()
{

}
