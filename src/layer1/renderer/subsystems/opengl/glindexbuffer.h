#ifndef GL_INDEXBUFFER_H
#define GL_INDEXBUFFER_H

#include "glrenderer.h"

namespace Renderer {

	class GLIndexBuffer : public IndexBuffer {
	public:
		GLIndexBuffer() {}

		bool SetData(unsigned *indices, unsigned count);
		void Release();
	};

}

#endif
