#ifndef RENDERER_INPUTLAYOUT_H
#define RENDERER_INPUTLAYOUT_H

#include "vertexbuffer.h"

namespace Renderer {

	class InputLayout {
	public:
		InputLayout() { inputlayout = nullptr; }
		virtual ~InputLayout() {  }

		// vsbytecode and vsbytecodelen are arguments because D3D11 needs them. I guess they could be ignored on Vulkan.
		// Since we're only using one form of data to be passed into the vertex shaders, we don't really need to deviate much from this.
		virtual bool Setup(unsigned char *vsbytecode, unsigned vsbytecodelen) { return false; }
		virtual void Release() {  }

		inline void *GetInputLayout() { return inputlayout; }
	
	protected:
		void *inputlayout; // [0] = Vertex, [1] = Normal, [2] = TexCoord
	};

}

#endif
