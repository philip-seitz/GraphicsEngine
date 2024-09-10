#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"
#include "ModelScene.h"
#include "Util.h"
#include "ModelInstance.h"
#include "Cubemap.h"
#include "UniformBufferObject.h"
#include <iostream> 
#include <random>
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
	Model sphere;

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
	Shader* depth_shader = nullptr;
	Shader* shadow_map_shader = nullptr;
	Shader* depth_cubemap_shader = nullptr;
	Shader* shadow_cubemap_shader = nullptr;
	Shader* normalmap_shader = nullptr;
	Shader* parallax_shader = nullptr;
	Shader* hdr_shader = nullptr;
	Shader* g_buffer_shader = nullptr;
	Shader* lighting_pass_shader = nullptr;
	Shader* ssao_geometry_shader = nullptr;
	Shader* ssao_shader = nullptr;
	Shader* ssao_lighting_pass_shader = nullptr;
	Shader* ssao_blur_shader = nullptr;
	Shader* pbr_shader = nullptr;
	Shader* equi_to_cubemap_shader = nullptr;
	Shader* pbr_cubemap_shader = nullptr;
	Shader* pbr_irr_convolution_shader = nullptr;

	Window* window = nullptr;

	// frame buffer oject
	unsigned int fbo_id[6];					// extra for hdr framebuffer, for deferred shading, ssao and ssao blur (this is rediculously stupid)
	unsigned int fbo_tex[6];
	unsigned int fbo_rbo_id[6];

	// for shadow map TODO: refactor
	unsigned int shadow_map_id;

	unsigned int colorBuffer;

	// ids of deferred shading textures
	unsigned int g_position, g_normal, g_colorspec;

	// ids for ssao 
	unsigned int ssao_noise_texture, ssao_color_buffer;

	// ids for ssao blur
	unsigned int ssao_blur_color_buffer;

	// fields for ssao kernel
	std::vector<glm::vec3> ssaoKernel;
	std::vector<glm::vec3> ssaoNoise;

	// fields for pbr 
	unsigned int capture_fbo_id, capture_rbo_id;
	Cubemap env_cubemap;
	Cubemap irr_cubemap;
	Texture equi_rect_map;
};

// create vao for sky box
static RendererData sData;
Cubemap cube_map;
Cubemap depth_cubemap;
UniformBufferObject uboMatrix;
UniformBufferObject uboCamera;
UniformBufferObject uboLightSpaceMatrix;

// setup quad va for instancing (def refactor needed)
std::vector<Triangle> quad_vertices;

