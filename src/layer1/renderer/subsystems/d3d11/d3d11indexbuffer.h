#ifndef RENDERER_D3D11INDEXBUFFER_H
#define RENDERER_D3D11INDEXBUFFER_H

#include "d3d11required.h"
#include "../../indexbuffer.h"

namespace Renderer {

	class D3D11IndexBuffer : public IndexBuffer {
	public:
		D3D11IndexBuffer() {}

		bool SetData(unsigned *indices, unsigned count);
		void Release();
	};

}

#endif
