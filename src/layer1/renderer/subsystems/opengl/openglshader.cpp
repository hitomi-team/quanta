#include "pch/pch.h"

#include "openglshader.h"
#include "openglinputlayout.h"

bool COpenGLShader::Build(unsigned char *vs_bytecode, unsigned int vs_size, unsigned char *fs_bytecode, unsigned int fs_size)
{
	GLuint vs, fs, pid;
	GLint i;

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderBinary(1, &vs, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, vs_bytecode, vs_size);
	glSpecializeShader(vs, "main", 0, 0, 0);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &i);
	if (i == GL_FALSE) {
		glDeleteShader(vs);
		return false;
	}

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderBinary(1, &fs, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, fs_bytecode, fs_size);
	glSpecializeShader(fs, "main", 0, 0, 0);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &i);
	if (i == GL_FALSE) {
		glDeleteShader(vs);
		glDeleteShader(fs);
		return false;
	}

	pid = glCreateProgram();
	glAttachShader(pid, vs);
	glAttachShader(pid, fs);
	glLinkProgram(pid);

	glDetachShader(pid, vs);
	glDetachShader(pid, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	this->program = reinterpret_cast< void * >(new GLuint);
	reinterpret_cast< GLuint * >(this->program)[0] = pid;

	std::memset(this->objects, 0, sizeof(this->objects));

	this->inputlayout = new COpenGLInputLayout;
	this->inputlayout->Setup(vs_bytecode, vs_size);

	return true;
}

void COpenGLShader::Release()
{
	glDeleteProgram(reinterpret_cast< GLuint * >(this->program)[0]);
	delete reinterpret_cast< GLuint * >(this->program);

	if (this->inputlayout != nullptr) {
		this->inputlayout->Release();
		delete this->inputlayout;
		this->inputlayout = nullptr;
	}
}

void *COpenGLShader::GetProgram()
{
	return this->program;
}
