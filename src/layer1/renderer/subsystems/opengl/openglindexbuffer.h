#ifndef LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLINDEXBUFFER_H
#define LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLINDEXBUFFER_H

#include "openglrequired.h"

class COpenGLIndexBuffer : public Renderer::IndexBuffer {
public:
	inline COpenGLIndexBuffer() {}

	bool SetData(const uint16_t *vertices, unsigned count);
	void Release();

};

#endif
