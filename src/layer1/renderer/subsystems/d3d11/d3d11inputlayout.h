#ifndef RENDERER_D3D11INPUTLAYOUT_H
#define RENDERER_D3D11INPUTLAYOUT_H

#include "d3d11required.h"
#include "layer1/renderer/inputlayout.h"

namespace Renderer {

	class D3D11InputLayout : public InputLayout {
	public:
		D3D11InputLayout() {}

		bool Setup(unsigned char *vsbytecode, unsigned vsbytecodelen);
		void Release();

	};

}

#endif
