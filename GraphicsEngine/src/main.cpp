#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

#include "Window.h"
#include "Renderer.h"
#include "Application.h"

int win_width = 640;
int win_height = 480;
const char* win_title = "Hello";

const char* shader_path = "res/shader/vert_frag.shader";


int main(void)
{
    Application app(win_width, win_height, win_title);
    if (!app.m_window.Init())
        return -1;
    Renderer::Init();

    while (!glfwWindowShouldClose(app.m_window.GetWindow()))
    {
        // glClear(GL_COLOR_BUFFER_BIT);
        Renderer::Clear();

        Renderer::DrawTriangle();
        
        app.OnUpdate();


        glfwSwapBuffers(app.m_window.GetWindow());
        glfwPollEvents();
    }

    return 0;
}