std::vector<TriangleIndices> quad_indices;
unsigned int number_asteroids = 0;

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
	sData.depth_shader = new Shader("res/shader/depth_shader.glsl");
	sData.shadow_map_shader = new Shader("res/shader/shadow_map_shader.glsl");
	sData.depth_cubemap_shader = new Shader("res/shader/depth_cubemap_shader.glsl");
	sData.shadow_cubemap_shader = new Shader("res/shader/shadow_cubemap_shader.glsl");
	sData.normalmap_shader = new Shader("res/shader/normalmap_shader.glsl");
	sData.parallax_shader = new Shader("res/shader/parallax_shader.glsl");
	sData.hdr_shader = new Shader("res/shader/hdr_shader.glsl");
	sData.g_buffer_shader = new Shader("res/shader/g_buffer_shader.glsl");
	sData.lighting_pass_shader = new Shader("res/shader/lighting_pass_shader.glsl");
	sData.ssao_geometry_shader = new Shader("res/shader/ssao_geometry_shader.glsl");
	sData.ssao_shader = new Shader("res/shader/ssao_shader.glsl");
	sData.ssao_lighting_pass_shader = new Shader("res/shader/ssao_lighting_pass_shader.glsl");
	sData.ssao_blur_shader = new Shader("res/shader/ssao_blur_shader.glsl");
	sData.pbr_shader = new Shader("res/shader/pbr_shader.glsl");
	sData.equi_to_cubemap_shader = new Shader("res/shader/equi_to_cubemap_shader.glsl");
	sData.pbr_cubemap_shader = new Shader("res/shader/pbr_cubemap_shader.glsl");
	sData.pbr_irr_convolution_shader = new Shader("res/shader/pbr_irr_convolution_shader.glsl");
	
	// sData.lighting_shader = new Shader("res/shader/lighting_shader.glsl");
	
	// initialize models (send data to GPU)
	sData.primitive_cube.InitSimpleCube();
	sData.cube.InitCube();
	sData.quad.InitQuad();		
	sData.quad_inst.InitQuad(0.1f, glm::vec3(-0.9f, 0.9f, 0.0f));	// for instanced rendering (usually with default values
	sData.point.InitPoint();
	sData.screen_quad.InitQuad(2.0f, {0.0f, 0.0f, -0.7f});
	sData.rearview_quad.InitQuad(0.5f, { 0.75f, 0.5f, -0.5f });
	sData.sphere.InitSphere();


	sData.grid_xz.InitGridxz();
	sData.backpack = ModelScene("res/models/backpack/backpack.obj");
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
	shader_list_matrices.push_back(sData.shadow_map_shader);
	shader_list_matrices.push_back(sData.shadow_cubemap_shader);
	shader_list_matrices.push_back(sData.normalmap_shader);
	shader_list_matrices.push_back(sData.parallax_shader);
	shader_list_matrices.push_back(sData.g_buffer_shader);
	shader_list_matrices.push_back(sData.ssao_geometry_shader);
	shader_list_matrices.push_back(sData.ssao_shader);
	shader_list_matrices.push_back(sData.pbr_shader);
	shader_list_matrices.push_back(sData.pbr_cubemap_shader);

	uboMatrix.InitBuffer(2 * sizeof(glm::mat4), "u_Matrices", 0, shader_list_matrices);

	// list of shaders that should be bound to binding point of u_CameraData Uniform Buffer
	std::vector<Shader*> shader_list_camera;
	shader_list_camera.push_back(sData.shader);
	shader_list_camera.push_back(sData.model_shader);
	shader_list_camera.push_back(sData.asteroids_instanced);
	shader_list_camera.push_back(sData.model_shader);
	shader_list_camera.push_back(sData.shadow_map_shader);
	shader_list_camera.push_back(sData.shadow_cubemap_shader);
	shader_list_camera.push_back(sData.normalmap_shader);
	shader_list_camera.push_back(sData.parallax_shader);
	shader_list_camera.push_back(sData.lighting_pass_shader);
	shader_list_camera.push_back(sData.ssao_shader);
	shader_list_camera.push_back(sData.ssao_lighting_pass_shader);
	shader_list_camera.push_back(sData.pbr_shader);
	
	uboCamera.InitBuffer(sizeof(glm::vec3), "u_CameraData", 1, shader_list_camera);
	
	std::vector<Shader*> shader_list_light_space_matrix;
	shader_list_light_space_matrix.push_back(sData.model_shader);
	shader_list_light_space_matrix.push_back(sData.depth_shader);
	shader_list_light_space_matrix.push_back(sData.shadow_map_shader);
	uboLightSpaceMatrix.InitBuffer(2 * sizeof(glm::mat4), "u_LightSpaceMatrices", 2, shader_list_light_space_matrix);

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

	// ==== init framebuffer for shadow mapping; TODO: refactor into seperate struct or class
	glGenFramebuffers(1, &sData.fbo_id[0]);

	glGenTextures(1, &sData.shadow_map_id);
	glBindTexture(GL_TEXTURE_2D, sData.shadow_map_id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// set wrapping to clamp, outside of range use border color (set depth to 1.0)...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float textureBorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, textureBorderColor);	

	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[0]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sData.shadow_map_id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// === setup cubemap stuff for point light shadow mapping
	glGenFramebuffers(1, &sData.fbo_id[1]);

	depth_cubemap.InitDepthCubemap();

	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[1]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap.cubemapID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// === setup framebuffer for HDR rendering === 
	// 
	// code for framebuffer object 
	glGenFramebuffers(1, &sData.fbo_id[2]);
	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[2]);

	// texture attachment
	glGenTextures(1, &sData.colorBuffer);
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_2D, sData.colorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, sData.window->m_Width, sData.window->m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sData.colorBuffer, 0);

	// renderbuffer attachment
	glGenRenderbuffers(1, &sData.fbo_rbo_id[2]);
	glBindRenderbuffer(GL_RENDERBUFFER, sData.fbo_rbo_id[2]);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, sData.window->m_Width, sData.window->m_Height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sData.fbo_rbo_id[2]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO SUCCESSFUL" << std::endl;
	else
		std::cout << "FBO FAILED" << std::endl;



	// === setup for deferred shading scene ===

	// framebuffer for geometry pass
	glGenFramebuffers(1, &sData.fbo_id[3]);
	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[3]);
	
	// position 
	glGenTextures(1, &sData.g_position);
	glBindTexture(GL_TEXTURE_2D, sData.g_position);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, sData.window->m_Width, sData.window->m_Height, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sData.g_position, 0);


	// normal
	glGenTextures(1, &sData.g_normal);
	glBindTexture(GL_TEXTURE_2D, sData.g_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, sData.window->m_Width, sData.window->m_Height, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, sData.g_normal, 0);

	// color spec
	glGenTextures(1, &sData.g_colorspec);
	glBindTexture(GL_TEXTURE_2D, sData.g_colorspec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sData.window->m_Width, sData.window->m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, sData.g_colorspec, 0);

	// which color attachments should be rendered
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	// doesn't draw anything
	glDrawBuffers(3, attachments);

	// renderbuffer for depth
	glGenRenderbuffers(1, &sData.fbo_rbo_id[3]);
	glBindRenderbuffer(GL_RENDERBUFFER, sData.fbo_rbo_id[3]);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, sData.window->m_Width, sData.window->m_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sData.fbo_rbo_id[3]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO DEFERRED SUCCESSFUL" << std::endl;
	else
		std::cout << "FBO DEFERRED FAILED" << std::endl;


	// === setups for ssao scene ===
	// we use the gbuffer here (recycled)

	// normal oriented hemisphere
	// distributed in a way that we get more samples closer to the origin (fragment)
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	// TODO: kernel size should be configurable (not hard coded)

	for (int i = 0; i < 64; i++)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		float scale = (float)i / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		sData.ssaoKernel.push_back(sample);
	}

	// random kernel rotations
	for (unsigned int i = 0; i < 9; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		sData.ssaoNoise.push_back(noise);
	}

	// create 4x4 noise texture
	glGenTextures(1, &sData.ssao_noise_texture);
	glBindTexture(GL_TEXTURE_2D, sData.ssao_noise_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 3, 3, 0, GL_RGB, GL_FLOAT, &sData.ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	// framebuffer for result of ssao
	glGenFramebuffers(1, &sData.fbo_id[4]);
	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[4]);

	// position 
	glGenTextures(1, &sData.ssao_color_buffer);
	glBindTexture(GL_TEXTURE_2D, sData.ssao_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, sData.window->m_Width, sData.window->m_Height, 0, GL_RED, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sData.ssao_color_buffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO SSAO SUCCESSFUL" << std::endl;
	else
		std::cout << "FBO SSAO FAILED" << std::endl;


	// framebuffer for ssao blur 
	glGenFramebuffers(1, &sData.fbo_id[5]);
	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[5]);

	glGenTextures(1, &sData.ssao_blur_color_buffer);
	glBindTexture(GL_TEXTURE_2D, sData.ssao_blur_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, sData.window->m_Width, sData.window->m_Height, 0, GL_RED, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sData.ssao_blur_color_buffer, 0);

	

	// ====

	glBindFramebuffer(GL_FRAMEBUFFER, 0);			// standard framebuffer	
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

