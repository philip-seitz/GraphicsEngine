#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

float pi = 3.14159f;

Camera::Camera():
	m_position(0.0f, 0.0f, 5.0f),
	m_target(0.0f, 0.0f, 0.0f),
	m_up(0.0f, 1.0f, 0.0f),
	m_look_at(glm::lookAtRH(m_position, m_target, m_up)),
	m_look_direction(0.0f, 0.0f, -1.0f),
	m_camera_angle_yaw(0.0f),
	m_camera_angle_pitch(0.0f),
	m_movement_speed(8.0f),
	// settings
	m_proj_3D(true),
	// values for the perspective transform
	m_yFOV(45.0f),
	m_zoom(1.0f),
	m_aspectRatio(1080.0f/720.0f),
	m_zNear(1.0f),
	m_zFar(-10000.0f),
	m_perspective_proj(glm::perspectiveRH(m_yFOV * pi / 180.0f, m_aspectRatio, m_zNear, m_zFar)),
	m_orthographic_proj(glm::orthoRH(0.0f, 300.0f , 0.0f, 300.0f , m_zNear, m_zFar))
{
}

Camera::Camera(const bool& isPerspective):
	m_position(0.0f, 0.0f, 8.0f),
	m_target(0.0f, 0.0f, 0.0f),
	m_up(0.0f, 1.0f, 0.0f),
	m_look_at(glm::lookAtRH(m_position, m_target, m_up)),
	m_look_direction(0.0f, 0.0f, -1.0f),
	m_camera_angle_yaw(0.0f),
	m_camera_angle_pitch(0.0f),
	m_movement_speed(8.0f),
	// settings
	m_proj_3D(isPerspective),
	// values for the perspective transform
	m_yFOV(45.0f),
	m_zoom(1.0f),
	m_aspectRatio(1080.0f / 720.0f),
	m_zNear(1.5f),
	m_zFar(-10000.0f),
	m_perspective_proj(glm::perspectiveRH(m_yFOV * pi / 180.0f, m_aspectRatio, m_zNear, m_zFar)),
	m_orthographic_proj(glm::orthoRH(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 100.0f))
{
}

glm::vec3 Camera::Position()
{
	return m_position;
}

glm::vec3 Camera::LookDirection()
{
	return m_look_direction;
}

glm::mat4x4 Camera::LookAtMatrix()
{
	return m_look_at;
}

glm::mat4x4 Camera::ProjectionMatrix()
{
	// return either perspectice (3D) or orthographic (2D) projection matrix
	if (m_proj_3D)
	{
		return m_perspective_proj;
	}
	else
	{
		return m_orthographic_proj;
	}
}

const void Camera::ManageInputs(Window& window, const float& dt)
{
	UpdatePosition(window, dt);
	Zoom(window, dt);
}

