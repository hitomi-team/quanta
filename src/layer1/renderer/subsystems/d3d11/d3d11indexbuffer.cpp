#include "pch/pch.h"

#include "d3d11indexbuffer.h"

namespace Renderer {

	bool D3D11IndexBuffer::SetData(unsigned *indices, unsigned count)
	{
		if (!d3d11_global_device)
			return false;

		if (!indices || count == 0)
			return false;

		D3D11_BUFFER_DESC index_desc = {};
		index_desc.ByteWidth = sizeof(unsigned) * count;
		index_desc.Usage = D3D11_USAGE_DEFAULT;
		index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sr_data = {};
		sr_data.pSysMem = indices;

		HRESULT hr = d3d11_global_device->CreateBuffer(&index_desc, &sr_data, (ID3D11Buffer **)&buffer);
		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(buffer);
			return false;
		}

		this->count = count;

		return true;

	}

	void D3D11IndexBuffer::Release()
	{
		D3D_SAFE_RELEASE(buffer);
		this->count = 0;
	}

}
