#ifndef RENDERER_SHADER_H
#define RENDERER_SHADER_H

#include "defs.h"

namespace Renderer {

	class Shader {
	public:
		Shader() {  }
		virtual ~Shader() {  }

		// true on success, false on failure.
		virtual bool Build(unsigned char *vs_bytecode, unsigned int vs_size,
				   unsigned char *fs_bytecode, unsigned int fs_size) { return true; }
		virtual void Release() {  }
		virtual void *GetProgram() { return program; }
		virtual void *GetObject(ShaderType type);

	protected:
		void *program; // I guess this can be the pipeline object?
		void *objects[2]; // put into pipeline in Vulkan, used for setting shader in D3D11
	};

}

#endif