bool initialized = false;

void Renderer::RenderSceneGraph(SceneGraph* scene_graph)
{
	cameras[0] = &scene_graph->m_Cameras[0];
	// RenderToDepthMap(scene_graph);
	// RenderSceneWithDepthMap(scene_graph);

	// TODO: refactor setting uniforms into function (for uniform buffer objects especially)

	// RenderToDepthCubemap(scene_graph);
	// RenderSceneWithDepthCubemap(scene_graph);

	// RenderNormalMapScene(scene_graph);

	// RenderHDRScene(scene_graph);

	// RenderDeferredScene(scene_graph);

	// RenderAmbientOcclusionScene(scene_graph);

	if (!initialized)
	{
		PBRScenePrecalculations();
		initialized = true;
	}
	
	RenderPBRScene(scene_graph);
}

void Renderer::PBRScenePrecalculations()
{
	// ==== setup for PBR scene ====

	// equirectangular environment map

	sData.equi_rect_map.InitEquiRectMap("res/equrec_maps/newport_loft.hdr");

	// environment cubemap
	sData.env_cubemap.InitEnvCubemap();
	
	// create frame and renderbuffer objects to capture cube faces

	glGenFramebuffers(1, &sData.capture_fbo_id);
	glGenRenderbuffers(1, &sData.capture_rbo_id);

	glBindFramebuffer(GL_FRAMEBUFFER, sData.capture_fbo_id);
	glBindRenderbuffer(GL_RENDERBUFFER, sData.capture_rbo_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sData.capture_rbo_id);

	// capture cube faces 
	glDisable(GL_CULL_FACE);		// needs to be off, else back faces won't be visible

	glm::mat4 caputureProjection = glm::perspectiveRH(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	glm::mat4 captureViews[] =
	{
		glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
					glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
					glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
					glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
					glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
					glm::vec3(0.0f, -1.0f, 0.0f))
	};

	sData.equi_to_cubemap_shader->Bind();
	sData.equi_to_cubemap_shader->SetUniform1i("u_EquirectangularMap", 0);
	sData.equi_to_cubemap_shader->SetUniformMat4f("u_Projection", caputureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sData.equi_rect_map.GetID());

	
	glBindFramebuffer(GL_FRAMEBUFFER, sData.capture_fbo_id);
	glViewport(0, 0, sData.env_cubemap.width, sData.env_cubemap.height);

	for (int i = 0; i < 6; i++)
	{
		sData.equi_to_cubemap_shader->SetUniformMat4f("u_View", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, sData.env_cubemap.cubemapID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(sData.primitive_cube.va);
		glDrawElements(GL_TRIANGLES, 3 * sData.primitive_cube.TriCount, GL_UNSIGNED_INT, nullptr);
	}

	sData.irr_cubemap.InitIrrCubemap();
	glBindFramebuffer(GL_FRAMEBUFFER, sData.capture_fbo_id);
	glBindRenderbuffer(GL_RENDERBUFFER, sData.capture_rbo_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sData.env_cubemap.cubemapID);
	sData.pbr_irr_convolution_shader->SetUniform1i("u_EnvironmentMap", 0);
	sData.pbr_irr_convolution_shader->SetUniformMat4f("u_Projection", caputureProjection);

	glViewport(0, 0, sData.irr_cubemap.width, sData.irr_cubemap.height);

	for (int i = 0; i < 6; i++)
	{
		sData.pbr_irr_convolution_shader->SetUniformMat4f("u_View", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, sData.irr_cubemap.cubemapID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(sData.primitive_cube.va);
		glDrawElements(GL_TRIANGLES, 3 * sData.primitive_cube.TriCount, GL_UNSIGNED_INT, nullptr);
	}
}

void Renderer::RenderPBRScene(SceneGraph* scene_graph)
{
	// ===== start of frame init =====
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, sData.window->m_Width, sData.window->m_Height);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Depth Test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// enable Backface Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// ===== set shader uniforms =====

	uboMatrix.SetMat4f(0, cameras[0]->ProjectionMatrix());
	uboMatrix.SetMat4f(sizeof(glm::mat4), cameras[0]->LookAtMatrix());
	uboCamera.SetVec3f(0, cameras[0]->Position());

	// render light cubes
	sData.light_source_shader->Bind();
	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		scene_graph->m_PointLights[i].m_Instance.position += glm::vec3(sin(glfwGetTime() * 1.0f) * 0.08, 0.0, 0.0);
		sData.light_source_shader->SetUniform3f("u_LightColor", scene_graph->m_PointLights[i].m_Color);
		scene_graph->m_PointLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);

		scene_graph->m_PointLights[i].SetLightingUniforms(sData.pbr_shader, i);
	}

	sData.pbr_shader->Bind();
	// so we know how many lights we have in our fragment shader
	
	sData.pbr_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());

	// ===== render spheres or other objects =====


	int x_size = glm::sqrt(scene_graph->m_Spheres.size());
	int y_size = x_size;
	for (int y = 0; y < y_size; y++)
	{
		for (int x = 0; x < x_size; x++)
		{
			// NOTE: set pbr uniforms per object (this should ideally be inside a Draw()-function, so i can set all of this in the scene graph)

			// metallicness and roughness from 0.0 to 1.0 across all objects
			sData.pbr_shader->SetUniform1f("u_Metallic", y / (float)(y_size - 1));
			sData.pbr_shader->SetUniform1f("u_Roughness", glm::clamp(x / (float)(x_size - 1), 0.05f, 1.0f));

			scene_graph->m_Spheres[x + x_size * y].Draw(sData.pbr_shader, &sData.sphere);
		}
	}

	// render skybox 
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);		// needs to be off, else back faces won't be visible
	sData.pbr_cubemap_shader->SetUniform1i("u_EnvironmentMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sData.irr_cubemap.cubemapID);		// TODO: revert to env_cubemap (this was for debugging purposes)
	glBindVertexArray(sData.primitive_cube.va);
	glDrawElements(GL_TRIANGLES, 3 * sData.primitive_cube.TriCount, GL_UNSIGNED_INT, nullptr);
}


