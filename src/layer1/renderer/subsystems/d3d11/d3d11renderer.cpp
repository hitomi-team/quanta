#include "pch/pch.h"

#include "d3d11renderer.h"

namespace Renderer {

	static const DWORD d3dBlendEnable[] =
	{
		FALSE,
		TRUE,
		TRUE,
		TRUE,
		TRUE,
		TRUE,
		TRUE,
		TRUE,
		TRUE,
		TRUE
	};

	static const D3D11_BLEND d3dSrcBlend[] =
	{
		D3D11_BLEND_ONE,
		D3D11_BLEND_ONE,
		D3D11_BLEND_DEST_COLOR,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_ONE,
		D3D11_BLEND_INV_DEST_ALPHA,
		D3D11_BLEND_ONE,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_ONE
	};

	static const D3D11_BLEND d3dDestBlend[] =
	{
		D3D11_BLEND_ZERO,
		D3D11_BLEND_ONE,
		D3D11_BLEND_ZERO,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_ONE,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_DEST_ALPHA,
		D3D11_BLEND_ONE,
		D3D11_BLEND_ONE,
		D3D11_BLEND_ONE
	};

	static const D3D11_BLEND_OP d3dBlendOp[] =
	{
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_REV_SUBTRACT,
		D3D11_BLEND_OP_REV_SUBTRACT,
		D3D11_BLEND_OP_MAX
	};

	static const D3D11_STENCIL_OP d3dStencilOp[] =
	{
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_ZERO,
		D3D11_STENCIL_OP_REPLACE,
		D3D11_STENCIL_OP_INCR,
		D3D11_STENCIL_OP_DECR
	};

	static const D3D11_CULL_MODE d3dCullMode[] =
	{
		D3D11_CULL_NONE,
		D3D11_CULL_BACK,
		D3D11_CULL_FRONT
	};

	static const D3D11_FILL_MODE d3dFillMode[] =
	{
		D3D11_FILL_SOLID,
		D3D11_FILL_WIREFRAME,
		D3D11_FILL_WIREFRAME // Point fill mode not supported
	};

	inline void GetD3D11PrimitiveType(PrimitiveType type, D3D_PRIMITIVE_TOPOLOGY *d3dtype, unsigned elementcount, unsigned *primitivecount)
	{
		switch (type) {
		case TRIANGLE_LIST:
			*primitivecount = elementcount / 3;
			*d3dtype = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			return;
		case POINT_LIST:
			*primitivecount = elementcount / 2;
			*d3dtype = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			return;
		default:
			*d3dtype = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
			FATAL("Unhandled Primitive Type!")
			return;
		}
	}

	D3D11Renderer::D3D11Renderer()
	{
		SDL_Init(SDL_INIT_VIDEO);
		ResetCache();
	}

