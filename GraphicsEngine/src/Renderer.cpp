#include "Renderer.h"
#include <GL/glew.h>
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"
#include "ModelScene.h"
#include <iostream>
#include "Util.h"
#include "ModelInstance.h"

// constants for vertices and textures
const unsigned int max_vertices = 4000000;
const unsigned int max_indices = max_vertices;
const unsigned int max_texture_slots = 32;				// checken

Camera camera(true);
Camera* cam = nullptr;		// pointed at the main camera for input handling

struct RendererData
{
	int cubeNumber = 0;
	int lightNumber = 0;

	glm::vec4 light_color = { 0.8f, 0.4f, 0.3f, 1.0f};
	glm::vec4 object_color = { 0.8f, 0.2f, 0.2f, 1.0f };

	// Models
	Model mclaren;
	Model cube;
	// Model triangle;		// TODO: implement pyramid which can be used as triangle in orthographic
	// Model quad;

	// Other objects
	Model primitive_cube;

	// assimp models
	ModelScene backpack;

	// Grid
	Grid grid_xz;

	// shaders
	Shader* shader = nullptr;
	Shader* light_source_shader = nullptr;
	Shader* lighting_shader = nullptr;
	Shader* model_shader = nullptr;

	Window* window = nullptr;
};

static RendererData sData;

void Renderer::Init(Window* window)
{
	sData.window = window;
	sData.light_source_shader = new Shader("res/shader/light_source_shader.glsl");
	sData.model_shader = new Shader("res/shader/model_shader.glsl");
	sData.shader = new Shader("res/shader/vert_frag.glsl");

	//sData.lighting_shader = new Shader("res/shader/lighting_shader.glsl");
	
	// initialize models (send data to GPU)
	sData.primitive_cube.InitSimpleCube();
	sData.cube.InitCube();
	sData.grid_xz.InitGridxz();
	sData.backpack = ModelScene("res/models/backpack/backpack.obj");
	
}

void Renderer::Shutdown()
{
	delete sData.shader;
	delete sData.light_source_shader;
	delete sData.lighting_shader;
}

// For Batch Rendering (implement when needed -> dynamic draw)
// -----------------------------------------------------------
void Renderer::BeginBatch()
{
}

void Renderer::EndBatch()
{
}

void Renderer::Flush()
{
}
// ------------------------------------------------------------


int Renderer::ModelGridLayout(ModelInstance** inst, const int& number, const glm::vec3& pos)
{
	// place multiple objects in grid

	float gap = 4.0f;
	int width = glm::sqrt(number);				// model grid width
	int height = width;							// model grid height
	int num = width * height;

	*inst = new ModelInstance[num];

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			
			int idx = j + width * i;
			(*(inst)+idx)->position = glm::vec3{ pos.x + j * gap, pos.y, pos.z + i * gap };
		}
	}
	return num;
}

void Renderer::RenderSceneGraph(SceneGraph* scene_graph)
{
	// Blending 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// get main camera 
	// TODO: check for some camera type or flag perhaps
	Camera sg_cam = scene_graph->m_Cameras[0];
	cam = &scene_graph->m_Cameras[0];

	// point lights
	// set uniforms for point lights
	sData.light_source_shader->SetUniformMat4f("u_Projection", sg_cam.ProjectionMatrix());
	sData.light_source_shader->SetUniformMat4f("u_View", sg_cam.LookAtMatrix());
	sData.model_shader->SetUniform3f("u_CameraPosition", sg_cam.Position());

	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		scene_graph->m_PointLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		// scene_graph->m_PointLights[i].SetLightingUniforms(sData.lighting_shader, i);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.model_shader, i);
	}

	for (int i = 0; i < scene_graph->m_DirectionalLights.size(); i++)
	{
		//scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.lighting_shader);
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.model_shader);
	}

	for (int i = 0; i < scene_graph->m_SpotLights.size(); i++)
	{
		// as flash light
		scene_graph->m_SpotLights[i].m_Direction = sg_cam.LookDirection();
		scene_graph->m_SpotLights[i].m_Instance.position = sg_cam.Position();

		scene_graph->m_SpotLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		// scene_graph->m_PointLights[i].SetLightingUniforms(sData.lighting_shader, i);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.model_shader);

	}

	/*sData.model_shader->SetUniformMat4f("u_Projection", camera.ProjectionMatrix());
	sData.model_shader->SetUniformMat4f("u_View", camera.LookAtMatrix());*/
	sData.model_shader->SetUniformMat4f("u_Projection", sg_cam.ProjectionMatrix());
	sData.model_shader->SetUniformMat4f("u_View", sg_cam.LookAtMatrix());

	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		scene_graph->m_Cubes[i].Draw(sData.model_shader, &sData.cube);
	}

	// sData.backpack.Draw(sData.model_shader);
	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].Draw(sData.model_shader, sData.backpack);
	}

	// draw grid (blending will be disabled)
	sData.shader->SetUniformMat4f("u_Projection", sg_cam.ProjectionMatrix());
	sData.shader->SetUniformMat4f("u_View", sg_cam.LookAtMatrix());
	scene_graph->m_Grid.Draw(sData.grid_xz, sData.shader);
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Callback functions 
// -------------------------------------------------------------------------------------------

// mouse input callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	const static float sens = 0.2f;
	static float lastx = 400;
	static float lasty = 300;
	static bool init = false;

	if (!init)
	{
		lastx = xpos;
		lasty = ypos;
		init = true;
	}

	float xoffset = (xpos - lastx) * sens;
	float yoffset = (lasty - ypos) * sens;

	cam->UpdateFromMouseCallback(xoffset, yoffset);

	lastx = xpos;
	lasty = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam->UpdateFromScrollCallback((float)xoffset, (float)yoffset);
}