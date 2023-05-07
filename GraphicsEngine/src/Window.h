#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(const int& width, const int& height, const char* title)
		:m_AllRunning(true), m_InstWindow(nullptr)
	{
		if (!glfwInit())
			m_AllRunning = false;
		else
			m_InstWindow = glfwCreateWindow(width, height, title, NULL, NULL);
			glViewport(0, 0, width, height);
	}
	~Window()
	{
		if (m_InstWindow)
			glfwTerminate();
	}

	bool Init()
	{
		if (!m_AllRunning)
			return false;
		else
		{
			if (!m_InstWindow)
			{
				glfwTerminate();
				m_AllRunning = false;
			}
			else
			{
				glfwMakeContextCurrent(m_InstWindow);
				if (glewInit() != GLEW_OK)
					std::cout << "error" << std::endl;

				std::cout << glGetString(GL_VERSION) << std::endl;
				m_AllRunning = true;
			}
		}
		return m_AllRunning;
	}

	GLFWwindow* GetWindow()
	{
		return m_InstWindow;
	}
	
private:
	bool m_AllRunning;
	GLFWwindow* m_InstWindow;
	

};