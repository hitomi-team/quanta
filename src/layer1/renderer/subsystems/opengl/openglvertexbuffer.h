#ifndef LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLVERTEXBUFFER_H
#define LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLVERTEXBUFFER_H

#include "openglrequired.h"

class COpenGLVertexBuffer : public Renderer::VertexBuffer {
public:
	inline COpenGLVertexBuffer() {}

	bool SetData(const Renderer::Vertex *vertices, unsigned count);
	void Release();

};

#endif
