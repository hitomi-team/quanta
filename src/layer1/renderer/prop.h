#ifndef RENDERER_PROP_H
#define RENDERER_PROP_H

#include "../graph/node.h"
#include "mesh.h"
#include "material.h"
#include "transform.h"

namespace Renderer {

	class Prop : public Transform, public Graph::Node {
	public:
		Prop() : Node("Prop") { mesh = nullptr; material = nullptr; }

		void Setup(Mesh *mesh, Material *material);
		void Release();

		void Destroy() { Release(); } // from Graph::Node

		void Draw(RHI *rhi); // Set position and other stuff to shader

		inline Mesh *getMesh() { return mesh; }
		inline Material *getMaterial() { return material; }

	protected:
		Mesh *mesh;
		Material *material;
	};

}

#endif
