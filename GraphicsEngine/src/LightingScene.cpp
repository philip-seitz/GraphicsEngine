#include "LightingScene.h"
#include "Renderer.h"


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
    glm::vec3 lightSourcePosition = { -3.0f, 2.5f, 0.0f };
    glm::vec3 objectPosition = { 0.0f, 0.0f, 0.0f };
    if (!m_window.Init())
        std::exit(0);
    Renderer::Init(&m_window);
    Renderer::CreatePrimitiveCube();
    Renderer::SpawnPrimitiveCube(1, lightSourcePosition);
    // Renderer::CreateCube();
    // Renderer::SpawnCube(16, objectPosition);

    // render loop
    while (!glfwWindowShouldClose(m_window.GetWindow()))
    {
        OnUpdate();
        OnRender();
        OnEvent();
    }
    Renderer::Shutdown();
}

void LightingScene::OnUpdate()
{
    static float lastTime = glfwGetTime();
    float currentTime = glfwGetTime();
    float dt = currentTime - lastTime;
    lastTime = currentTime;
    Renderer::Clear();
    // caclulations etc
    glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    Renderer::Calculate(dt);
}
