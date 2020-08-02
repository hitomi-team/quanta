#ifndef RENDERER_VERTEXBUFFER_H
#define RENDERER_VERTEXBUFFER_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Renderer {

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 tex;
	};

	class VertexBuffer {
	public:
		VertexBuffer() {}
		virtual ~VertexBuffer() {}
	
		virtual bool SetData(Vertex *vertices, unsigned count) { (void)vertices; this->count = count; return true; }
		virtual void Release() {}

		inline unsigned GetCount() { return count; }

	protected:
		void *buffer;
		unsigned count;
	};

}

#endif
