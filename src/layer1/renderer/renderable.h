#ifndef RENDERER_RENDERABLE_H
#define RENDERER_RENDERABLE_H

#include "../graph/node.h"
#include "transform.h"

namespace Renderer {
	
	enum RenderableType {
		REND_2D = 0,
		REND_3D,
		REND_TEXT
	};

	class Renderable : public Graph::Node, public Transform {
	public:
		Renderable() : Node("Renderable") {}

		inline RenderableType getType() { return type; }
		inline void setType(RenderableType type) { this->type = type; }

		unsigned MatIdx;  // index into registered materials
		unsigned MeshIdx; // index into registered meshes

	protected:
		RenderableType type;
	};

}

#endif
