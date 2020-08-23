#ifndef RENDERER_CAMERA_H
#define RENDERER_CAMERA_H

#include "layer1/graph/node.h"
#include "rendertarget.h"
#include "transform.h"

namespace Renderer {

	class Camera : public Transform, public Graph::Node {
	public:
		inline Camera() : Node("Camera") { rt = nullptr; }

		void Setup(float fov, float aspect, float zNear, float zFar);
		void Update(float fov, float aspect, float zNear, float zFar);

		void setRenderTarget(RenderTarget *rt) { this->rt = rt; }
		inline RenderTarget *getRenderTarget() { return rt; }

		inline glm::mat4 getPerspective() { return projection * glm::lookAt(pos, pos + forward, up); } // camera perspective matrix == projection * view

	protected:
		RenderTarget *rt;

		glm::mat4 projection;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;
	};

}

#endif