void Renderer::RenderAmbientOcclusionScene(SceneGraph* scene_graph)
{
	// === render into g-buffer ===
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Depth Test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// enable Backface Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[3]);							// 3 -> g-buffer framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uboMatrix.SetMat4f(0, cameras[0]->ProjectionMatrix());
	uboMatrix.SetMat4f(sizeof(glm::mat4), cameras[0]->LookAtMatrix());
	uboCamera.SetVec3f(0, cameras[0]->Position());

	sData.g_buffer_shader->Bind();

	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		// create normal matrix for view space
		glm::mat4x4 normal_matrix_view = cameras[0]->LookAtMatrix() * scene_graph->m_Backpacks[i].ModelMatrix();
		sData.ssao_geometry_shader->SetUniformMat3f("u_Normal_Matrix_View", glm::inverse(glm::transpose(normal_matrix_view)));
		scene_graph->m_Backpacks[i].Draw(sData.ssao_geometry_shader, sData.backpack);
	}

	// === render into ssao texture using gBuffer ===

	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[4]);							// 4 -> ssao-buffer framebuffer
	glClear(GL_COLOR_BUFFER_BIT);

	// bind position 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sData.g_position);

	// bind normal 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sData.g_normal);

	// bind noise texture 
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sData.ssao_noise_texture);

	sData.ssao_shader->Bind();

	sData.ssao_shader->SetUniform1i("g_Position", 0);
	sData.ssao_shader->SetUniform1i("g_Normal", 1);
	sData.ssao_shader->SetUniform1i("u_NoiseTexture", 2);

	sData.ssao_shader->SetUniform1f("u_ScreenWidth", (float)sData.window->m_Width);
	sData.ssao_shader->SetUniform1f("u_ScreenHeight", (float)sData.window->m_Height);

	// send kernel samples 

	for (int i = 0; i < sData.ssaoKernel.size(); i++)
	{
		sData.ssao_shader->SetUniform3f("u_Samples[" + std::to_string(i) + "]", sData.ssaoKernel[i]);
	}
	
	// draw screened quad
	glDepthMask(GL_FALSE);		// Don't change depth values with the quad
	glBindVertexArray(sData.screen_quad.va);
	glDrawElements(GL_TRIANGLES, 3 * sData.screen_quad.TriCount, GL_UNSIGNED_INT, nullptr);
	glDepthMask(GL_TRUE);		// turn it back on


	// === blur pass ===

	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[5]);							// 5 -> ssao-blur framebuffer
	glClear(GL_COLOR_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sData.ssao_color_buffer);
	
	sData.ssao_blur_shader->Bind();
	sData.ssao_blur_shader->SetUniform1i("u_TextureSSAO", 0);
	
	// draw screened quad
	glDepthMask(GL_FALSE);		// Don't change depth values with the quad
	glBindVertexArray(sData.screen_quad.va);
	glDrawElements(GL_TRIANGLES, 3 * sData.screen_quad.TriCount, GL_UNSIGNED_INT, nullptr);
	glDepthMask(GL_TRUE);		// turn it back on
	
	// === lighting pass ===
	glBindFramebuffer(GL_FRAMEBUFFER, 0);							// 4 -> ssao-buffer framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sData.ssao_blur_color_buffer);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sData.g_position);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sData.g_normal);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sData.g_colorspec);
	
	sData.ssao_lighting_pass_shader->Bind();
	sData.ssao_lighting_pass_shader->SetUniform1i("u_AmbienceSSAO", 0);
	
	sData.ssao_lighting_pass_shader->SetUniform1i("g_Position", 1);
	sData.ssao_lighting_pass_shader->SetUniform1i("g_Normal", 2);
	sData.ssao_lighting_pass_shader->SetUniform1i("g_ColorSpec", 3);
	
	scene_graph->m_PointLights[0].SetLightingUniforms(sData.ssao_lighting_pass_shader, 0);
	
	// draw screened quad
	glDepthMask(GL_FALSE);		// Don't change depth values with the quad
	glBindVertexArray(sData.screen_quad.va);
	glDrawElements(GL_TRIANGLES, 3 * sData.screen_quad.TriCount, GL_UNSIGNED_INT, nullptr);
	glDepthMask(GL_TRUE);		// turn it back on
}


