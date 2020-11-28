#ifndef LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLTEXTURE2D_H
#define LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLTEXTURE2D_H

#include "openglrequired.h"

class COpenGLTexture2D : public Renderer::Texture2D {
public:
	inline COpenGLTexture2D() {}

	bool SetData(unsigned char *data, unsigned width, unsigned height, Renderer::SamplerStateDesc samplerstatedesc);
	void Release();

protected:
	GLuint bo;

};

#endif
