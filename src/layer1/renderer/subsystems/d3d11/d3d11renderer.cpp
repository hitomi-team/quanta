#include <iostream>
#include "d3d11renderer.h"

namespace Renderer {

	D3D11Renderer::D3D11Renderer()
	{
		SDL_Init(SDL_INIT_VIDEO);
		ResetCache();
	}

	bool D3D11Renderer::SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample)
	{
		int x = fullscreen ? 0 : SDL_WINDOWPOS_CENTERED;
		int y = fullscreen ? 0 : SDL_WINDOWPOS_CENTERED;

		unsigned flags = SDL_WINDOW_SHOWN;

		if (fullscreen) {
			SDL_GetCurrentDisplayMode(0, &display);

			width = display.w;
			height = display.h;

			flags |= SDL_WINDOW_FULLSCREEN;
		} else if (resizable) {
			flags |= SDL_WINDOW_RESIZABLE;
		} else if (borderless) {
			flags |= SDL_WINDOW_BORDERLESS;
		}

		window = SDL_CreateWindow(APP_TITLE, x, y, width, height, flags);
		if (!window) {
			FATAL(SDL_GetError());
			return false;
		}

		SDL_SysWMinfo sysinfo;
		SDL_VERSION(&sysinfo.version);
		SDL_GetWindowWMInfo(window, &sysinfo);

		HRESULT hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG, 0, 0, D3D11_SDK_VERSION, &device, 0, &context);

		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(device);
			D3D_SAFE_RELEASE(context);
			FATAL("Failed to create D3D11 device");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC swapchain_desc = {};
		swapchain_desc.BufferCount = 1;
		swapchain_desc.BufferDesc.Width = (unsigned int)width;
		swapchain_desc.BufferDesc.Height = (unsigned int)height;
		swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchain_desc.OutputWindow = sysinfo.info.win.window;
		swapchain_desc.SampleDesc.Count = (unsigned int)multisample;
		swapchain_desc.SampleDesc.Quality = multisample > 1 ? 0xffffffff : 0;
		swapchain_desc.Windowed = TRUE;
		swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		IDXGIDevice *dxgidev = nullptr;
		IDXGIAdapter *dxgiadapter = nullptr;
		IDXGIFactory *dxgifactory = nullptr;

		device->QueryInterface(IID_IDXGIDevice, (void **)&dxgidev);
		dxgidev->GetParent(IID_IDXGIAdapter, (void **)&dxgiadapter);
		dxgiadapter->GetParent(IID_IDXGIFactory, (void **)&dxgifactory);

		hr = dxgifactory->CreateSwapChain(device, &swapchain_desc, &swapchain);
		dxgifactory->MakeWindowAssociation(sysinfo.info.win.window, DXGI_MWA_NO_ALT_ENTER);

		dxgifactory->Release();
		dxgiadapter->Release();
		dxgidev->Release();

		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(swapchain);
			D3D_SAFE_RELEASE(device);
			D3D_SAFE_RELEASE(context);
			FATAL("Failed to create D3D11 swapchain");
			return false;			
		}

		if (!UpdateSwapchain(width, height, multisample))
			return false;

		vsync_ = vsync;

		Clear(CLEAR_COLOR);
		SetViewport(glm::vec4(0, 0, width, height));
		swapchain->Present(0, 0);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().DisplaySize.x = width;
		ImGui::GetIO().DisplaySize.y = height;
		ImGui::GetIO().Fonts->AddFontDefault();
		ImGui::GetIO().Fonts->Build();
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForD3D(window);
		ImGui_ImplDX11_Init(device, context);

		return true;
	}

	bool D3D11Renderer::UpdateSwapchain(int width, int height, int multisample)
	{
		ID3D11RenderTargetView *nullview = nullptr;
		context->OMSetRenderTargets(1, &nullview, 0);

		if (defaultRenderTargetView) {
			defaultRenderTargetView->Release();
			defaultRenderTargetView = nullptr;
		}
		if (defaultDepthStencilView) {
			defaultDepthStencilView->Release();
			defaultDepthStencilView = nullptr;
		}
		if (defaultDepthTexture) {
			defaultDepthTexture->Release();
			defaultDepthTexture = nullptr;
		}

		depthStencilView = nullptr;
		for (int i = 0; i < MAX_RENDERTARGETS; ++i)
			renderTargetViews[i] = nullptr;
		rendertargetsdirty_ = true;

		swapchain->ResizeBuffers(1,(UINT)width,(UINT)height,DXGI_FORMAT_UNKNOWN,DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		ID3D11Texture2D *backbuffertex;
		HRESULT hr = swapchain->GetBuffer(0, IID_ID3D11Texture2D, (void **)&backbuffertex);
		
		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(backbuffertex);
			FATAL("Failed to get backbuffer texture.");
			return false;
		} else {
			hr = device->CreateRenderTargetView(backbuffertex, 0, &defaultRenderTargetView);
			backbuffertex->Release();
			
			if (D3D_FAILED(hr)) {
				D3D_SAFE_RELEASE(defaultRenderTargetView);
				FATAL("Failed to create backbuffer rendertarget view.");
				return false;
			}
		}

		D3D11_TEXTURE2D_DESC depthdesc = {};
		depthdesc.Width = (UINT)width;
		depthdesc.Height = (UINT)height;
		depthdesc.MipLevels = 1;
		depthdesc.ArraySize = 1;
		depthdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthdesc.SampleDesc.Count = multisample;
		depthdesc.SampleDesc.Quality = multisample > 1 ? 0xFFFFFFFF : 0;
		depthdesc.Usage = D3D11_USAGE_DEFAULT;
		depthdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		
		hr = device->CreateTexture2D(&depthdesc, 0, &defaultDepthTexture);

		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(defaultDepthTexture);
			FATAL("Failed to create backbuffer depth-stencil texture.");
			return false;
		} else {
			hr = device->CreateDepthStencilView(defaultDepthTexture, 0, &defaultDepthStencilView);
			if (D3D_FAILED(hr)) {
				D3D_SAFE_RELEASE(defaultDepthStencilView);
				FATAL("Failed to create backbuffer depth-stencil view.");
				return false;
			}
		}

		return true;
	}

	void D3D11Renderer::PreDraw()
	{
		if (rendertargetsdirty_) {
			depthStencilView = defaultDepthStencilView;
			renderTargetViews[0] = defaultRenderTargetView;
			context->OMSetRenderTargets(MAX_RENDERTARGETS, &renderTargetViews[0], nullptr);
			rendertargetsdirty_ = false;
		}
	}

	bool D3D11Renderer::BeginFrame() // true = continue rendering, false = dont render
	{
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
			return false;
		
		PreDraw();
		
		return true;
	}
	
	void D3D11Renderer::EndFrame()
	{
		swapchain->Present(vsync_ ? 1 : 0, 0);
	}
	
	void D3D11Renderer::ResetCache()
	{
		for (int i = 0; i < MAX_RENDERTARGETS; ++i) {
			renderTargetViews[i] = nullptr;
		}

		defaultRenderTargetView = nullptr;
		defaultDepthTexture = nullptr;
		defaultDepthStencilView = nullptr;

		vsync_ = false;
		rendertargetsdirty_ = false;
	}

	void D3D11Renderer::Clear(unsigned flags, const glm::vec4& color, float depth, unsigned stencil)
	{
		if ((flags & CLEAR_COLOR) && renderTargetViews[0]) {
			for (int i = 0; i < MAX_RENDERTARGETS; ++i) {
				if (renderTargetViews[i])
					context->ClearRenderTargetView(renderTargetViews[i], &color[0]);
			}
		}

		if ((flags & (CLEAR_DEPTH | CLEAR_STENCIL)) && depthStencilView) {
			unsigned depthClearFlags = 0;
			if (flags & CLEAR_DEPTH)
				depthClearFlags |= D3D11_CLEAR_DEPTH;
			if (flags & CLEAR_STENCIL)
				depthClearFlags |= D3D11_CLEAR_STENCIL;
			
			context->ClearDepthStencilView(depthStencilView, depthClearFlags, depth, (UINT8)stencil);
		}
	}

	void D3D11Renderer::SetColorWrite(bool enable)
	{

	}

	void D3D11Renderer::SetDepthWrite(bool enable)
	{
	}

	void D3D11Renderer::SetViewport(const glm::vec4& rect)
	{
		D3D11_VIEWPORT d3dviewport;
		d3dviewport.TopLeftX = rect.x;
		d3dviewport.TopLeftY = rect.y;
		d3dviewport.Width = rect.z;
		d3dviewport.Height = rect.w;
		d3dviewport.MinDepth = 0.0f;
		d3dviewport.MaxDepth = 1.0f;

		context->RSSetViewports(1, &d3dviewport);
	}

	void D3D11Renderer::ImGuiNewFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
	}

	void D3D11Renderer::ImGuiEndFrame()
	{
		ImGui::End();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

}
