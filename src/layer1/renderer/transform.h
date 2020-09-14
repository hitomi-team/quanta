#ifndef RENDERER_TRANSFORM_H
#define RENDERER_TRANSFORM_H

namespace Renderer {

	// The angle is in degrees
	class Transform {
	public:
		inline Transform() {}
		inline Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, float angle) { Setup(pos, rot, scale, angle); }

		void Setup(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, float angle);

		inline void setPos(glm::vec3 pos) { this->pos = pos; }
		inline void setRot(glm::vec3 rot, float angle) { this->rot = rot; this->angle = angle; }
		inline void setScale(glm::vec3 scale) { this->scale = scale; }

		inline float getAngle() { return angle; }
		inline glm::vec3 getPos() { return pos; }
		inline glm::vec3 getRot() { return rot; }
		inline glm::vec3 getScale() { return scale; }

		glm::mat4 getModel();

	protected:
		float angle;
		glm::vec3 pos, rot, scale;
	};
}

#endif
