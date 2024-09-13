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

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            PointLight pl(glm::vec3(300.0f, 300.0f, 300.0f));
            pl.m_Instance.position = { (float)j * 4.0f, (float)i * 4.0f + 7.5f, 10.0f };
            m_SceneGraph.m_PointLights.push_back(pl);
        }
    }
   

    DirectionalLight dirLight;
    dirLight.m_Direction = { -1.0f, -3.0f, 2.0f };
    // m_SceneGraph.m_DirectionalLights.push_back(dirLight);

    SpotLight spotLight;
    // m_SceneGraph.m_SpotLights.push_back(spotLight);
    // add cubes 

    Cube big_box;
    big_box.m_Instance.position = { -20.0f, 0.0f, 0.0f};
    big_box.m_Instance.scale = { 4.0f, 4.0f, 4.0f };
    // m_SceneGraph.m_Cubes.push_back(big_box);

    unsigned int grid_size = 7;
    Sphere t_sphere;

    for (int y = 0; y < grid_size; y++)
    {
        for (int x = 0; x < grid_size; x++)
        {
            t_sphere.m_Instance.position = { (float)x * 3.0f, (float)y * 3.0f, 0.0f };
            m_SceneGraph.m_Spheres.push_back(t_sphere);
        }
    }

    for (int i = 0; i < 0; i++)
    {
        Cube t_cube;
        t_cube.m_Instance.position = { 3.0f * i, 0.0f, -3.0f};
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
    floor.m_Instance.scale = { 1.0f, 0.2f, 100.0f };
    floor.m_Instance.hasOutline = false;
    // m_SceneGraph.m_Cubes.push_back(floor);

    Cube wall;
    // wall right side
    wall.m_Instance.position = { 1.1f, 1.0f, 0.0f };
    wall.m_Instance.scale = { 0.2f, 1.0f, 100.0f };
   //  m_SceneGraph.m_Cubes.push_back(wall);

    // wall left side
    wall.m_Instance.position = { -1.1f, 1.0f, 0.0f };
    wall.m_Instance.scale = { 0.2f, 1.0f, 100.0f };
    // m_SceneGraph.m_Cubes.push_back(wall);

    // wall front 
    wall.m_Instance.position = { 0.0f, 1.0f, 10.1f };
    wall.m_Instance.scale = { 1.0f, 1.0f, 0.2f };
    // m_SceneGraph.m_Cubes.push_back(wall);

    // wall back
    wall.m_Instance.position = { 0.0f, 1.0f, -10.1f };
    wall.m_Instance.scale = { 1.0f, 1.0f, 0.2f };
    // m_SceneGraph.m_Cubes.push_back(wall);


    Cube roof;
    roof.m_Instance.position = { 0.0f, 2.0f, 0.0f };
    roof.m_Instance.scale = { 1.0f, 0.2f, 100.0f };
    // m_SceneGraph.m_Cubes.push_back(roof);

    Cube light_cube;        // as visualization 
    light_cube.m_Instance.position = { -7.0f, 0.0f, -4.0f};
    light_cube.m_Instance.scale = { 0.1f, 0.1f, 0.1f };
    // m_SceneGraph.m_Cubes.push_back(light_cube);


    // add a backback model
    for (int i = 0; i < 0; i++)
    {
        ModelSceneInstance backpack;
        backpack.m_position = { 3 * i, 0.0f, -3.0f };
        backpack.m_angles = { 0, -40.0f, 0.0f }; 
        m_SceneGraph.m_Backpacks.push_back(backpack);
    }

    // add material ball model
    for (int i = 0; i < 1; i++)
    {
        ModelSceneInstance material_ball;
        material_ball.m_position = { 3 * i, 0.0f, -3.0f };
        material_ball.m_angles = { 0, -40.0f, 0.0f };
        m_SceneGraph.m_MaterialBalls.push_back(material_ball);
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
    quadInst.m_Instance.position = { 0.0f, 0.0f, -0.5f };
    quadInst.m_Instance.scale = { 1.0f, 1.0f, 1.0f };
    quadInst.m_Instance.angles = { 0.0f, 0.0f, 0.0f };
    // m_SceneGraph.m_Quads.push_back(quadInst);

    // planets 
    ModelSceneInstance planet;
    planet.m_position = { 0.0f, 3.0f, 0.0f };
    planet.m_scale = { 4.0f, 4.0f, 4.0f };
    // m_SceneGraph.m_Planets.push_back(planet);

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
