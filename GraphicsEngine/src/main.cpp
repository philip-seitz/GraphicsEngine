#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "Window.h"
#include "Renderer.h"
#include "Application.h"

int win_width = 1080;
int win_height = 720;
const char* win_title = "3D Engine";
const char* shader_path = "res/shader/vert_frag.shader";

int main(void)
{
    Application app(win_width, win_height, win_title);
    app.Run();

    return 0;
}