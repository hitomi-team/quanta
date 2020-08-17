#include "pch/pch.h"

#include "d3d11vertexbuffer.h"

namespace Renderer {

	bool D3D11VertexBuffer::SetData(Vertex *vertices, unsigned count)
	{
		if (!d3d11_global_device)
			return false;

		if (!vertices || count == 0)
			return false;

		D3D11_BUFFER_DESC vertex_desc = {};
		vertex_desc.ByteWidth = sizeof(Vertex) * count;
		vertex_desc.Usage = D3D11_USAGE_IMMUTABLE;
		vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sr_data = {};
		sr_data.pSysMem = vertices;

		HRESULT hr = d3d11_global_device->CreateBuffer(&vertex_desc, &sr_data, (ID3D11Buffer **)&buffer);
		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(buffer);
			return false;
		}

		this->count = count;

		return true;
	}

	void D3D11VertexBuffer::Release()
	{
		D3D_SAFE_RELEASE(buffer);
		this->count = 0;
	}

}