	bool D3D11Renderer::SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample)
	{
		if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO)
			SDL_Init(SDL_INIT_VIDEO);

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

		HRESULT hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, D3D11_SDK_VERSION, &device, 0, &context);

		if (D3D_FAILED(hr)) {
			D3D_SAFE_RELEASE(device);
			D3D_SAFE_RELEASE(context);
			FATAL("Failed to create D3D11 device");
			return false;
		}

		d3d11_global_device = device;
		d3d11_global_context = context;

		DXGI_SWAP_CHAIN_DESC swapchain_desc = {};
		swapchain_desc.BufferCount = 2;
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
		renderTargetsDirty_ = true;

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
		if (renderTargetsDirty_) {
			if (!depthStencilView)
				depthStencilView = defaultDepthStencilView;
			if (!renderTargetViews[0])
				renderTargetViews[0] = defaultRenderTargetView;
			
			context->OMSetRenderTargets(MAX_RENDERTARGETS, &renderTargetViews[0], nullptr);
			renderTargetsDirty_ = false;
		}

		// Bind Shader Resource Views to Pixel Shader
		if (textureViewsDirty_) {
			context->PSSetSamplers(0, MAX_TEXTURE_UNITS, &samplerStates_[0]);
			context->PSSetShaderResources(0, MAX_TEXTURE_UNITS, &textureViews_[0]);
			textureViewsDirty_ = false;
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

		primitiveCount = 0;
		drawCount = 0;
		drawIndexedCount = 0;
		drawInstancedCount = 0;
	}


	glm::vec2 D3D11Renderer::GetRenderTargetDimensions()
	{
		return glm::vec2(0, 0);
	}

	void D3D11Renderer::CreateRendererCapabilities()
	{
	}

	std::vector<int> D3D11Renderer::GetMultiSampleLevels()
	{
		std::vector< int > a;
		return a;
	}


	void D3D11Renderer::Close()
	{
		ImGui_ImplDX11_Shutdown();

		D3D_SAFE_RELEASE(defaultRenderTargetView);
		D3D_SAFE_RELEASE(defaultDepthTexture);
		D3D_SAFE_RELEASE(defaultDepthStencilView);
		D3D_SAFE_RELEASE(swapchain);
		D3D_SAFE_RELEASE(context);
		D3D_SAFE_RELEASE(device);

		ResetCache();

		SDL_ShowCursor(SDL_TRUE);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void D3D11Renderer::AddGpuResource(GPUResource* object)
	{
		(void)object;
	}

	void D3D11Renderer::RemoveGpuResource(GPUResource* object)
	{
		(void)object;
	}

	void D3D11Renderer::ResetCache()
	{
		for (int i = 0; i < MAX_RENDERTARGETS; ++i) {
			renderTargetViews[i] = nullptr;
		}

		for (int i = 0; i < MAX_TEXTURE_UNITS; i++) {
			samplerStates_[i] = nullptr;
			textureViews_[i] = nullptr;
		}

		defaultRenderTargetView = nullptr;
		defaultDepthTexture = nullptr;
		defaultDepthStencilView = nullptr;
		primitiveType_ = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		shaderProgram_ = nullptr;
		vertexBuffer_ = nullptr;

		vsync_ = false;
		renderTargetsDirty_ = false;
		textureViewsDirty_ = false;
		rasterizerStateDirty_ = false;

		d3d11_global_device = nullptr;
		d3d11_global_context = nullptr;
	}

	bool D3D11Renderer::IsDeviceLost()
	{
		return false;
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

	VertexBuffer* D3D11Renderer::CreateVertexBuffer(Vertex *vertices, unsigned count)
	{
		if (!vertices || !count)
			return nullptr;

		D3D11VertexBuffer *vertbuf = new D3D11VertexBuffer;

		if (!vertbuf->SetData(vertices, count)) {
			delete vertbuf;
			return nullptr;
		}

		return vertbuf;
	}

	IndexBuffer* D3D11Renderer::CreateIndexBuffer(unsigned *indices, unsigned count)
	{
		if (!indices || !count)
			return nullptr;

		D3D11IndexBuffer *indexbuf = new D3D11IndexBuffer;

		if (!indexbuf->SetData(indices, count)) {
			delete indexbuf;
			return nullptr;
		}

		return indexbuf;
	}

	InputLayout* D3D11Renderer::CreateInputLayout(unsigned char *vsbytecode, unsigned vsbytecodelen)
	{
		if (!vsbytecode || !vsbytecodelen)
			return nullptr;

		D3D11InputLayout *inputlayout = new D3D11InputLayout;

		if (!inputlayout->Setup(vsbytecode, vsbytecodelen)) {
			delete inputlayout;
			return nullptr;
		}

		return inputlayout;
	}

	Shader *D3D11Renderer::CreateShader(unsigned char *vs_bytecode, unsigned int vs_size,
					    unsigned char *fs_bytecode, unsigned int fs_size)
	{
		if ((!vs_bytecode || !vs_size) || (!fs_bytecode || !fs_size))
			return nullptr;

		D3D11Shader *shader = new D3D11Shader;

		if (!shader->Build(vs_bytecode, vs_size, fs_bytecode, fs_size)) {
			delete shader;
			return nullptr;
		}

		return shader;
	}

	Texture2D *D3D11Renderer::CreateTexture2D(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc)
	{
		if (!data || !width || !height)
			return nullptr;

		D3D11Texture2D *texture2d = new D3D11Texture2D;

		if (!texture2d->SetData(data, width, height, samplerstatedesc)) {
			delete texture2d;
			return nullptr;
		}

		return texture2d;
	}

	ShaderParameterBuffer *D3D11Renderer::CreateShaderParameterBuffer(std::vector<ShaderParameterElement> elements)
	{
		if (!elements.size())
			return nullptr;
		
		D3D11ShaderParameterBuffer *shaderParam = new D3D11ShaderParameterBuffer;

		if (!shaderParam->Setup(elements)) {
			delete shaderParam;
			return nullptr;
		}		
	
		return shaderParam;
	}


	void D3D11Renderer::SetShaders(Shader *shader)
	{
		// Assume the shaders are already compiled (Which they should be)
		if (!shader)
			return;

		if (shaderProgram_ == shader)
			return;

		ID3D11VertexShader *vs = (ID3D11VertexShader *)shader->GetShaderObject(VS);
		ID3D11PixelShader *ps = (ID3D11PixelShader *)shader->GetShaderObject(FS);
		ID3D11InputLayout *il = (ID3D11InputLayout *)shader->GetInputLayout();

		if (!vs || !ps || !il)
			return;

		context->IASetInputLayout(il);
		context->VSSetShader(vs, nullptr, 0);
		context->PSSetShader(ps, nullptr, 0);

		shaderProgram_ = shader;
	}

	void D3D11Renderer::SetVertexBuffer(VertexBuffer* buffer)
	{
		unsigned stride = 0;
		unsigned offset = 0;

		if (!buffer) {
			context->IASetVertexBuffers(0, 1, nullptr, &stride, &offset);
			vertexBuffer_ = nullptr;

			return;
		}


		ID3D11Buffer *vbuf = (ID3D11Buffer *)buffer->GetBuffer();
		if (vbuf != vertexBuffer_) {
			stride = sizeof(Vertex);
			context->IASetVertexBuffers(0, 1, &vbuf, &stride, &offset);
			vertexBuffer_ = vbuf;
		}
	}

	void D3D11Renderer::SetIndexBuffer(IndexBuffer* buffer)
	{
		if (!buffer) {
			context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

			return;
		}

		ID3D11Buffer *ibuf = (ID3D11Buffer *)buffer->GetBuffer();
		if (ibuf != indexBuffer_) {
			context->IASetIndexBuffer(ibuf, DXGI_FORMAT_R32_UINT, 0);
			indexBuffer_ = ibuf;
		}
	}

	bool D3D11Renderer::SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, const std::vector<unsigned>& elementMasks, unsigned instanceOffset)
	{
		(void)buffers;
		(void)elementMasks;
		(void)instanceOffset;
		return true;
	}

	void D3D11Renderer::SetFlushGPU(bool flushGpu)
	{
		(void)flushGpu;
	}

	void D3D11Renderer::SetBlendMode(BlendMode mode)
	{
		(void)mode;
	}

	void D3D11Renderer::SetColorWrite(bool enable)
	{
		(void)enable;
	}

	void D3D11Renderer::SetCullMode(CullMode mode)
	{
		(void)mode;
	}

	void D3D11Renderer::SetDepthBias(float constantBias, float slopeScaledBias)
	{
		(void)constantBias;
		(void)slopeScaledBias;
	}

	void D3D11Renderer::SetDepthTest(CompareMode mode)
	{
		(void)mode;
	}

	void D3D11Renderer::SetDepthWrite(bool enable)
	{
		(void)enable;
	}

	void D3D11Renderer::SetFillMode(FillMode mode)
	{
		(void)mode;
	}

	void D3D11Renderer::SetScissorTest(bool enable, const glm::vec2& rect)
	{
		(void)enable;
		(void)rect;
	}

	void D3D11Renderer::SetStencilTest(bool enable, CompareMode mode, StencilOp pass, StencilOp fail, StencilOp zFail, unsigned stencilRef, unsigned compareMask, unsigned writeMask)
	{
		(void)enable;
		(void)mode;
		(void)pass;
		(void)fail;
		(void)zFail;
		(void)stencilRef;
		(void)compareMask;
		(void)writeMask;
	}


	void D3D11Renderer::SetTexture(unsigned index, Texture2D* texture)
	{
		if ((index >= MAX_TEXTURE_UNITS) || !texture)
			return;

		if ((ID3D11ShaderResourceView *)texture->GetView() != textureViews_[index]) {
			samplerStates_[index] = (ID3D11SamplerState *)texture->GetSampler();
			textureViews_[index] = (ID3D11ShaderResourceView *)texture->GetView();
			textureViewsDirty_ = true;
		}
	}

	void D3D11Renderer::SetRenderTarget(unsigned index, RenderTarget* renderTarget)
	{
		if (index >= MAX_RENDERTARGETS)
			return;
		
		if (renderTarget->getView() != this->renderTargetViews[index]) {
			renderTargetViews[index] = (ID3D11RenderTargetView *)renderTarget->getView();
			renderTargetsDirty_ = true;
		}
	}

	void D3D11Renderer::SetDepthStencil(RenderTarget* depthStencil)
	{
		(void)depthStencil;
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

	void D3D11Renderer::Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount)
	{
		if (!shaderProgram_ || !vertexCount)
			return;

		PreDraw();

		unsigned primitive = 0;
		D3D_PRIMITIVE_TOPOLOGY d3dtype;
		GetD3D11PrimitiveType(type, &d3dtype, vertexCount, &primitive);
		if (d3dtype != primitiveType_) {
			context->IASetPrimitiveTopology(d3dtype);
			primitiveType_ = d3dtype;
		}

		context->Draw(vertexCount, vertexStart);
		global_log.Info("draw!");

#ifdef PROFILING
		primitiveCount += primitive;
		++drawCount;
#endif
	}

	void D3D11Renderer::DrawIndexed(PrimitiveType type, unsigned indexStart, unsigned indexCount)
	{
		if (!shaderProgram_ || !indexCount) {
			return;
		}

		unsigned primitive = 0;
		D3D_PRIMITIVE_TOPOLOGY d3dtype;
		GetD3D11PrimitiveType(type, &d3dtype, indexCount, &primitive);
		if (d3dtype != primitiveType_) {
			context->IASetPrimitiveTopology(d3dtype);
			primitiveType_ = d3dtype;
		}

		context->DrawIndexed(indexCount, indexStart, 0);

#ifdef PROFILING
		primitiveCount += primitive;
		++drawIndexedCount;
#endif
	}

	void D3D11Renderer::DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned instanceCount)
	{
		if (!shaderProgram_ || !indexCount || !instanceCount)
			return;

		PreDraw();

		unsigned primitive = 0;
		D3D_PRIMITIVE_TOPOLOGY d3dtype;
		GetD3D11PrimitiveType(type, &d3dtype, indexCount, &primitive);
		if (d3dtype != primitiveType_) {
			context->IASetPrimitiveTopology(d3dtype);
			primitiveType_ = d3dtype;
		}

		context->DrawIndexedInstanced(indexCount, instanceCount, indexStart, 0, 0);

#ifdef PROFILING
		primitiveCount += instanceCount * primitive;
		++drawInstancedCount;
#endif
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

	RendererType D3D11Renderer::getRendererType()
	{
		return RENDERER_D3D11;
	}

	ID3D11Device *d3d11_global_device;
	ID3D11DeviceContext *d3d11_global_context;

}
