#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

class Window
{
public:
	Window(const int& width, const int& height, const char* title)
		:m_AllRunning(true), m_InstWindow(nullptr)
	{
		if (!glfwInit())
			m_AllRunning = false;
		else 
		{
			int w = width;
			int h = height;
			ScaleWindowSize(w, h);					// resolution based scaling
			glfwWindowHint(GLFW_SAMPLES, 4);		// for multi sampling AA
			// set OpenGL Version 3.3
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			m_InstWindow = glfwCreateWindow(w, h, title, NULL, NULL);
			// register window resize callback
			glfwSetFramebufferSizeCallback(m_InstWindow, framebuffer_resize_callback);
			glViewport(0, 0, w, h);
		}
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
	void ScaleWindowSize(int& w, int& h)
	{
		int scale = 1;
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		if (mode->width > 1920)
			scale = 2;

		w *= scale;
		h *= scale;

	}

};