void Renderer::RenderDeferredScene(SceneGraph* scene_graph)
{
	// ===================================== start of frame setup ==================================
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Depth Test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// enable Backface Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[3]);							// 3 -> g-buffer framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glViewport(0.0f, 0.0f, sData.window->m_Width, sData.window->m_Height);

	uboMatrix.SetMat4f(0, cameras[0]->ProjectionMatrix());
	uboMatrix.SetMat4f(sizeof(glm::mat4), cameras[0]->LookAtMatrix());
	uboCamera.SetVec3f(0, cameras[0]->Position());

	sData.g_buffer_shader->Bind();

	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].Draw(sData.g_buffer_shader, sData.backpack);
	}

	// lighting pass 

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sData.lighting_pass_shader->Bind();
	
	// bind position 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sData.g_position);
	
	// bind normal 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sData.g_normal);
	
	// bind color spec 
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sData.g_colorspec);
	
	sData.lighting_pass_shader->SetUniform1i("g_Position", 0);
	sData.lighting_pass_shader->SetUniform1i("g_Normal", 1);
	sData.lighting_pass_shader->SetUniform1i("g_Colorspec", 2);
	
	sData.lighting_pass_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());
	
	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.lighting_pass_shader, i);
	}

	// draw screened quad
	glDepthMask(GL_FALSE);		// Don't change depth values with the quad
	glBindVertexArray(sData.screen_quad.va);
	glDrawElements(GL_TRIANGLES, 3 * sData.screen_quad.TriCount, GL_UNSIGNED_INT, nullptr);
	glDepthMask(GL_TRUE);		// turn it back on

	// copy depth information over from gBuffer to Back buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, sData.fbo_id[3]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, sData.window->m_Width, sData.window->m_Height, 0, 0, sData.window->m_Width, sData.window->m_Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// draw lights on top
	sData.light_source_shader->Bind();

	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		sData.light_source_shader->SetUniform3f("u_LightColor", scene_graph->m_PointLights[i].m_Color);
		scene_graph->m_PointLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
	}
}

void Renderer::RenderHDRScene(SceneGraph* scene_graph)
{
	// NOTE: gamma correction is applied in the hdr shader. 
	// caluclations should all be in linear (diff texture should be in linear aswell)

	// ===================================== start of frame setup ==================================
	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[2]);
	glViewport(0.0f, 0.0f, sData.window->m_Width, sData.window->m_Height);

	// gamma correction in shader
	float clearColorLinear = pow(0.1f, 2.2f);	// clear Color picked based on monitor output sRGB values: vec3(0.1f) -> should be darker for linear)
	glClearColor(clearColorLinear, clearColorLinear, clearColorLinear, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// glEnable(GL_FRAMEBUFFER_SRGB);				// gamma correction 

	// enable Backface Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);			// if stencil and depth pass
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);								// enable writing to stencil buffer

	// ============================= actual rendering code ====================================

	uboMatrix.SetMat4f(0, cameras[0]->ProjectionMatrix());
	uboMatrix.SetMat4f(sizeof(glm::mat4), cameras[0]->LookAtMatrix());
	uboCamera.SetVec3f(0, cameras[0]->Position());

	// point lights uniform setup
	sData.normalmap_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());
	sData.model_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());
	sData.parallax_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());

	// ==== render lighting scene to HDR framebuffer ==== 

	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		sData.light_source_shader->SetUniform3f("u_LightColor", scene_graph->m_PointLights[i].m_Color);
		scene_graph->m_PointLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.normalmap_shader, i);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.model_shader, i);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.parallax_shader, i);
	}

	for (int i = 0; i < scene_graph->m_DirectionalLights.size(); i++)
	{
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.normalmap_shader);
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.model_shader);
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.parallax_shader);
	}

	for (int i = 0; i < scene_graph->m_SpotLights.size(); i++)
	{
		// as flash light
		scene_graph->m_SpotLights[i].m_Direction = cameras[0]->LookDirection();
		scene_graph->m_SpotLights[i].m_Instance.position = cameras[0]->Position();

		scene_graph->m_SpotLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.normalmap_shader);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.model_shader);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.parallax_shader);
	}

	sData.normalmap_shader->SetUniform1i("u_Skybox", 10);

	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		scene_graph->m_Cubes[i].Draw(sData.model_shader, &sData.cube);
	}

	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].Draw(sData.normalmap_shader, sData.backpack);
		float time = glfwGetTime();
		sData.geometry_shader->SetUniform1f("u_Time", time);
		// scene_graph->m_Backpacks[i].Draw(sData.geometry_shader, sData.backpack);				// for displaying normal etc.
	}

	for (int i = 0; i < scene_graph->m_Points.size(); i++)
	{
		scene_graph->m_Points[i].Draw(sData.normalmap_shader, &sData.point);
	}

	for (int i = 0; i < scene_graph->m_Quads.size(); i++)
	{
		scene_graph->m_Quads[i].Draw(sData.parallax_shader, &sData.quad);
	}

	// draw sky box after drawing all models (as optimization)
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	// cube_map.Draw(sData.primitive_cube, sData.cubemap_shader);

	// enable depth buffer
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// draw grid (after sky box to benefit from blending with skybox colors)
	glBindTexture(GL_TEXTURE_2D, cube_map.cubemapID);

	for (int i = 0; i < scene_graph->m_Grids.size(); i++)
	{
		scene_graph->m_Grids[i].Draw(sData.grid_xz, sData.shader);
	}

	// === render to screened quad
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// gamma correction in shader

	sData.hdr_shader->Bind();
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_2D, sData.colorBuffer);
	sData.hdr_shader->SetUniform1i("u_hdrBuffer", 5);
	glBindVertexArray(sData.screen_quad.va);
	glDisable(GL_DEPTH_TEST);

	glDrawElements(GL_TRIANGLES, 3 * sData.screen_quad.TriCount, GL_UNSIGNED_INT, nullptr);
}

