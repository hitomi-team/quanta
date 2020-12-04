#ifndef RENDERER_RHI_H
#define RENDERER_RHI_H

// Renderer Handler Implementation

#include "required.h"

namespace Renderer {

	class RHI {
	public:
		inline RHI() {}
		inline virtual ~RHI() {}

		virtual bool SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample) = 0;
		virtual bool BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Clear(unsigned flags, const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), float depth = 1.0f, unsigned stencil = 0) = 0;

		virtual glm::vec2 GetRenderTargetDimensions() = 0;
		virtual void CreateRendererCapabilities() = 0;
		virtual std::vector<int> GetMultiSampleLevels() = 0;

		virtual void Close() = 0;

		virtual void AddGpuResource(GPUResource* object) = 0;
		virtual void RemoveGpuResource(GPUResource* object) = 0;

		virtual void ResetCache() = 0;

		virtual bool IsDeviceLost() = 0;

		virtual VertexBuffer* CreateVertexBuffer(const Vertex *vertices, unsigned count) = 0;
		virtual IndexBuffer* CreateIndexBuffer(const uint16_t *indices, unsigned count) = 0;
		virtual InputLayout* CreateInputLayout(unsigned char *vsbytecode, unsigned vsbytecodelen) = 0;
		virtual Shader *CreateShader(unsigned char *vs_bytecode, unsigned int vs_size,
					     unsigned char *fs_bytecode, unsigned int fs_size) = 0;
		virtual Texture2D *CreateTexture2D(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc) = 0;
		virtual ShaderParameterBuffer *CreateShaderParameterBuffer(std::vector<ShaderParameterElement> elements) = 0;

		virtual void SetShaders(Shader *shader) = 0;

		virtual void SetVertexBuffer(VertexBuffer* buffer) = 0;
		virtual void SetIndexBuffer(IndexBuffer* buffer) = 0;
		virtual bool SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, const std::vector<unsigned>& elementMasks, unsigned instanceOffset = 0) = 0;

		virtual void SetFlushGPU(bool flushGpu) = 0;
		virtual void SetBlendMode(BlendMode mode) = 0;
		virtual void SetColorWrite(bool enable) = 0;
		virtual void SetCullMode(CullMode mode) = 0;
		virtual void SetDepthBias(float constantBias, float slopeScaledBias) = 0;
		virtual void SetDepthTest(CompareMode mode) = 0;
		virtual void SetDepthWrite(bool enable) = 0;
		virtual void SetFillMode(FillMode mode) = 0;
		virtual void SetScissorTest(bool enable, const glm::vec2& rect) = 0;
		virtual void SetStencilTest(bool enable, CompareMode mode = CMP_ALWAYS, StencilOp pass = OP_KEEP, StencilOp fail = OP_KEEP, StencilOp zFail = OP_KEEP, unsigned stencilRef = 0, unsigned compareMask = UINT32_MAX, unsigned writeMask = UINT32_MAX) = 0;

		virtual void SetTexture(unsigned index, Texture2D* texture) = 0;

		virtual void SetRenderTarget(unsigned index, RenderTarget* renderTarget) = 0;
		virtual void SetDepthStencil(RenderTarget* depthStencil) = 0;

		virtual void SetViewport(const glm::vec4& rect) = 0;

		virtual void Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount) = 0;
		virtual void DrawIndexed(PrimitiveType type, unsigned indexStart, unsigned indexCount) = 0;
		virtual void DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned instanceCount) = 0;

		virtual SDL_Window *GetWindow() = 0;

		virtual void ImGuiNewFrame() = 0; // Called before ImGui commands
		virtual void ImGuiEndFrame() = 0; // Called after ImGui commands

		inline virtual bool IsInitialized() const { return isInitialized; }

		// For Profiler
		inline unsigned getPrimitiveCount() { return primitiveCount; } // Reset to 0 every frame.
		inline unsigned getDrawCount() { return drawCount; }
		inline unsigned getDrawIndexedCount() { return drawIndexedCount; }
		inline unsigned getDrawInstancedCount() { return drawInstancedCount; }
		inline unsigned getTotalDrawCallCount() { return drawCount + drawIndexedCount + drawInstancedCount; }

		virtual RendererType getRendererType() = 0;
		virtual void WaitForDevice() = 0;

	protected:
		// For Profiler
		unsigned primitiveCount;
		unsigned drawCount;
		unsigned drawIndexedCount;
		unsigned drawInstancedCount;

		bool isInitialized;
	};

}

#endif
