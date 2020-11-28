#ifndef LAYER1_RENDERER_SUBSYSTEM_OPENGL_RENDERTARGET_H
#define LAYER1_RENDERER_SUBSYSTEM_OPENGL_RENDERTARGET_H

#include "openglrequired.h"

class COpenGLRenderTarget : public Renderer::RenderTarget {
public:
	inline COpenGLRenderTarget() {}

	bool Setup(unsigned width, unsigned height, Renderer::TextureUsage usage);
	void Release();

protected:
	GLuint bo;
	GLuint texid;

};

#endif