void Renderer::RenderNormalMapScene(SceneGraph* scene_graph)
{
	// ===================================== start of frame setup ==================================
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0.0f, 0.0f, sData.window->m_Width, sData.window->m_Height);

	glDisable(GL_FRAMEBUFFER_SRGB);				// gamma correction disabled for setting clear color (so that background color stays the same)
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_FRAMEBUFFER_SRGB);				// gamma correction 

	// enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);			// if stencil and depth pass
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);								// enable writing to stencil buffer

	uboMatrix.SetMat4f(0, cameras[0]->ProjectionMatrix());
	uboMatrix.SetMat4f(sizeof(glm::mat4), cameras[0]->LookAtMatrix());
	uboCamera.SetVec3f(0, cameras[0]->Position());

	// point lights
	sData.normalmap_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());
	sData.model_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());
	sData.parallax_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());
	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		// scene_graph->m_PointLights[i].m_Instance.position.x = glm::sin(glfwGetTime()) * 5.0f;
		// scene_graph->m_PointLights[i].m_Instance.position.z = glm::cos(glfwGetTime()) * 5.0f;
		// scene_graph->m_PointLights[i].m_Instance.position.y = glm::sin(glfwGetTime()) * 6.0f;
		scene_graph->m_PointLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.normalmap_shader, i);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.model_shader, i);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.parallax_shader, i);
	}

	for (int i = 0; i < scene_graph->m_DirectionalLights.size(); i++)
	{
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.normalmap_shader);
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.model_shader);
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.parallax_shader);
	}

	for (int i = 0; i < scene_graph->m_SpotLights.size(); i++)
	{
		// as flash light
		scene_graph->m_SpotLights[i].m_Direction = cameras[0]->LookDirection();
		scene_graph->m_SpotLights[i].m_Instance.position = cameras[0]->Position();

		scene_graph->m_SpotLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.normalmap_shader);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.model_shader);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.parallax_shader);
	}

	sData.normalmap_shader->SetUniform1i("u_Skybox", 10);

	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		scene_graph->m_Cubes[i].Draw(sData.normalmap_shader, &sData.cube);
	}

	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].Draw(sData.normalmap_shader, sData.backpack);
		float time = glfwGetTime();
		sData.geometry_shader->SetUniform1f("u_Time", time);
		// scene_graph->m_Backpacks[i].Draw(sData.geometry_shader, sData.backpack);				// for displaying normal etc.
	}

	for (int i = 0; i < scene_graph->m_Points.size(); i++)
	{
		scene_graph->m_Points[i].Draw(sData.normalmap_shader, &sData.point);
	}

	for (int i = 0; i < scene_graph->m_Quads.size(); i++)
	{
		scene_graph->m_Quads[i].Draw(sData.parallax_shader, &sData.quad);
	}

	// no instanced rendering here for now

	// draw sky box after drawing all models (as optimization)
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	// cube_map.Draw(sData.primitive_cube, sData.cubemap_shader);

	// enable depth buffer
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// draw grid (after sky box to benefit from blending with skybox colors)
	glBindTexture(GL_TEXTURE_2D, cube_map.cubemapID);

	for (int i = 0; i < scene_graph->m_Grids.size(); i++)
	{
		scene_graph->m_Grids[i].Draw(sData.grid_xz, sData.shader);
	}
}

