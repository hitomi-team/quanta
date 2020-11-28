#include "pch/pch.h"

#include "openglparameterbuffer.h"
#include "openglrenderer.h"

bool COpenGLParameterBuffer::Setup(std::vector< Renderer::ShaderParameterElement > velements)
{
	this->elements = velements;
	this->mem_size = 0;

	for (auto &i : this->elements) {
		if (i.dataSize < 16)
			i.dataSize = 16;

		this->mem_size += i.dataSize;
	}

	glCreateBuffers(1, &this->bo);
	glNamedBufferStorage(this->bo, this->mem_size, nullptr, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT);

	this->Map();
	this->Flush();
	this->Unmap();

	return true;
}

void COpenGLParameterBuffer::Release()
{
	this->Unmap();
	glDeleteBuffers(1, &this->bo);
}

void COpenGLParameterBuffer::Apply()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->bo);
}

void COpenGLParameterBuffer::Map()
{
	this->map_mem = reinterpret_cast< char * >(glMapNamedBuffer(this->bo, GL_WRITE_ONLY));
}

void COpenGLParameterBuffer::Flush()
{
	size_t stride = 0;

	for (auto &i : this->elements) {
		std::memcpy(this->map_mem + stride, i.data, i.dataSize);
		stride += i.dataSize;
	}
}

void COpenGLParameterBuffer::Unmap()
{
	if (this->map_mem != nullptr) {
		glUnmapNamedBuffer(this->bo);
		this->map_mem = nullptr;
	}
}

void COpenGLParameterBuffer::SetShaderParameter(Renderer::ShaderParameterUsage param, float value)
{
	size_t stride = 0;
	for (auto &i : this->elements) {
		if (i.usage == param) {
			*(float *)i.data = value;
			std::memcpy(this->map_mem + stride, &value, i.dataSize);
		}
		stride += i.dataSize;
	}
}

void COpenGLParameterBuffer::SetShaderParameter(Renderer::ShaderParameterUsage param, const glm::mat4 &mat)
{
	size_t stride = 0;
	for (auto &i : this->elements) {
		if (i.usage == param) {
			*(glm::mat4 *)i.data = mat;
			std::memcpy(this->map_mem + stride, &mat[0], i.dataSize);
		}
		stride += i.dataSize;
	}
}
