#ifndef RENDERER_D3D11SHADER_H
#define RENDERER_D3D11SHADER_H

#include "d3d11required.h"
#include "../../shader.h"

namespace Renderer {

	class D3D11Shader : public Shader {
	public:
		D3D11Shader() {}

		bool Build(unsigned char *vs_bytecode, unsigned int vs_size,
			   unsigned char *fs_bytecode, unsigned int fs_size);
		void Release();

		void *GetProgram() { return nullptr; } // dont use this for d3d11!
		void *GetObject(ShaderType type);
	};

}

#endif
