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

#include "stb_image/stb_image.h"

// constants for vertices and textures
const unsigned int max_vertices = 4000000;
const unsigned int max_indices = max_vertices;
const unsigned int max_texture_slots = 32;				// checken

Camera* cameras[2];										// pointed at the main camera for input handling

struct RendererData
{
	int cubeNumber = 0;
	int lightNumber = 0;

	glm::vec4 light_color = { 0.8f, 0.4f, 0.3f, 1.0f};
	glm::vec4 object_color = { 0.8f, 0.2f, 0.2f, 1.0f };

	// Models
	Model mclaren;
	Model cube;
	// Model triangle;				// TODO: implement pyramid which can be used as triangle in orthographic
	// Model quad;

	// Other objects
	Model primitive_cube;
	Model quad;
	Model screen_quad;
	Model rearview_quad;

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
	Shader* postprocessing_shader = nullptr;
	Shader* cubemap_shader = nullptr;

	Window* window = nullptr;

	// frame buffer oject
	unsigned int fbo_id[2];
	unsigned int fbo_tex[2];
	unsigned int fbo_rbo_id[2];
};

struct Cubemap
{
	unsigned int cubemapID;
	int width, height, nrChannels;
	unsigned char* data;

	glm::vec3 angles = {0.0f, 0.0f, 0.0f};
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::vec3 scale = {1.0f, 1.0f, 1.0f};		// 1x1x1 cube
	void Init()
	{
		// setup cube map (refactor into class later)
		
		glGenTextures(1, &cubemapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

		std::vector<std::string> texture_faces
		{
			"res/textures/skybox/right.jpg",
			"res/textures/skybox/left.jpg",
			"res/textures/skybox/top.jpg",
			"res/textures/skybox/bottom.jpg",
			"res/textures/skybox/front.jpg",
			"res/textures/skybox/back.jpg"
		};

		for (int i = 0; i < texture_faces.size(); i++)
		{
			stbi_set_flip_vertically_on_load(0);		// set to 1 originally doesn't
			data = stbi_load(texture_faces[i].c_str(), &width, &height, &nrChannels, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	void Draw(const Model& base_model, Shader* shader)
	{
		glBindVertexArray(base_model.va);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
		shader->SetUniformMat4f("u_Model", ModelMatrix());
		glDrawElements(GL_TRIANGLES, 3 * base_model.TriCount, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(base_model.va);
		// draw all vertices 
		glDrawElements(GL_TRIANGLES, 3 * base_model.TriCount, GL_UNSIGNED_INT, nullptr);
	}

	glm::mat4x4 ModelMatrix()
	{
		glm::mat4x4 combinedRotation(0);
		glm::mat4x4 translation(0);
		glm::mat4x4 scaling(0);

		combinedRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angles.x),
			glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
				glm::radians(angles.y), glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
					glm::radians(angles.z), glm::vec3(0.0, 0.0, 1.0));
		translation = glm::translate(glm::mat4(1.0f), position);
		scaling = glm::scale(glm::mat4(1.0f), scale);

		return translation * combinedRotation * scaling;
	}
};

// create vao for sky box

static RendererData sData;
Cubemap cube_map;

void Renderer::Init(Window* window)
{
	sData.window = window;
	sData.light_source_shader = new Shader("res/shader/light_source_shader.glsl");
	sData.model_shader = new Shader("res/shader/model_shader.glsl");
	sData.shader = new Shader("res/shader/vert_frag.glsl");
	sData.texture_shader = new Shader("res/shader/texture_shader.glsl");			// only textures no lighting
	sData.clear_color_shader = new Shader("res/shader/clear_color_shader.glsl");
	sData.postprocessing_shader = new Shader("res/shader/postprocessing.glsl");
	sData.cubemap_shader = new Shader("res/shader/cubemap_shader.glsl");
	//sData.lighting_shader = new Shader("res/shader/lighting_shader.glsl");
	
	// initialize models (send data to GPU)
	sData.primitive_cube.InitSimpleCube();
	sData.cube.InitCube();
	sData.quad.InitQuad();
	sData.screen_quad.InitQuad(2.0f, {0.0f, 0.0f, -0.7f});
	sData.rearview_quad.InitQuad(0.5f, { 0.75f, 0.5f, -0.5f });

	sData.grid_xz.InitGridxz();
	sData.backpack = ModelScene("res/models/backpack/backpack.obj");

	cube_map.Init();

	for (int i = 0; i < 2; i++)
	{
		// code for framebuffer object 
		glGenFramebuffers(1, &sData.fbo_id[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[i]);

		// texture attachment
		glGenTextures(1, &sData.fbo_tex[i]);
		glActiveTexture(GL_TEXTURE0 + 5 + i);
		glBindTexture(GL_TEXTURE_2D, sData.fbo_tex[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sData.window->m_Width, sData.window->m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sData.fbo_tex[i], 0);

		// renderbuffer attachment

		glGenRenderbuffers(1, &sData.fbo_rbo_id[i]);
		glBindRenderbuffer(GL_RENDERBUFFER, sData.fbo_rbo_id[i]);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, sData.window->m_Width, sData.window->m_Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sData.fbo_rbo_id[i]);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
			std::cout << "FBO SUCCESSFUL" << std::endl;
		else
			std::cout << "FBO FAILED" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
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
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	// if stencil and depth pass
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);						// enable writing to stencil buffer

	cameras[0] = &scene_graph->m_Cameras[0];

	// point lights
	sData.light_source_shader->SetUniformMat4f("u_Projection", cameras[0]->ProjectionMatrix());
	sData.light_source_shader->SetUniformMat4f("u_View", cameras[0]->LookAtMatrix());
	sData.model_shader->SetUniform3f("u_CameraPosition", cameras[0]->Position());
	sData.model_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());
	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		scene_graph->m_PointLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		// scene_graph->m_PointLights[i].SetLightingUniforms(sData.lighting_shader, i);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.model_shader, i);
	}

	for (int i = 0; i < scene_graph->m_DirectionalLights.size(); i++)
	{
		// scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.lighting_shader);
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.model_shader);
	}

	for (int i = 0; i < scene_graph->m_SpotLights.size(); i++)
	{
		// as flash light
		scene_graph->m_SpotLights[i].m_Direction = cameras[0]->LookDirection();
		scene_graph->m_SpotLights[i].m_Instance.position = cameras[0]->Position();

		scene_graph->m_SpotLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		// scene_graph->m_PointLights[i].SetLightingUniforms(sData.lighting_shader, i);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.model_shader);
	}

	sData.model_shader->SetUniformMat4f("u_Projection", cameras[0]->ProjectionMatrix());
	sData.model_shader->SetUniformMat4f("u_View", cameras[0]->LookAtMatrix());

	sData.clear_color_shader->SetUniformMat4f("u_Projection", cameras[0]->ProjectionMatrix());
	sData.clear_color_shader->SetUniformMat4f("u_View", cameras[0]->LookAtMatrix());

	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		scene_graph->m_Cubes[i].Draw(sData.model_shader, &sData.cube);
	}

	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].Draw(sData.model_shader, sData.backpack);
	}

	sData.texture_shader->SetUniformMat4f("u_Projection", cameras[0]->ProjectionMatrix());
	sData.texture_shader->SetUniformMat4f("u_View", cameras[0]->LookAtMatrix());

	

	// draw sky box after drawing all models (as optimization)
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	sData.cubemap_shader->SetUniformMat4f("u_Projection", cameras[0]->ProjectionMatrix());
	sData.cubemap_shader->SetUniformMat4f("u_View", glm::mat3(cameras[0]->LookAtMatrix()));
	cube_map.Draw(sData.primitive_cube, sData.cubemap_shader);

	// enable depth buffer
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// draw grid (after sky box to benefit from blending with skybox colors)
	sData.shader->SetUniformMat4f("u_Projection", cameras[0]->ProjectionMatrix());
	sData.shader->SetUniformMat4f("u_View", cameras[0]->LookAtMatrix());
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

	cameras[0]->UpdateFromMouseCallback(xoffset, yoffset);
	// cameras[1]->UpdateFromMouseCallback(xoffset, yoffset);

	lastx = xpos;
	lasty = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cameras[0]->UpdateFromScrollCallback((float)xoffset, (float)yoffset);
	// cameras[1]->UpdateFromScrollCallback((float)xoffset, (float)yoffset);
}