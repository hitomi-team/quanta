#ifndef RENDERER_INPUTLAYOUT_H
#define RENDERER_INPUTLAYOUT_H

#include "shader.h"
#include "vertexbuffer.h"

namespace Renderer {

	class InputLayout {
	public:
		InputLayout(Shader *vertexshader, VertexBuffer **buffers, unsigned *ElementMasks) { }
		virtual ~InputLayout() {  }

		void *GetInputLayout() const { return inputlayout; }
	
	protected:
		void *inputlayout;
	};

}

#endif
