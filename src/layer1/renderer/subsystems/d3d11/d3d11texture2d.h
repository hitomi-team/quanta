#ifndef RENDERER_D3D11TEXTURE2D_H
#define RENDERER_D3D11TEXTURE2D_H

#include "d3d11required.h"
#include "../../texture2d.h"

namespace Renderer {

	// buffer is ID3D11ShaderResourceView

	class D3D11Texture2D : public Texture2D {
	public:
		D3D11Texture2D() { texture_res = nullptr; }

		bool SetData(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc);
		void Release();
	
	protected:
		ID3D11Texture2D *texture_res;
	};

}

#endif
