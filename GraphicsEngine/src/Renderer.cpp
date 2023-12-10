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
#include "vendor/glm/gtc/type_ptr.hpp"

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

	// Other objects
	Model primitive_cube;
	Model quad;
	Model point;
	Model screen_quad;
	Model rearview_quad;

	// for instancing quad

	Model quad_inst;

	// assimp models
	ModelScene backpack;
	ModelScene rock;
	ModelScene planet;

	// Grid
	Grid grid_xz;

	// offset vectors for instanced rendering
	glm::vec2 offsets[100];


	// asteroid model matrices
	glm::mat4* matrices = nullptr;


	// shaders
	Shader* shader = nullptr;
	Shader* light_source_shader = nullptr;
	Shader* lighting_shader = nullptr;
	Shader* model_shader = nullptr;
	Shader* texture_shader = nullptr;
	Shader* clear_color_shader = nullptr;
	Shader* postprocessing_shader = nullptr;
	Shader* cubemap_shader = nullptr;
	Shader* geometry_shader = nullptr;
	Shader* instancing_shader = nullptr;
	Shader* asteroids_instanced = nullptr;

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
		
		// === those two lines are not needed somehow (works without them) ===
		glGenTextures(1, &cubemapID);
		glActiveTexture(GL_TEXTURE0 + 10);
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
			stbi_set_flip_vertically_on_load(0);		// set to 1 originally. cubemap textures are not inverted (unlike other textures)
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
		// glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);		// not necessary for cubemap
		shader->SetUniform1i("u_Skybox", 10);
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


struct UniformBufferObject
{
	
	unsigned int uboID = 0;

	// size: total size corresponding to std140 
	void InitBuffer(unsigned int size, const std::string& uniform_name, unsigned int bindingPointNumber, std::vector<Shader*> shader_list)
	{
		unsigned int uniformBlockID = 0;
		// Binding Point
		for (int i = 0; i < shader_list.size(); i++)
		{
			uniformBlockID = glGetUniformBlockIndex(shader_list[i]->GetRendererID(), uniform_name.c_str());
			glUniformBlockBinding(shader_list[i]->GetRendererID(), uniformBlockID, bindingPointNumber);
		}
		
		// Buffer
		glGenBuffers(1, &uboID);
		glBindBuffer(GL_UNIFORM_BUFFER, uboID);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);					// might have to be dynamic draw
		glBindBuffer(GL_UNIFORM_BUFFER, 0);												// unbind

		glBindBufferRange(GL_UNIFORM_BUFFER, bindingPointNumber, uboID, 0, size);		// use entire buffer memory and set to binding point number
	}

	void SetMat4f(unsigned int offset, glm::mat4 data)
	{
		// setup Uniform Buffer Object (each frame)
		glBindBuffer(GL_UNIFORM_BUFFER, uboID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), glm::value_ptr(data));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void SetVec3f(unsigned int offset, glm::vec3 data)
	{
		// setup Uniform Buffer Object (each frame)
		glBindBuffer(GL_UNIFORM_BUFFER, uboID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), glm::value_ptr(data));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	void SetVec4f(unsigned int offset, glm::vec4 data)
	{
		// setup Uniform Buffer Object (each frame)
		glBindBuffer(GL_UNIFORM_BUFFER, uboID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec4), glm::value_ptr(data));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
};

// create vao for sky box
static RendererData sData;
Cubemap cube_map;
UniformBufferObject uboMatrix;
UniformBufferObject uboCamera;

// setup quad va for instancing (def refactor needed)
std::vector<Triangle> quad_vertices;

