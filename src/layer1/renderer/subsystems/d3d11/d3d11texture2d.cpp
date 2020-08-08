#include "d3d11texture2d.h"


#include "d3d11texture2d.h"

namespace Renderer {

	static const D3D11_TEXTURE_ADDRESS_MODE d3dAddressMode[] =
	{
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MIRROR,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_BORDER
	};

	static const D3D11_COMPARISON_FUNC d3dComparisonFunc[] = {
		D3D11_COMPARISON_NEVER,
		D3D11_COMPARISON_EQUAL,
		D3D11_COMPARISON_NOT_EQUAL,
		D3D11_COMPARISON_LESS,
		D3D11_COMPARISON_LESS_EQUAL,
		D3D11_COMPARISON_GREATER,
		D3D11_COMPARISON_GREATER_EQUAL,
		D3D11_COMPARISON_ALWAYS
	};

	bool D3D11Texture2D::SetData(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc)
	{
		if (!data || !width || !height)
			return false;
		
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		D3D11_SUBRESOURCE_DATA sr_data = {};
		sr_data.pSysMem = data;
		sr_data.SysMemPitch = sizeof(unsigned char) * 4 * width;

		HRESULT hr = d3d11_global_device->CreateTexture2D(&desc, &sr_data, &texture_res);
		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(texture_res);
			return false;
		}

//		d3d11_global_context->UpdateSubresource(texture_res, 0, nullptr, data, width * 4 * sizeof(unsigned char), 0);

		D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
		view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		view_desc.Texture2D.MipLevels = -1;
		view_desc.Texture2D.MostDetailedMip = 0;

		hr = d3d11_global_device->CreateShaderResourceView(texture_res, &view_desc, (ID3D11ShaderResourceView **)&buffer);
		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(buffer);
			D3D_SAFE_RELEASE(texture_res);
			return false;
		}

		d3d11_global_context->GenerateMips((ID3D11ShaderResourceView *)buffer);

		D3D11_FILTER filter;

		switch(samplerstatedesc.Filter) {
		case FILTER_NEAREST:
			filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			break;
		case FILTER_BILINEAR:
			filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			break;
		case FILTER_TRILINEAR:
			filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		case FILTER_ANISOTROPIC:
			filter = D3D11_FILTER_ANISOTROPIC;
			break;
		case FILTER_DEFAULT:
		default:
			break;
		}

		D3D11_SAMPLER_DESC sampler_desc = {};
		sampler_desc.Filter = filter;
		sampler_desc.AddressU = d3dAddressMode[samplerstatedesc.AddressModeU];
		sampler_desc.AddressV = d3dAddressMode[samplerstatedesc.AddressModeV];
		sampler_desc.AddressW = d3dAddressMode[samplerstatedesc.AddressModeW];
		sampler_desc.MipLODBias = samplerstatedesc.MipLODBias;
		sampler_desc.MaxAnisotropy = samplerstatedesc.MaxAniso;
		sampler_desc.ComparisonFunc = d3dComparisonFunc[samplerstatedesc.ComparisonFunc];
		
		for (int i = 0; i < 4; i++)
			sampler_desc.BorderColor[i] = samplerstatedesc.BorderColor[i];

		sampler_desc.MinLOD = samplerstatedesc.MinLOD;
		sampler_desc.MaxLOD = samplerstatedesc.MaxLOD;

		hr = d3d11_global_device->CreateSamplerState(&sampler_desc, (ID3D11SamplerState **)&sampler);
		if (D3D_FAILED(hr)) {
			Release();
		}

		return true;
	}

	void D3D11Texture2D::Release()
	{
		D3D_SAFE_RELEASE(buffer);
		D3D_SAFE_RELEASE(sampler);
		D3D_SAFE_RELEASE(texture_res);
	}

}
