#ifndef RENDERER_D3D11VERTEXBUFFER_H
#define RENDERER_D3D11VERTEXBUFFER_H

#include "d3d11required.h"

#include "layer1/renderer/vertexbuffer.h"

namespace Renderer {

	class D3D11VertexBuffer : public VertexBuffer {
	public:
		D3D11VertexBuffer() {}

		bool SetData(Vertex *vertices, unsigned count);
		void Release();
	};

}

#endif
