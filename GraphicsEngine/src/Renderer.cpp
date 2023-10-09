#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"
#include "ModelScene.h"
#include "Util.h"
#include "ModelInstance.h"
#include <iostream>
#include <GL/glew.h>
#include <map>

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
	Model quad;

	// assimp models
	ModelScene backpack;

	// Grid
	Grid grid_xz;

	// shaders
	Shader* shader = nullptr;
	Shader* light_source_shader = nullptr;
	Shader* lighting_shader = nullptr;
	Shader* model_shader = nullptr;
	Shader* texture_shader = nullptr;
	Shader* clear_color_shader = nullptr;

	Window* window = nullptr;
};

static RendererData sData;

void Renderer::Init(Window* window)
{
	sData.window = window;
	sData.light_source_shader = new Shader("res/shader/light_source_shader.glsl");
	sData.model_shader = new Shader("res/shader/model_shader.glsl");
	sData.shader = new Shader("res/shader/vert_frag.glsl");
	sData.texture_shader = new Shader("res/shader/texture_shader.glsl"); // only textures no lighting
	sData.clear_color_shader = new Shader("res/shader/clear_color_shader.glsl");
	//sData.lighting_shader = new Shader("res/shader/lighting_shader.glsl");
	
	// initialize models (send data to GPU)
	sData.primitive_cube.InitSimpleCube();
	sData.cube.InitCube();
	sData.quad.InitQuad();
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
	// enable backface culling
	glEnable(GL_CULL_FACE);

	// Depth Test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Blending 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClear(GL_STENCIL_BUFFER_BIT);
	// enable stencil 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	// if stenicil and depth pass
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);		// enable writing to stencil buffer

	// get main camera 
	// TODO: check for some camera type or flag perhaps
	Camera sg_cam = scene_graph->m_Cameras[0];
	cam = &scene_graph->m_Cameras[0];

	// point lights
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

	sData.model_shader->SetUniformMat4f("u_Projection", sg_cam.ProjectionMatrix());
	sData.model_shader->SetUniformMat4f("u_View", sg_cam.LookAtMatrix());

	sData.clear_color_shader->SetUniformMat4f("u_Projection", sg_cam.ProjectionMatrix());
	sData.clear_color_shader->SetUniformMat4f("u_View", sg_cam.LookAtMatrix());
	
	// draw all objects without outline
	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		if (!scene_graph->m_Cubes[i].m_Instance.hasOutline)
		{
			glStencilMask(0x00);
			scene_graph->m_Cubes[i].Draw(sData.model_shader, &sData.cube);
			glStencilMask(0xFF);
		}
	}
	// draw objects with outline
	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		if (scene_graph->m_Cubes[i].m_Instance.hasOutline)
		{
			scene_graph->m_Cubes[i].Draw(sData.model_shader, &sData.cube);
		}
	}

	// draw outlines
	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		if (scene_graph->m_Cubes[i].m_Instance.hasOutline)
			scene_graph->m_Cubes[i].DrawOutline(sData.model_shader, sData.clear_color_shader, &sData.cube);
	}
	
	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].Draw(sData.model_shader, sData.backpack);
	}

	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].DrawOutline(sData.backpack, sData.model_shader, sData.clear_color_shader);
	}
	sData.texture_shader->SetUniformMat4f("u_Projection", sg_cam.ProjectionMatrix());
	sData.texture_shader->SetUniformMat4f("u_View", sg_cam.LookAtMatrix());

	// NOTE: should be drawn after all opaque objects; to improve performance look into order independent transparency
	// draw windows (transparent objects) from back to front (in terms of distance from camera)
	std::map<float, Quad*> distance_sorted;		// unique float keys, sorted in ascending order (has to be a pointer or it will copy or create a bunch of textures)
	for (int i = 0; i < scene_graph->m_Quads.size(); i++)
	{
		glm::vec3 pos = scene_graph->m_Quads[i].m_Instance.position;
		float distance = glm::distance(sg_cam.Position(), pos);
		distance_sorted[distance] = &scene_graph->m_Quads[i];
	}
	
	// disable backface culling
	glDisable(GL_CULL_FACE);
	for (std::map<float, Quad*>::reverse_iterator rit = distance_sorted.rbegin(); rit != distance_sorted.rend(); ++rit)
	{
		rit->second->Draw(sData.texture_shader, &sData.quad);
	}

	// draw grid (blending will be enabled and disabled again)
	sData.shader->SetUniformMat4f("u_Projection", sg_cam.ProjectionMatrix());
	sData.shader->SetUniformMat4f("u_View", sg_cam.LookAtMatrix());
	for (int i = 0; i < scene_graph->m_Grids.size(); i++)
	{
		scene_graph->m_Grids[i].Draw(sData.grid_xz, sData.shader);
	}
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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