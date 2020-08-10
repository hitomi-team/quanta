#ifndef RENDERER_SHADER_H
#define RENDERER_SHADER_H

#include "inputlayout.h"
#include "defs.h"

namespace Renderer {

	class Shader {
	public:
		inline Shader() { program = nullptr; objects[0] = nullptr; objects[1] = nullptr; inputlayout = nullptr; }
		inline virtual ~Shader() {  }

		// true on success, false on failure.
		virtual bool Build(unsigned char *vs_bytecode, unsigned int vs_size,
				   unsigned char *fs_bytecode, unsigned int fs_size) = 0;
		virtual void Release() = 0;

		virtual void *GetProgram() = 0;

		// koukuno: used to be GetObject, but WinAPI kicks in.
		virtual void *GetShaderObject(ShaderType type) = 0;
		virtual void *GetInputLayout() = 0;

	protected:
		void *program; // I guess this can be the pipeline object?
		void *objects[3]; // put into pipeline in Vulkan, used for setting shader in D3D11

		InputLayout *inputlayout;
	};

}

#endif
