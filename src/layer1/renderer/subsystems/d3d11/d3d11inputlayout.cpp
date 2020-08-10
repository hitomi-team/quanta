#include "pch/pch.h"

#include "d3d11inputlayout.h"

namespace Renderer {

	bool D3D11InputLayout::Setup(unsigned char *vsbytecode, unsigned vsbytecodelen)
	{
		if (!vsbytecode || !vsbytecodelen)
			return false;

		D3D11_INPUT_ELEMENT_DESC layout_descs[3];

		layout_descs[0] = {};
		layout_descs[0].SemanticName = "POSITION";
		layout_descs[0].SemanticIndex = 0;
		layout_descs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		layout_descs[0].InputSlot = 0;
		layout_descs[0].AlignedByteOffset = 0;
		layout_descs[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layout_descs[0].InstanceDataStepRate = 0;

		layout_descs[1] = {};
		layout_descs[1].SemanticName = "NORMAL";
		layout_descs[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		layout_descs[1].InputSlot = 0;
		layout_descs[1].AlignedByteOffset = 12;
		layout_descs[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layout_descs[1].InstanceDataStepRate = 0;

		layout_descs[2] = {};
		layout_descs[2].SemanticName = "TEXCOORD";
		layout_descs[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		layout_descs[2].InputSlot = 0;
		layout_descs[2].AlignedByteOffset = 24;
		layout_descs[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layout_descs[2].InstanceDataStepRate = 0;

		HRESULT hr = d3d11_global_device->CreateInputLayout(layout_descs, 3, vsbytecode, vsbytecodelen, (ID3D11InputLayout **)&inputlayout);
		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(inputlayout);

			return false;
		}

		return true;
	}

	void D3D11InputLayout::Release()
	{
		D3D_SAFE_RELEASE(inputlayout);
	}

}
