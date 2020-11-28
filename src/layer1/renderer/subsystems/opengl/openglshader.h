#ifndef LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLSHADER_H
#define LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLSHADER_H

#include "openglrequired.h"

class COpenGLShader : public Renderer::Shader {
public:
	inline COpenGLShader() {}

	bool Build(unsigned char *vs_bytecode, unsigned int vs_size, unsigned char *fs_bytecode, unsigned int fs_size);
	void Release();

	void *GetProgram();

	// do not use for OpenGL
	inline void *GetShaderObject(Renderer::ShaderType type) { (void)type; return nullptr; }
};

#endif
