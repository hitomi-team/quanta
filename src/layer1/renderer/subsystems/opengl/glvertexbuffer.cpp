#include "glvertexbuffer.h"

namespace Renderer {

	bool GLVertexBuffer::SetData(Vertex *vertices, unsigned count)
	{
		buffer = new GLuint *;
		glGenBuffers(1, (GLuint *)buffer);

		if (!glIsBuffer(*(GLuint *)buffer)) {
			delete (GLuint *)buffer;
			buffer = nullptr;
			return false;
		}

		glNamedBufferData(*(GLuint *)buffer, sizeof(Vertex) * count, vertices,  GL_STATIC_DRAW);

		return true;
	}

	void GLVertexBuffer::Release()
	{
		if (!buffer)
			return;
		
		if (glIsBuffer(*(GLuint *)buffer))
			glDeleteBuffers(1, (GLuint *)buffer);
	
		delete (GLuint *)buffer;
		buffer = nullptr;
	}

}
