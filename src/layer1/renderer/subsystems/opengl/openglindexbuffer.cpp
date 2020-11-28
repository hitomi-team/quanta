#include "pch/pch.h"

#include "openglindexbuffer.h"

bool COpenGLIndexBuffer::SetData(const uint16_t *vertex, unsigned count)
{
	this->buffer = reinterpret_cast< void * >(new GLuint);

	glCreateBuffers(1, reinterpret_cast< GLuint * >(this->buffer));
	glNamedBufferStorage(reinterpret_cast< GLuint * >(this->buffer)[0], sizeof(uint16_t)*count, vertex, 0);

	this->count = count;
	return true;
}

void COpenGLIndexBuffer::Release()
{
	glDeleteBuffers(1, reinterpret_cast< GLuint * >(this->buffer));
	delete reinterpret_cast< GLuint * >(this->buffer);
}
