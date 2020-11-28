#include "pch/pch.h"

#include "openglvertexbuffer.h"

bool COpenGLVertexBuffer::SetData(const Renderer::Vertex *vertex, unsigned count)
{
	this->buffer = reinterpret_cast< void * >(new GLuint);

	glCreateBuffers(1, reinterpret_cast< GLuint * >(this->buffer));
	glNamedBufferStorage(reinterpret_cast< GLuint * >(this->buffer)[0], sizeof(Renderer::Vertex)*count, vertex, 0);

	this->count = count;
	return true;
}

void COpenGLVertexBuffer::Release()
{
	glDeleteBuffers(1, reinterpret_cast< GLuint * >(this->buffer));
	delete reinterpret_cast< GLuint * >(this->buffer);
}
