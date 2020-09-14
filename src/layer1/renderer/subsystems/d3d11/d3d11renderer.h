#ifndef RENDERER_D3D11RENDERER_H
#define RENDERER_D3D11RENDERER_H

#include "layer1/renderer/subsystems/required.h"
#include "d3d11required.h"

#include "layer1/renderer/imgui/imgui_impl_sdl.h"
#include "imgui_impl_d3d11.h"

#include "d3d11inputlayout.h"
#include "d3d11parameterbuffer.h"
#include "d3d11indexbuffer.h"
#include "d3d11vertexbuffer.h"
#include "d3d11texture2d.h"
#include "d3d11shader.h"

namespace Renderer {

	class D3D11Renderer : public RHI {
	public:
		D3D11Renderer();
		inline virtual ~D3D11Renderer() {}

		bool SetGraphicsMode(int width = WINDOW_DEFAULT_WIDTH, int height = WINDOW_DEFAULT_HEIGHT, bool fullscreen = false, bool borderless = false, bool resizable = false, bool vsync = true, int multisample = 0);
		void Clear(unsigned flags, const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), float depth = 1.0f, unsigned stencil = 0);

// TO BE IMPLEMENTED

		bool BeginFrame(); // true = continue rendering, false = dont render
		void EndFrame();


		glm::vec2 GetRenderTargetDimensions();
		void CreateRendererCapabilities();
		std::vector<int> GetMultiSampleLevels();


		void Close();

		void AddGpuResource(GPUResource* object);
		void RemoveGpuResource(GPUResource* object);

		void ResetCache();

		bool IsDeviceLost();


		VertexBuffer* CreateVertexBuffer(Vertex *vertices, unsigned count);
		IndexBuffer* CreateIndexBuffer(unsigned *indices, unsigned count);
		InputLayout* CreateInputLayout(unsigned char *vsbytecode, unsigned vsbytecodelen);
		Shader *CreateShader(unsigned char *vs_bytecode, unsigned int vs_size,
				     unsigned char *fs_bytecode, unsigned int fs_size);
		Texture2D *CreateTexture2D(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc);
		ShaderParameterBuffer *CreateShaderParameterBuffer(std::vector<ShaderParameterElement> elements);

		void SetShaders(Shader *shader);
		void SetShaderParameter(unsigned param, const float* data, unsigned count);

		void SetVertexBuffer(VertexBuffer* buffer);
		void SetIndexBuffer(IndexBuffer* buffer);
		bool SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, const std::vector<unsigned>& elementMasks, unsigned instanceOffset);

		void SetFlushGPU(bool flushGpu);
		void SetBlendMode(BlendMode mode);
		void SetColorWrite(bool enable);
		void SetCullMode(CullMode mode);
		void SetDepthBias(float constantBias, float slopeScaledBias);
		void SetDepthTest(CompareMode mode);
		void SetDepthWrite(bool enable);
		void SetFillMode(FillMode mode);
		void SetScissorTest(bool enable, const glm::vec2& rect);
		void SetStencilTest(bool enable, CompareMode mode = CMP_ALWAYS, StencilOp pass = OP_KEEP, StencilOp fail = OP_KEEP, StencilOp zFail = OP_KEEP, unsigned stencilRef = 0, unsigned compareMask = UINT32_MAX, unsigned writeMask = UINT32_MAX);

		void SetTexture(unsigned index, Texture2D *texture);

		void SetRenderTarget(unsigned index, RenderTarget* renderTarget);
		void SetDepthStencil(RenderTarget* depthStencil);

		void SetViewport(const glm::vec4& rect);

		void Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount);
		void DrawIndexed(PrimitiveType type, unsigned indexStart, unsigned indexCount);
		void DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned instanceCount);

		SDL_Window *GetWindow() { return window; }

		void ImGuiNewFrame();
		void ImGuiEndFrame();

		RendererType getRendererType();

	private:
		SDL_Window *window;
		SDL_DisplayMode display;
		bool isInitialized;

		ID3D11Device *device;
		ID3D11DeviceContext *context;
		IDXGISwapChain *swapchain;


		// internal cache
		ID3D11RenderTargetView *defaultRenderTargetView;
		ID3D11Texture2D        *defaultDepthTexture;
		ID3D11DepthStencilView *defaultDepthStencilView;
		ID3D11RenderTargetView *renderTargetViews[MAX_RENDERTARGETS];
		ID3D11DepthStencilView *depthStencilView;
		ID3D11ShaderResourceView *textureViews_[MAX_TEXTURE_UNITS];
		ID3D11SamplerState *samplerStates_[MAX_TEXTURE_UNITS];
		D3D_PRIMITIVE_TOPOLOGY primitiveType_;
		Shader *shaderProgram_;
		ID3D11Buffer *vertexBuffer_;
		ID3D11Buffer *indexBuffer_;

		// states
		bool vsync_;
		bool renderTargetsDirty_;
		bool textureViewsDirty_;
		bool rasterizerStateDirty_;


		// internal functions
		bool UpdateSwapchain(int width, int height, int multisample);
		void PreDraw();
	};

}

#endif