// update position
const void Camera::UpdatePosition(Window& window, const float& dt)
{
	static float time = 0.0f;
	float z_limit = m_zFar - int(0.1 * m_zFar);		// avoid moving camera outside behind zFar (mirrored image... not good)
	if (m_position.z > z_limit)
	{
		if (glfwGetKey(window.GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		{
			m_position += m_look_direction * dt * m_movement_speed;
		}

		if (glfwGetKey(window.GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
			m_position -= m_look_direction * dt * m_movement_speed;

		if (glfwGetKey(window.GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
			m_position += glm::normalize(glm::cross(m_look_direction, glm::vec3(0.0f, 1.0f, 0.0f))) * dt * m_movement_speed;

		if (glfwGetKey(window.GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
			m_position -= glm::normalize(glm::cross(m_look_direction, glm::vec3(0.0f, 1.0f, 0.0f))) * dt * m_movement_speed;

		if (glfwGetKey(window.GetWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			m_camera_angle_yaw += dt * m_movement_speed*10.0f;
			m_camera_angle_yaw = fmod(m_camera_angle_yaw, 360.0f);
		}

		if (glfwGetKey(window.GetWindow(), GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			m_camera_angle_yaw -= dt * m_movement_speed*10.0f;
			m_camera_angle_yaw = fmod(m_camera_angle_yaw, 360.0f);
		}


		if (glfwGetKey(window.GetWindow(), GLFW_KEY_UP) == GLFW_PRESS)
		{
			m_camera_angle_pitch += dt * m_movement_speed*5.0f;
			if (m_camera_angle_pitch >= 88.0f)
				m_camera_angle_pitch = 88.0f;
			// m_camera_angle_pitch = fmod(m_camera_angle_pitch, 360.0f);
			
		}

		if (glfwGetKey(window.GetWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			m_camera_angle_pitch -= dt * m_movement_speed*5.0f;
			// m_camera_angle_pitch = fmod(m_camera_angle_pitch, 360.0f);
			if (m_camera_angle_pitch <= -88.0f)
				m_camera_angle_pitch = -88.0f;
		}

		// first yaw rotation, then pitch of the direction vector (relevant for z-component)
			m_look_direction.x = glm::sin(m_camera_angle_yaw * pi / 180.0f) * glm::cos(m_camera_angle_pitch * pi / 180.0f);
			m_look_direction.y = glm::sin(m_camera_angle_pitch * pi / 180.0f);
			m_look_direction.z = -glm::cos(m_camera_angle_yaw * pi / 180.0f) * glm::cos(m_camera_angle_pitch * pi / 180.0f);

		// without this updating target the camera will be locked on to the initial target 
		m_target = m_position + m_look_direction;
	}

	else {
		if (glm::vec3(m_look_direction * dt * m_movement_speed * 10.0f + m_position).z < z_limit )
			m_position -= m_look_direction * dt * m_movement_speed * 10.0f;
		else
			m_position += m_look_direction * dt * m_movement_speed * 10.0f;
	}

	// update matrix
	m_look_at = glm::lookAtRH(m_position, m_target, m_up);

	// additional outpuots for debugging 
	if (time >= 2.0f)
	{
		std::cout << "x: " << m_position.x << ", y: " << m_position.y << ", z: " << m_position.z << ", pitch: "<< m_camera_angle_pitch << ", yaw: " << m_camera_angle_yaw<< ", dir: {"<< m_look_direction.x << ", " << m_look_direction.y << ", " << m_look_direction.z << "}" << std::endl;
		time = 0.0f;
	}
	else
		time += dt;
}

const void Camera::Zoom(Window& window, const float& dt)
{
	static float time_z = 0.0f;
	if (glfwGetKey(window.GetWindow(), GLFW_KEY_P) == GLFW_PRESS)
	{
		if (m_yFOV / (m_zoom + dt * m_movement_speed * 0.5f) <= 5.0f)
		{
			m_zoom -= dt * m_movement_speed * 5.0f;
		}
		else
			m_zoom += dt * m_movement_speed * 0.5f;
	}

	if (glfwGetKey(window.GetWindow(), GLFW_KEY_M) == GLFW_PRESS)
	{
		if (m_yFOV / (m_zoom - dt * m_movement_speed * 0.5f) >= 90.0f)
		{
			m_zoom += dt * m_movement_speed * 0.05f;
		}
		else
			m_zoom -= dt * m_movement_speed * 0.5f;
	}

	// update the corresponding projection matrix
	if (m_proj_3D)
		m_perspective_proj = glm::perspectiveRH((m_yFOV / m_zoom) * pi / 180.0f, m_aspectRatio, m_zNear, m_zFar);
	else
	{
		// orthographic (TODO)
	}

	// additional outpuots for debugging 
	if (time_z >= 2.0f)
	{
		std::cout << "zoom: " << m_zoom << ", yFOV: " << (m_yFOV / m_zoom) << std::endl;
		std::cout << "FPS: " << 1 / dt << std::endl;
		time_z = 0.0f;
	}
	else
		time_z += dt;

}
