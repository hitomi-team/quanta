#ifndef GL_SHADER_H
#define GL_SHADER_H

#include "glrenderer.h"
#include "../../shader.h"

namespace Renderer {

	class GLShader : public Shader {
		GLShader() {  }
		
		bool Build(unsigned char *vs_bytecode, unsigned int vs_size,
			   unsigned char *fs_bytecode, unsigned int fs_size);
		void Release();

	private:
		bool CompileShader(unsigned int i, unsigned char *bytecode, unsigned int size, GLuint shadertype);
	};

}

#endif
