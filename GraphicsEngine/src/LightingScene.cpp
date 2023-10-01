#include "LightingScene.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "Renderer.h"
#include "Camera.h"
#include "ModelSceneInstance.h"


LightingScene::LightingScene():Application()
{
    m_ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
}
LightingScene::LightingScene(const int& width, const int& height, const char* title):Application(width, height, title)
{
    m_ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
}

LightingScene::~LightingScene()
{
}

void LightingScene::Run()
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

    GridInstance grid;
    m_SceneGraph.m_Grid = grid;         // unnecessary bc not a vector but einheitlicher

    // create point light
    for (int i = 0; i < 1; i++)
    {
        PointLight testLight;
        testLight.m_Instance.position = {i * 3.0f, 1.0f, 3.0f };
        m_SceneGraph.m_PointLights.push_back(testLight);
    }

    // create directional light
    for (int i = 0; i < 1; i++)
    {
        DirectionalLight dirLight;
        m_SceneGraph.m_DirectionalLights.push_back(dirLight);
    }
    
    for (int i = 0; i < 1; i++)
    {
        SpotLight spotLight;
        m_SceneGraph.m_SpotLights.push_back(spotLight);
    }

    // add 5 cubes that are spread out
    for (int i = 0; i < 5; i++)
    {
        Cube t_cube;
        t_cube.m_Instance.position = { 0.0f + i * 3.0f, 7.0f + glm::pow(-1, i), 0.0f};
        m_SceneGraph.m_Cubes.push_back(t_cube);
    }
    // add a backback model
    for (int i = 0; i < 2; i++)
    {
        ModelSceneInstance backpack;
        backpack.m_position = { 0.0f + 4*i, 0.0f, 0.0f };
        m_SceneGraph.m_Backpacks.push_back(backpack);
    }
    
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

void LightingScene::OnUpdate()
{
    // move objects around and so on

}

void LightingScene::OnRender()
{
    glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    Renderer::Clear();
    Renderer::RenderSceneGraph(&m_SceneGraph);
    glfwSwapBuffers(m_window.GetWindow());
}

void LightingScene::OnEvent(float& dt)
{
    glfwPollEvents();
    // check for window termination 
    if (glfwGetKey(m_window.GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_window.GetWindow(), true);
    }

    m_SceneGraph.m_Cameras[0].ManageInputs(m_window, dt);
    m_SceneGraph.m_Cameras[0].ManageInputs(m_window, dt);
}
