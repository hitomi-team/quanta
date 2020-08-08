#ifndef RENDERER_D3D11REQUIRED_H
#define RENDERER_D3D11REQUIRED_H

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <d3d11.h>
#include <dxgi.h>

#define D3D_FAILED(x) (x != S_OK)
#define D3D_SAFE_RELEASE(p) if (p != nullptr) { ((IUnknown*)p)->Release();  p = nullptr; }

namespace Renderer {

	extern ID3D11Device *d3d11_global_device;
	extern ID3D11DeviceContext *d3d11_global_context;

}

#endif
