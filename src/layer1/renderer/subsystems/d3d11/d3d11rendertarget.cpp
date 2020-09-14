#include "pch/pch.h"

#include "d3d11rendertarget.h"

namespace Renderer {

	bool D3D11RenderTarget::Setup(unsigned width, unsigned height, TextureUsage usage)
	{
		DXGI_FORMAT d3dformat;
		D3D11_USAGE d3dusage = D3D11_USAGE_DEFAULT;
		UINT d3dbind = D3D11_BIND_SHADER_RESOURCE;

		switch (usage) {
		case TEXTURE_RENDERTARGET:
			d3dformat = DXGI_FORMAT_R8G8B8A8_UNORM;
			d3dbind |= D3D11_BIND_RENDER_TARGET;
			break;
		case TEXTURE_DEPTHSTENCIL: // fall through on purpose
			d3dformat = DXGI_FORMAT_R32_FLOAT;
			d3dbind |= D3D11_BIND_DEPTH_STENCIL;
			break;
		case TEXTURE_DEPTHSTENCIL_READONLY:
			d3dformat = DXGI_FORMAT_R32_FLOAT;
			d3dusage = D3D11_USAGE_IMMUTABLE;
			d3dbind |= D3D11_BIND_DEPTH_STENCIL;
			break;
		default:
			global_log.Error("d3d11: Unhandled TextureUsage case for RenderTarget generation!");
			return false;
		};

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = d3dformat;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = d3dusage;
		desc.BindFlags = d3dbind;
		desc.MiscFlags = 0;
		
		HRESULT hr = d3d11_global_device->CreateTexture2D(&desc, nullptr, &image);
		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(image);
			global_log.Error("d3d11: RenderTarget " + hr);
			return false;
		}

/*		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
		viewDesc.Format = desc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipLevels = -1;
		viewDesc.Texture2D.MostDetailedMip = 0;
*/		
		if (usage == TEXTURE_RENDERTARGET) {
			D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc = {};
			rtViewDesc.Format = desc.Format;
			rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			d3d11_global_device->CreateRenderTargetView((ID3D11Texture2D *)image, &rtViewDesc, (ID3D11RenderTargetView **)&view);
		} else {
			global_log.Error("d3d11: Unhandled RenderTarget type!");
			return false;
		}

		return true;
	}

	void D3D11RenderTarget::Release()
	{
		D3D_SAFE_RELEASE(view);
		D3D_SAFE_RELEASE(image);
	}

}
