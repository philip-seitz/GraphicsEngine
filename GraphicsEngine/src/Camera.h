#pragma once
#include <glm/glm.hpp>
#include "Util.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"

class Camera
{

public:

	Camera();
	glm::vec3 Position();
	glm::mat4x4 LookAtMatrix();
	void UpdatePosition(Window& window, float dt, const float z_far);

private:

	// look at matrix
	glm::vec3 m_position;
	glm::vec3 m_target;
	glm::vec3 m_up;
	glm::mat4x4 m_look_at;

	// variables for movement
	glm::vec3 m_look_direction;
	float m_movement_speed;
	// float m_camera_angle_roll;
	float m_camera_angle_yaw;
	float m_camera_angle_pitch;
};