#ifndef GL_VERTEXBUFFER_H
#define GL_VERTEXBUFFER_H

#include "glrenderer.h"

namespace Renderer {

	class GLVertexBuffer : public VertexBuffer {
	public:
		GLVertexBuffer() {}

		bool SetData(Vertex *vertices, unsigned count);
		void Release();
	};

}

#endif
