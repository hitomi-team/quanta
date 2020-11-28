#ifndef LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLPARAMETERBUFFER_H
#define LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLPARAMETERBUFFER_H

#include "openglrequired.h"

class COpenGLParameterBuffer : public Renderer::ShaderParameterBuffer {
public:
	inline COpenGLParameterBuffer()
	{
		this->bo = static_cast< GLuint >(-1);
		this->map_mem = nullptr;
	}

	bool Setup(std::vector< Renderer::ShaderParameterElement > elements);
	void Release();

	void Apply();
	void Map();
	void Flush();
	void Unmap();

	void SetShaderParameter(Renderer::ShaderParameterUsage param, float value);
	void SetShaderParameter(Renderer::ShaderParameterUsage param, const glm::mat4 &mat);

protected:
	GLuint bo;
	char *map_mem;
	unsigned mem_size;
};

#endif
