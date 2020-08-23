#ifndef RENDERER_D3D11RENDERTARGET_H
#define RENDERER_D3D11RENDERTARGET_H

#include "d3d11required.h"
#include "layer1/renderer/rendertarget.h"

namespace Renderer {

	class D3D11RenderTarget : public RenderTarget {
	public:
		D3D11RenderTarget() { image = nullptr; }

		bool Setup(unsigned width, unsigned height, TextureUsage usage);
		void Release();
	
	protected:
		ID3D11Texture2D *image;
	};

}

#endif
