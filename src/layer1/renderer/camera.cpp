#include "pch/pch.h"

#include "camera.h"

namespace Renderer {

	void Camera::Setup(float fov, float aspect, float zNear, float zFar)
	{
		forward = glm::vec3(0, 0, -1);
		up = glm::vec3(0, 1, 0);

		Update(fov, aspect, zNear, zFar);
	}

	void Camera::Update(float fov, float aspect, float zNear, float zFar)
	{
		projection = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
	}

}