void Renderer::RenderToDepthMap(SceneGraph* scene_graph)
{
	// 1. bind framebuffer (shadow map)
	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[0]);
	glViewport(0, 0, 1024, 1024);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_DEPTH_BUFFER_BIT);

	// 2. setup matrices from light source position
	// uboLightSpaceMatrix.SetMat4f(0, glm::perspectiveRH(glm::radians(90.0f), 1.0f, 1.0f, 25.0f));
	uboLightSpaceMatrix.SetMat4f(0, glm::orthoRH(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 100.0f));
	uboLightSpaceMatrix.SetMat4f(sizeof(glm::mat4), glm::lookAt(glm::vec3(-6.0f, 0.0f, -4.0f), glm::vec3(-5.0f, 0.0f, -4.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	// 3. render models to depth map 
	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		scene_graph->m_PointLights[i].Draw(sData.depth_shader, &sData.primitive_cube);
	}

	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		scene_graph->m_Cubes[i].Draw(sData.depth_shader, &sData.cube);
	}

	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].Draw(sData.depth_shader, sData.backpack);
	}

	for (int i = 0; i < scene_graph->m_Points.size(); i++)
	{
		scene_graph->m_Points[i].Draw(sData.depth_shader, &sData.point);
	}

	for (int i = 0; i < scene_graph->m_Quads.size(); i++)
	{
		scene_graph->m_Quads[i].Draw(sData.depth_shader, &sData.quad);
	}

	// instancing stuff left out for now... TODO: add later once the rest is working

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0.0f, 0.0f, sData.window->m_Width, sData.window->m_Height);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::RenderToDepthCubemap(SceneGraph* scene_graph)
{
	// settings
	float near_plane = 1.0f;
	float far_plane = 25.0f;


	// 1. bind framebuffer (shadow map)
	glBindFramebuffer(GL_FRAMEBUFFER, sData.fbo_id[1]);
	glViewport(0, 0, 1024, 1024);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_DEPTH_BUFFER_BIT);

	// 2. setup matrices from light source position

	glm::mat4 perspective = glm::perspectiveRH(glm::radians(90.0f), 1024.0f/1024.0f, near_plane, far_plane);
	std::vector<glm::mat4> light_view; 

	glm::vec3 lightPosition = glm::vec3(-6.0f, 0.0f, -4.0f);
	// TODO: check if up vectors are correctly setup
	light_view.push_back(perspective * glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));				// right
	light_view.push_back(perspective * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));				// left
	light_view.push_back(perspective * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));				// top
	light_view.push_back(perspective * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));				// bottom
	light_view.push_back(perspective * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));				// near
	light_view.push_back(perspective * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));				// far

	// send to shader/GPU
	for (int i = 0; i < light_view.size(); i++)
	{
		sData.depth_cubemap_shader->SetUniformMat4f("u_LightView[" + std::to_string(i) + "]", light_view[i]);
	}

	sData.depth_cubemap_shader->SetUniform3f("u_LightPosition", lightPosition);
	sData.depth_cubemap_shader->SetUniform1f("u_FarPlane", far_plane);

	// 3. render models to depth map 
	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		scene_graph->m_PointLights[i].Draw(sData.depth_cubemap_shader, &sData.primitive_cube);
	}

	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		scene_graph->m_Cubes[i].Draw(sData.depth_cubemap_shader, &sData.cube);
	}

	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].Draw(sData.depth_cubemap_shader, sData.backpack);
	}

	for (int i = 0; i < scene_graph->m_Points.size(); i++)
	{
		scene_graph->m_Points[i].Draw(sData.depth_cubemap_shader, &sData.point);
	}

	for (int i = 0; i < scene_graph->m_Quads.size(); i++)
	{
		scene_graph->m_Quads[i].Draw(sData.depth_cubemap_shader, &sData.quad);
	}

	// instancing stuff left out for now... TODO: add later once the rest is working
}


void Renderer::RenderSceneWithDepthMap(SceneGraph* scene_graph)
{
	// ===================================== start of frame setup ==================================
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0.0f, 0.0f, sData.window->m_Width, sData.window->m_Height);

	glDisable(GL_FRAMEBUFFER_SRGB);				// gamma correction disabled for setting clear color (so that background color stays the same)
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_FRAMEBUFFER_SRGB);				// gamma correction 

	// enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);			// if stencil and depth pass
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);								// enable writing to stencil buffer

	uboMatrix.SetMat4f(0, cameras[0]->ProjectionMatrix());
	uboMatrix.SetMat4f(sizeof(glm::mat4), cameras[0]->LookAtMatrix());
	uboCamera.SetVec3f(0, cameras[0]->Position());
	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sData.shadow_map_id);
	sData.shadow_map_shader->SetUniform1i("u_DepthMap", 10);

	sData.shadow_map_shader->SetUniform3f("u_LightPosition", glm::vec3(-6.0f, 0.0f, -4.0f));		// hardcoded not ideal TODO

	// point lights
	sData.model_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());
	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		scene_graph->m_PointLights[i].Draw(sData.shadow_map_shader, &sData.primitive_cube);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.model_shader, i);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.asteroids_instanced, i);
	}

	for (int i = 0; i < scene_graph->m_DirectionalLights.size(); i++)
	{
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.model_shader);
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.asteroids_instanced);
	}

	for (int i = 0; i < scene_graph->m_SpotLights.size(); i++)
	{
		// as flash light
		scene_graph->m_SpotLights[i].m_Direction = cameras[0]->LookDirection();
		scene_graph->m_SpotLights[i].m_Instance.position = cameras[0]->Position();

		scene_graph->m_SpotLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.model_shader);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.asteroids_instanced);
	}

	sData.model_shader->SetUniform1i("u_Skybox", 10);

	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		scene_graph->m_Cubes[i].Draw(sData.shadow_map_shader, &sData.cube);
	}

	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].Draw(sData.shadow_map_shader, sData.backpack);
		float time = glfwGetTime();
		sData.geometry_shader->SetUniform1f("u_Time", time);
		// scene_graph->m_Backpacks[i].Draw(sData.geometry_shader, sData.backpack);				// for displaying normal etc.
	}

	for (int i = 0; i < scene_graph->m_Points.size(); i++)
	{
		scene_graph->m_Points[i].Draw(sData.shadow_map_shader, &sData.point);
	}

	for (int i = 0; i < scene_graph->m_Quads.size(); i++)
	{
		scene_graph->m_Quads[i].Draw(sData.shadow_map_shader, &sData.quad);
	}

	// instanced rendering of asteroid etc.
	sData.instancing_shader->SetUniformMat4f("u_Projection", cameras[0]->ProjectionMatrix());
	sData.instancing_shader->SetUniformMat4f("u_View", cameras[0]->LookAtMatrix());

	for (int i = 0; i < scene_graph->m_Planets.size(); i++)
	{
		scene_graph->m_Planets[i].Draw(sData.model_shader, sData.planet);
	}

	if (number_asteroids > 0)
	{
		scene_graph->m_Rocks[0].DrawInstanced(sData.asteroids_instanced, sData.rock, sData.matrices, number_asteroids);
	}

	// draw sky box after drawing all models (as optimization)
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	// cube_map.Draw(sData.primitive_cube, sData.cubemap_shader);

	// enable depth buffer
	glDepthFunc(GL_LESS); 
	glEnable(GL_CULL_FACE);

	// draw grid (after sky box to benefit from blending with skybox colors)
	glBindTexture(GL_TEXTURE_2D, cube_map.cubemapID);

	for (int i = 0; i < scene_graph->m_Grids.size(); i++)
	{
		scene_graph->m_Grids[i].Draw(sData.grid_xz, sData.shader);
	}
}

