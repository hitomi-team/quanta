#ifndef LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLINPUTLAYOUT_H
#define LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLINPUTLAYOUT_H

#include "openglrequired.h"

class COpenGLInputLayout : public Renderer::InputLayout {
public:
	inline COpenGLInputLayout() {}

	// parameters ignored here; but need to associate buffers to input layout
	bool Setup(unsigned char *vs_bytecode, unsigned int vs_size);
	void Release();

};

#endif
