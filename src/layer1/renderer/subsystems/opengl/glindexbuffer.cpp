#include "glindexbuffer.h"

namespace Renderer {

	bool GLIndexBuffer::SetData(unsigned *indices, unsigned count)
	{
		buffer = new GLuint *;
		glGenBuffers(1, (GLuint *)buffer);

		if (!glIsBuffer(*(GLuint *)buffer)) {
			delete (GLuint *)buffer;
			buffer = nullptr;
			return false;
		}

		glNamedBufferData(*(GLuint *)buffer, sizeof(unsigned) * count, indices,  GL_STATIC_DRAW);

		this->count = count;

		return true;
	}

	void GLIndexBuffer::Release()
	{
		if (!buffer)
			return;
		
		if (glIsBuffer(*(GLuint *)buffer))
			glDeleteBuffers(1, (GLuint *)buffer);
	
		delete (GLuint *)buffer;
		buffer = nullptr;
		
		this->count = 0;
	}

}