void Renderer::RenderSceneWithDepthCubemap(SceneGraph* scene_graph) 
{
	// settings
	float near_plane = 1.0f;
	float far_plane = 25.0f;

	// ===================================== start of frame setup ==================================
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0.0f, 0.0f, sData.window->m_Width, sData.window->m_Height);

	glDisable(GL_FRAMEBUFFER_SRGB);				// gamma correction disabled for setting clear color (so that background color stays the same)
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_FRAMEBUFFER_SRGB);				// gamma correction 

	// enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);			// if stencil and depth pass
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);								// enable writing to stencil buffer

	uboMatrix.SetMat4f(0, cameras[0]->ProjectionMatrix());
	uboMatrix.SetMat4f(sizeof(glm::mat4), cameras[0]->LookAtMatrix());
	uboCamera.SetVec3f(0, cameras[0]->Position());

	// bind cubemap
	glActiveTexture(GL_TEXTURE0 + 11);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap.cubemapID);
	sData.shadow_cubemap_shader->SetUniform1i("u_DepthMap", 11);

	sData.shadow_cubemap_shader->SetUniform3f("u_LightPosition", glm::vec3(-6.0f, 0.0f, -4.0f));		// hardcoded not ideal TODO
	sData.shadow_cubemap_shader->SetUniform1f("u_FarPlane", far_plane);

	// point lights
	sData.model_shader->SetUniform1i("u_NumberPointLights", scene_graph->m_PointLights.size());
	for (int i = 0; i < scene_graph->m_PointLights.size(); i++)
	{
		scene_graph->m_PointLights[i].Draw(sData.shadow_cubemap_shader, &sData.primitive_cube);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.model_shader, i);
		scene_graph->m_PointLights[i].SetLightingUniforms(sData.asteroids_instanced, i);
	}

	for (int i = 0; i < scene_graph->m_DirectionalLights.size(); i++)
	{
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.model_shader);
		scene_graph->m_DirectionalLights[i].SetLightingUniforms(sData.asteroids_instanced);
	}

	for (int i = 0; i < scene_graph->m_SpotLights.size(); i++)
	{
		// as flash light
		scene_graph->m_SpotLights[i].m_Direction = cameras[0]->LookDirection();
		scene_graph->m_SpotLights[i].m_Instance.position = cameras[0]->Position();

		scene_graph->m_SpotLights[i].Draw(sData.light_source_shader, &sData.primitive_cube);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.model_shader);
		scene_graph->m_SpotLights[i].SetLightingUniforms(sData.asteroids_instanced);
	}

	sData.model_shader->SetUniform1i("u_Skybox", 10);

	for (int i = 0; i < scene_graph->m_Cubes.size(); i++)
	{
		scene_graph->m_Cubes[i].Draw(sData.shadow_cubemap_shader, &sData.cube);
	}

	for (int i = 0; i < scene_graph->m_Backpacks.size(); i++)
	{
		scene_graph->m_Backpacks[i].Draw(sData.shadow_cubemap_shader, sData.backpack);
		float time = glfwGetTime();
		sData.geometry_shader->SetUniform1f("u_Time", time);
		// scene_graph->m_Backpacks[i].Draw(sData.geometry_shader, sData.backpack);				// for displaying normal etc.
	}

	for (int i = 0; i < scene_graph->m_Points.size(); i++)
	{
		scene_graph->m_Points[i].Draw(sData.shadow_cubemap_shader, &sData.point);
	}

	for (int i = 0; i < scene_graph->m_Quads.size(); i++)
	{
		scene_graph->m_Quads[i].Draw(sData.shadow_cubemap_shader, &sData.quad);
	}

	// instanced rendering of asteroid etc.
	sData.instancing_shader->SetUniformMat4f("u_Projection", cameras[0]->ProjectionMatrix());
	sData.instancing_shader->SetUniformMat4f("u_View", cameras[0]->LookAtMatrix());

	for (int i = 0; i < scene_graph->m_Planets.size(); i++)
	{
		scene_graph->m_Planets[i].Draw(sData.model_shader, sData.planet);
	}

	if (number_asteroids > 0)
	{
		scene_graph->m_Rocks[0].DrawInstanced(sData.asteroids_instanced, sData.rock, sData.matrices, number_asteroids);
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