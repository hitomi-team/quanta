#ifndef RENDERER_VERTEXBUFFER_H
#define RENDERER_VERTEXBUFFER_H

namespace Renderer {

	struct Vertex {
		Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex) { this->pos = pos; this->norm = norm; this->tex = tex; }
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 tex;
	};

	class VertexBuffer {
	public:
		inline VertexBuffer() {}
		inline virtual ~VertexBuffer() {}

		virtual bool SetData(const Vertex *vertices, unsigned count) = 0;
		virtual void Release() = 0;

		inline void *GetBuffer() { return buffer; }
		inline unsigned GetCount() { return count; }

	protected:
		void *buffer;
		unsigned count;
	};

}

#endif
