#ifndef RENDERER_D3D11SHADER_H
#define RENDERER_D3D11SHADER_H

#include "d3d11required.h"

#include "d3d11inputlayout.h"
#include "layer1/renderer/shader.h"

namespace Renderer {

	class D3D11Shader : public Shader {
	public:
		inline D3D11Shader() {}

		bool Build(unsigned char *vs_bytecode, unsigned int vs_size,
			   unsigned char *fs_bytecode, unsigned int fs_size);
		void Release();

		inline void *GetProgram() { return nullptr; } // dont use this for d3d11!
		void *GetShaderObject(ShaderType type);
		void *GetInputLayout();

	};

}

#endif
