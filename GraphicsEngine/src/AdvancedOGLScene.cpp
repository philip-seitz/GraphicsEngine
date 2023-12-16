#include "AdvancedOGLScene.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "Renderer.h"
#include "Camera.h"
#include "ModelSceneInstance.h"
#include "Point.h"

AdvancedOGLScene::AdvancedOGLScene():
    m_window(1000, 500, "Advanced")
{
	m_ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
}

AdvancedOGLScene::AdvancedOGLScene(const int& width, const int& height, const char* title)
    :m_window(width, height, title)
{
	m_ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
}

AdvancedOGLScene::~AdvancedOGLScene()
{
}

void AdvancedOGLScene::Run()
{
    if (!m_window.Init())       // setup OpenGl context
        std::exit(0);

    static float lastTime = glfwGetTime();
    Renderer::Init(&m_window);

    for (int i = 0; i < 1; i++)
    {
        Camera cam(true);
        m_SceneGraph.m_Cameras.push_back(cam);
    }

    // create point light
    for (int i = 0; i < 1; i++)
    {
        PointLight testLight;
        testLight.m_Instance.position = { i * 3.0f, 4.0f, 4.0f };
        m_SceneGraph.m_PointLights.push_back(testLight);
    }

    DirectionalLight dirLight;
    dirLight.m_Direction = { -1.0f, -0.5f, 2.0f };
    // m_SceneGraph.m_DirectionalLights.push_back(dirLight);

    // add cubes 
    for (int i = 0; i < 0; i++)
    {
        Cube t_cube;
        t_cube.m_Instance.position = { 0.0f + i * 1.0f, 1.52f, -1.0f + 3.0f*i };
        m_SceneGraph.m_Cubes.push_back(t_cube);
    }

    // add quads (vegetation or other textures)
    for (int i = 0; i < 0; i++)
    {
        Quad quad;
        quad.m_Instance.position = { -3.0f + i * 3.0f, 2.52f, -3.0f - i };
        m_SceneGraph.m_Quads.push_back(quad);
    }

    GridInstance grid;
    // m_SceneGraph.m_Grids.push_back(grid);

    // create floor
    Cube floor;
    floor.m_Instance.position = { 0.0f, 0.0f, 0.0f };
    floor.m_Instance.scale = { 20.0f, 0.5f, 15.0f };
    floor.m_Instance.hasOutline = false;
    m_SceneGraph.m_Cubes.push_back(floor);

    // add a backback model
    for (int i = 0; i < 0; i++)
    {
        ModelSceneInstance backpack;
        backpack.m_position = { 0.0f + 4 * i, 7.0f, 0.0f };
        m_SceneGraph.m_Backpacks.push_back(backpack);
    }

    for (int i = 0; i < 0; i++)
    {
        Point point;
        if (i < 2)
        {
            point.m_position = { -0.5f + 1.0f * float(i), 0.5f, 0.0f };
        }
        else 
            point.m_position = { -0.5f + 1.0f * float(i-2), -0.5f, 0.0f };
        m_SceneGraph.m_Points.push_back(point);
    }

    // instancing stuff 
    // could add multiple quads that would be instanced with how instancing should work etc
    Quad quadInst;
    m_SceneGraph.m_Quads.push_back(quadInst);

    // planets 
    ModelSceneInstance planet;
    planet.m_position = { 0.0f, 3.0f, 0.0f };
    planet.m_scale = { 4.0f, 4.0f, 4.0f };
    m_SceneGraph.m_Planets.push_back(planet);

    // used for instancing rocks (at some point)
    ModelSceneInstance rock;
    m_SceneGraph.m_Rocks.push_back(rock);


    // render loop
    while (!glfwWindowShouldClose(m_window.GetWindow()))
    {
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;
        OnUpdate();
        OnRender();
        OnEvent(dt);
    }
    Renderer::Shutdown();
}

void AdvancedOGLScene::OnUpdate()
{
   
}

void AdvancedOGLScene::OnRender()
{
    // glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    // Renderer::Clear();
    Renderer::RenderSceneGraph(&m_SceneGraph);
    glfwSwapBuffers(m_window.GetWindow());
}

void AdvancedOGLScene::OnEvent(float& dt)
{
    glfwPollEvents();
    // check for window termination 
    if (glfwGetKey(m_window.GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_window.GetWindow(), true);
    }

    if (glfwGetKey(m_window.GetWindow(), GLFW_KEY_B) == GLFW_PRESS)
    {
        m_SceneGraph.m_Blinn = !m_SceneGraph.m_Blinn;       // toggle setting in shader
    }

    m_SceneGraph.m_Cameras[0].ManageInputs(m_window, dt);
}
