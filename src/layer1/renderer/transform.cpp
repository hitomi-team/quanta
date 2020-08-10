#include "pch/pch.h"

#include "transform.h"

namespace Renderer {

	void Transform::Setup(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, float angle)
	{
		setPos(pos);
		setRot(rot, angle);
		setScale(scale);
	}

	glm::mat4 Transform::getModel()
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, pos);
		model = glm::rotate(model, glm::radians(angle), rot);
		model = glm::scale(model, scale);

		return model;
	}

}
