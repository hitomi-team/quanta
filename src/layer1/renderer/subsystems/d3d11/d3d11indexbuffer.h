#ifndef RENDERER_D3D11INDEXBUFFER_H
#define RENDERER_D3D11INDEXBUFFER_H

#include "d3d11required.h"
#include "layer1/renderer/indexbuffer.h"

namespace Renderer {

	class D3D11IndexBuffer : public IndexBuffer {
	public:
		D3D11IndexBuffer() {}

		bool SetData(const uint16_t *indices, unsigned count);
		void Release();
	};

}

#endif
