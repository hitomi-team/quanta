#include "pch/pch.h"

#include "layer1/log.h"

#include "d3d11shader.h"

namespace Renderer {

	bool D3D11Shader::Build(unsigned char *vs_bytecode, unsigned int vs_size,
				unsigned char *fs_bytecode, unsigned int fs_size)
	{
		if (!d3d11_global_device)
			return false;

		if ((!vs_bytecode || !vs_size) || (!fs_bytecode || !fs_size))
			return false;

		for (unsigned i = 0; i < 2; i++)
			objects[i] = nullptr;

		HRESULT hr = d3d11_global_device->CreateVertexShader((const void *)vs_bytecode, vs_size, nullptr, (ID3D11VertexShader **)&objects[0]);
		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(objects[0]);
			return false;
		}

		hr = d3d11_global_device->CreatePixelShader((const void *)fs_bytecode, fs_size, nullptr, (ID3D11PixelShader **)&objects[1]);
		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(objects[1]);
			return false;
		}

		inputlayout = new D3D11InputLayout;

		if (!inputlayout->Setup(vs_bytecode, vs_size)) {
			Release();
			return false;
		}

		return true;
	}

	void D3D11Shader::Release()
	{
		for (unsigned i = 0; i < 2; i++)
			D3D_SAFE_RELEASE(objects[i]);

		if (inputlayout) {
			inputlayout->Release();
			delete inputlayout;
			inputlayout = nullptr;
		}
	}

}
