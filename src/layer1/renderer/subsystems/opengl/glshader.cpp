#include "glshader.h"

namespace Renderer {

	bool GLShader::CompileShader(unsigned int i, unsigned char *bytecode, unsigned int size, GLuint shadertype)
	{
		objects[i] = new GLuint *;
		*(GLuint *)objects[i] = glCreateShader(shadertype);
		glShaderBinary(1, (GLuint *)objects[i], GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, bytecode, size);
		glSpecializeShader(*(GLuint *)objects[i], "main", i, nullptr, nullptr);

		GLint compiled;
		glGetShaderiv(*(GLuint *)objects[i], GL_COMPILE_STATUS, &compiled);
			
		if (!compiled) {
			GLint logsize;
			GLchar *infolog;
			glGetShaderiv(*(GLuint *)objects[i], GL_INFO_LOG_LENGTH, &logsize);
			infolog = new GLchar[logsize];
			glGetShaderInfoLog(*(GLuint *)objects[i], logsize, nullptr, infolog);
				
			// print error log
				
			glDeleteShader(*(GLuint *)objects[i]);
			delete infolog;
			delete (GLuint *)objects[i];
			objects[i] = nullptr;

			return false;
		}

		glAttachShader(*(GLuint *)program,  *(GLuint *)objects[i]);

		return true;
	}

	bool GLShader::Build(unsigned char *vs_bytecode, unsigned int vs_size,
			     unsigned char *fs_bytecode, unsigned int fs_size)
	{
		if (!vs_bytecode || !fs_bytecode)
			return false;
		
		program = new GLuint *;
		*(GLuint *)program = glCreateProgram();

		if (!CompileShader(0, vs_bytecode, vs_size, GL_VERTEX_SHADER) || !CompileShader(1, fs_bytecode, fs_size, GL_FRAGMENT_SHADER))
			return false;

		glLinkProgram(*(GLuint *)program);

		GLint linked;
		glGetProgramiv(*(GLuint *)program, GL_LINK_STATUS, &linked);
		if (!linked) {
			GLint logsize;
			GLchar *infolog;
			glGetProgramiv(*(GLuint *)program, GL_INFO_LOG_LENGTH, &logsize);
			infolog = new GLchar[logsize];
			glGetProgramInfoLog(*(GLuint *)program, logsize, nullptr, infolog);
				
			// print error log
			Release();

			return false;
		}

		return true;
	}

	void GLShader::Release()
	{
		if (program) {
			glDeleteProgram(*(GLuint *)program);
			delete (GLuint *)program;
			program = nullptr;
		}
		
		for (int i = 0; i < 2; i++) {
			if (objects[i]) {
				glDeleteShader(*(GLuint *)objects[i]);
				delete (GLuint *)objects[i];
				objects[i] = nullptr;
			}
		}
	}

}
