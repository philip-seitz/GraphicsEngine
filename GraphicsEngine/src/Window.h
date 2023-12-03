#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


class Window
{
public:
	int m_Height;
	int m_Width;
	Window(const int& width, const int& height, const char* title):
		m_AllRunning(true), 
		m_InstWindow(nullptr),
		m_Height(height),
		m_Width(width)
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
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			m_InstWindow = glfwCreateWindow(w, h, title, NULL, NULL);
			// register window resize callback
			glfwSetFramebufferSizeCallback(m_InstWindow, framebuffer_resize_callback);

			// settings for mouse input
			glfwSetInputMode(m_InstWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// hide cursor and keep in window when focused
			glfwSetCursorPosCallback(m_InstWindow, mouse_callback);
			glfwSetScrollCallback(m_InstWindow, scroll_callback);

			glViewport(0, 0, w, h);
			m_Width = w;
			m_Height = h;
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
				GLint maxUniformBufferBindings;
				glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferBindings);
				std::cout << "Max Uniform Buffer Bindings: " << maxUniformBufferBindings << std::endl;

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