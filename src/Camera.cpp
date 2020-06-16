#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

const float DEF_FOV = 45.f;

Camera::Camera() : 
	m_position(glm::vec3{0.f, 0.f, 0.f}),
	m_targetPos(glm::vec3{ 0.f, 0.f, 0.f }),
	m_up(glm::vec3{ 0.f, 1.f, 0.f }),
	WORLD_UP(0.f, 1.f, 0.f),
	m_yaw(0.f),
	m_pitch(0.f),
	m_fov(DEF_FOV)
{

}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(m_position, m_targetPos, m_up);
}

const glm::vec3& Camera::GetLook() const
{
	return m_look;
}

const glm::vec3& Camera::GetRight() const
{
	return m_right;
}

const glm::vec3& Camera::GetUp() const
{
	return m_up;
}

const glm::vec3& Camera::GetPosition() const
{
	return m_position;
}

FPSCamera::FPSCamera(glm::vec3 position, float yaw, float pitch)	
{
	m_position = position;
	m_yaw = yaw;
	m_pitch = pitch;
}

void FPSCamera::SetPosition(const glm::vec3& position)
{
	m_position = position;
}

void FPSCamera::Move(const glm::vec3& offsetPos)
{
	m_position += offsetPos;
	UpdateCameraVectors();
}

void FPSCamera::Rotate(float yaw, float pitch)
{
	m_yaw += glm::radians(yaw);
	m_pitch += glm::radians(pitch);

	m_pitch = glm::clamp(m_pitch, -glm::pi<float>() / 2.f + 0.1f, glm::pi<float>() / 2.f - 0.1f);
	UpdateCameraVectors();
}

void FPSCamera::UpdateCameraVectors()
{
	//Spherical to Cartesian Coordinates

	glm::vec3 look;
	look.x = cosf(m_pitch) * sinf(m_yaw);
	look.y = sinf(m_pitch);
	look.z = cosf(m_pitch) * cosf(m_yaw);

	m_look = glm::normalize(look);

	m_right = glm::normalize(glm::cross(m_look, WORLD_UP));
	m_up = glm::normalize(glm::cross(m_right, m_look));

	m_targetPos = m_position + m_look;
}




OrbitCamera::OrbitCamera() :
	m_radius(10.f)
{

}

void OrbitCamera::SetLookAt(const glm::vec3 &target)
{
	m_targetPos = target;
}

void OrbitCamera::SetRadius(float radius)
{
	m_radius = glm::clamp(radius, 2.f, 80.f);
}

void OrbitCamera::Rotate(float yaw, float pitch)
{
	m_yaw = glm::radians(yaw);
	m_pitch = glm::radians(pitch);

	m_pitch = glm::clamp(m_pitch, -glm::pi<float>() / 2.f + 0.1f, glm::pi<float>() / 2.f - 0.1f);
	UpdateCameraVectors();
}

void OrbitCamera::UpdateCameraVectors()
{
	//Spherical to Cartesian Coordinates
	m_position.x = m_targetPos.x + m_radius * cosf(m_pitch) * sinf(m_yaw);
	m_position.y = m_targetPos.y + m_radius * sinf(m_pitch);
	m_position.z = m_targetPos.z + m_radius * cosf(m_pitch) * cosf(m_yaw);
}