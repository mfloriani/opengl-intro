#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"


class Camera
{
public:
	glm::mat4 GetViewMatrix() const;

	virtual void SetPosition(const glm::vec3 &position) {}
	virtual void Rotate(float yaw, float pitch) {}
	virtual void Move(const glm::vec3 &offsetPos) {}

	const glm::vec3& GetLook() const;
	const glm::vec3& GetRight() const;
	const glm::vec3& GetUp() const;
	const glm::vec3& GetPosition() const;

	float GetFOV() const { return m_fov; }
	void SetFOV(float fov) { m_fov = fov; }

protected:
	Camera();

	glm::vec3 m_position;
	glm::vec3 m_targetPos;
	glm::vec3 m_up;
	glm::vec3 m_look;
	glm::vec3 m_right;
	const glm::vec3 WORLD_UP;

	//Euler angles (radians)
	float m_yaw;
	float m_pitch;

	float m_fov;
};

class FPSCamera : public Camera
{
public:
	FPSCamera(glm::vec3 position = glm::vec3(0.f, 0.f, 0.f), float yaw = glm::pi<float>(), float pitch = 0.f);

	virtual void SetPosition(const glm::vec3& position);
	virtual void Rotate(float yaw, float pitch); //degrees
	virtual void Move(const glm::vec3& offsetPos);

private:
	void UpdateCameraVectors();
};

class OrbitCamera : public Camera
{
public:
	OrbitCamera();

	virtual void Rotate(float yaw, float pitch); //degrees

	void SetLookAt(const glm::vec3 &target);
	void SetRadius(float radius);

private:
	void UpdateCameraVectors();
	float m_radius;
};

#endif // !__CAMERA_H__