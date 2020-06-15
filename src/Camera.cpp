#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera() : 
	m_position(glm::vec3{0.f, 0.f, 0.f}),
	m_targetPos(glm::vec3{ 0.f, 0.f, 0.f }),
	m_up(glm::vec3{ 0.f, 1.f, 0.f }),
	m_yaw(0.f),
	m_pitch(0.f)
{

}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(m_position, m_targetPos, m_up);
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