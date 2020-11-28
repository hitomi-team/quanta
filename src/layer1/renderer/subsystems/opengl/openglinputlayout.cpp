#include "pch/pch.h"

#include "openglinputlayout.h"

bool COpenGLInputLayout::Setup(unsigned char *vs_bytecode, unsigned vs_size)
{
	GLuint *pvao;

	(void)vs_bytecode; (void)vs_size;

	pvao = new GLuint;
	glCreateVertexArrays(1, pvao);

	glEnableVertexArrayAttrib(pvao[0], 0);
	glVertexArrayAttribFormat(pvao[0], 0, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(pvao[0], 1);
	glVertexArrayAttribFormat(pvao[0], 1, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(pvao[0], 2);
	glVertexArrayAttribFormat(pvao[0], 2, 2, GL_FLOAT, GL_FALSE, 0);

	this->inputlayout = reinterpret_cast< void * >(pvao);

	return true;
}

void COpenGLInputLayout::Release()
{
	glDeleteVertexArrays(1, reinterpret_cast< GLuint * >(this->inputlayout));
	delete reinterpret_cast< GLuint * >(this->inputlayout);
}
