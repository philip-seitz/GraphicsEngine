#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera():
	m_position(0.0f, 0.0f, 19.0f),
	m_target(0.0f, 0.0f, 0.0f),
	m_up(0.0f, 1.0f, 0.0f),
	m_look_at(glm::lookAt(m_position, m_target, m_up)),
	m_look_direction(0.0f, 0.0f, -1.0f),
	m_camera_angle_yaw(0.0f),
	//m_camera_angle_roll(0.0f),
	m_camera_angle_pitch(0.0f),
	m_movement_speed(8.0f)
{}

glm::vec3 Camera::Position()
{
	return m_position;
}

glm::mat4x4 Camera::LookAtMatrix()
{
	return m_look_at;
}

// update position
void Camera::UpdatePosition(Window& window, float dt, const float z_far)
{
	static float time = 0.0f;
	float z_limit = z_far - int(0.1 * z_far);		// avoid moving camera outside behind zFar (mirrored image... not good)
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

		// for camera roll. makes combined rotation much more difficult though
		/*if (glfwGetKey(window.GetWindow(), GLFW_KEY_E) == GLFW_PRESS)
		{
			m_camera_angle_roll += dt * m_movement_speed *10.0f;
			m_camera_angle_roll = fmod(m_camera_angle_roll, 360.0f);
			m_up.x = glm::sin(m_camera_angle_roll *3.14f/180.0f);
			m_up.y = glm::cos(m_camera_angle_roll * 3.14f / 180.0f);
		}
		
		same for other direction...
		*/

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
			m_camera_angle_pitch = fmod(m_camera_angle_pitch, 360.0f);
			
		}

		if (glfwGetKey(window.GetWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			m_camera_angle_pitch -= dt * m_movement_speed*5.0f;
			m_camera_angle_pitch = fmod(m_camera_angle_pitch, 360.0f);
		}

		// first yaw rotation, then pitch of the direction vector
			m_look_direction.x = glm::sin(m_camera_angle_yaw * 3.14f / 180.0f);;
			m_look_direction.y = glm::sin(m_camera_angle_pitch * 3.14f / 180.0f);
			m_look_direction.z = -glm::cos(m_camera_angle_yaw * 3.14f / 180.0f);

		// for also implementing roll this would have to be more complicated. (FPS games mostly no roll)

		// without this the updating target the camera will be locked on to the initial target 
		m_target = m_position + m_look_direction;
	}

	else {
		if (glm::vec3(m_look_direction * dt * m_movement_speed * 10.0f + m_position).z < z_limit )
			m_position -= m_look_direction * dt * m_movement_speed * 10.0f;
		else
			m_position += m_look_direction * dt * m_movement_speed * 10.0f;
	}

	m_look_at = glm::lookAt(m_position, m_target, m_up);

	if (time >= 2.0f)
	{
		std::cout << "x: " << m_position.x << ", y: " << m_position.y << ", z: " << m_position.z << ", pitch: "<< m_camera_angle_pitch << ", yaw: " << m_camera_angle_yaw<< ", dir: {"<< m_look_direction.x << ", " << m_look_direction.y << ", " << m_look_direction.z << "}" << std::endl;
		time = 0.0f;
	}
	else
		time += dt;
}