std::vector<TriangleIndices> quad_indices;
unsigned int number_asteroids = 100000;

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
	sData.geometry_shader = new Shader("res/shader/geometry.glsl");
	sData.instancing_shader = new Shader("res/shader/instancing_shader.glsl");
	sData.asteroids_instanced = new Shader("res/shader/asteroids_instanced.glsl");
	// sData.lighting_shader = new Shader("res/shader/lighting_shader.glsl");
	
	// initialize models (send data to GPU)
	sData.primitive_cube.InitSimpleCube();
	sData.cube.InitCube();
	sData.quad.InitQuad();		
	sData.quad_inst.InitQuad(0.1f, glm::vec3(-0.9f, 0.9f, 0.0f));	// for instanced rendering (usually with default values
	sData.point.InitPoint();
	sData.screen_quad.InitQuad(2.0f, {0.0f, 0.0f, -0.7f});
	sData.rearview_quad.InitQuad(0.5f, { 0.75f, 0.5f, -0.5f });

	sData.grid_xz.InitGridxz();
	// sData.backpack = ModelScene("res/models/backpack/backpack.obj");
	sData.rock = ModelScene("res/models/rocky/rocky.obj");
	sData.planet = ModelScene("res/models/planet/planet.obj");
	cube_map.Init();

	// list of shaders that should be bound to binding point of u_Matrices Uniform Buffer
	std::vector<Shader*> shader_list_matrices;
	shader_list_matrices.push_back(sData.cubemap_shader);
	shader_list_matrices.push_back(sData.light_source_shader);
	shader_list_matrices.push_back(sData.clear_color_shader);
	shader_list_matrices.push_back(sData.texture_shader);
	shader_list_matrices.push_back(sData.shader);
	shader_list_matrices.push_back(sData.geometry_shader);
	shader_list_matrices.push_back(sData.model_shader);
	shader_list_matrices.push_back(sData.asteroids_instanced);
	uboMatrix.InitBuffer(2 * sizeof(glm::mat4), "u_Matrices", 0, shader_list_matrices);

	// list of shaders that should be bound to binding point of u_CameraData Uniform Buffer
	std::vector<Shader*> shader_list_camera;
	shader_list_camera.push_back(sData.shader);
	shader_list_camera.push_back(sData.model_shader);
	shader_list_camera.push_back(sData.asteroids_instanced);
	uboCamera.InitBuffer(sizeof(glm::vec3), "u_CameraData", 1, shader_list_camera);

	// set offsets for quads
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			sData.offsets[10 * i + j] = { 0.0f + 0.2f * float(j), 0.0f - 0.2f * float(i) };
		}
	}
	sData.quad_inst.AddInstancedArray(sData.offsets);

	// set model matrices for asteroids (asteroid ring)
	
	float radius = 150.0f;
	float offset = 25.0f;
	sData.matrices = new glm::mat4[number_asteroids];
	srand(glfwGetTime());		// random seed

	for (int i = 0; i < number_asteroids; i++)
	{
		glm::mat4 model(1.0f);
		float angle = float(i) / float(number_asteroids) * 360.0f;

		// translate
		float displacement = rand() % (int(2 * offset)) - offset;
		float x = radius * cos(angle) + displacement;

		displacement = rand() % (int(2 * offset)) - offset;
		float y = 0.2f * displacement;

		displacement = rand() % (int(2 * offset)) - offset;
		float z = radius * sin(angle) + displacement;

		model = glm::translate(model, glm::vec3(x, y, z));

		// scale (between 0.05 and 0.25)
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// rotate (around axis)
		float rot_angle = rand() % 360;
		model = glm::rotate(model, rot_angle, glm::vec3(0.5f, 0.6f, 0.2f));
		
		sData.matrices[i] = model;
	}

	sData.rock.AddInstArrayAttribMat4(sData.matrices, number_asteroids);
}


void Renderer::Shutdown()
{
	delete sData.shader;
	delete sData.light_source_shader;
	delete sData.lighting_shader;
	
	delete[] sData.matrices;
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

	uboMatrix.SetMat4f(0, cameras[0]->ProjectionMatrix());
	uboMatrix.SetMat4f(sizeof(glm::mat4), cameras[0]->LookAtMatrix());

	uboCamera.SetVec3f(0, cameras[0]->Position());

	// point lights
	sData.model_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());
	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		scene_graph->m_PointLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		// scene_graph->m_PointLights[i].SetLightingUniforms(sData.lighting_shader, i);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.model_shader, i);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.asteroids_instanced, i);
	}

	for (int i = 0; i < scene_graph->m_DirectionalLights.size(); i++)
	{
		// scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.lighting_shader);
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.model_shader);
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.asteroids_instanced);
	}

	for (int i = 0; i < scene_graph->m_SpotLights.size(); i++)
	{
		// as flash light
		scene_graph->m_SpotLights[i].m_Direction = cameras[0]->LookDirection();
		scene_graph->m_SpotLights[i].m_Instance.position = cameras[0]->Position();

		scene_graph->m_SpotLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		// scene_graph->m_PointLights[i].SetLightingUniforms(sData.lighting_shader, i);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.model_shader);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.asteroids_instanced);
	}

	sData.model_shader->SetUniform1i("u_Skybox", 10);

	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		scene_graph->m_Cubes[i].Draw(sData.clear_color_shader, &sData.cube);
	}

	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].Draw(sData.model_shader, sData.backpack);
		float time = glfwGetTime();
		sData.geometry_shader->SetUniform1f("u_Time", time);
		// scene_graph->m_Backpacks[i].Draw(sData.geometry_shader, sData.backpack);
	}

	for (int i = 0; i < scene_graph->m_Points.size(); i++)
	{
		scene_graph->m_Points[i].Draw(sData.geometry_shader, &sData.point);
	}

	// draw sky box after drawing all models (as optimization)
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	// cube_map.Draw(sData.primitive_cube, sData.cubemap_shader);

	// enable depth buffer
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// draw grid (after sky box to benefit from blending with skybox colors)

	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map.cubemapID);

	// send offset uniform(s)

	for (int i = 0; i < 100; i++)
	{
		// sData.instancing_shader->SetUniform2f("offsets[" + std::to_string(i) + "]", sData.offsets[i]);
	}

	sData.instancing_shader->SetUniformMat4f("u_Projection", cameras[0]->ProjectionMatrix());
	sData.instancing_shader->SetUniformMat4f("u_View", cameras[0]->LookAtMatrix());

	// scene_graph->m_Quads[0].DrawInstanced(sData.instancing_shader, &sData.quad_inst, 100);	// idx 0 -> "quad inst" in this case
	// scene_graph->m_Quads[0].Draw(sData.clear_color_shader, &sData.quad);

	for (int i = 0; i < scene_graph->m_Planets.size(); i++)
	{
		scene_graph->m_Planets[i].Draw(sData.model_shader, sData.planet);
	}
	
	scene_graph->m_Rocks[0].DrawInstanced(sData.asteroids_instanced, sData.rock, sData.matrices, number_asteroids);

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