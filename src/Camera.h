#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "glm/glm.hpp"


class Camera
{
public:
	glm::mat4 GetViewMatrix() const;
	virtual void Rotate(float yaw, float pitch) {}

protected:
	Camera();

	glm::vec3 m_position;
	glm::vec3 m_targetPos;
	glm::vec3 m_up;

	//Euler angles (radians)
	float m_yaw;
	float m_pitch;
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