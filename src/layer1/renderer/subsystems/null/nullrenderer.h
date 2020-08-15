#ifndef RENDERER_NULLRENDERER_H
#define RENDERER_NULLRENDERER_H

// Renderer Handler Implementation

#include "layer1/renderer/subsystems/required.h"

namespace Renderer {

	class NullRenderer : public RHI {
	public:
		inline NullRenderer()
			: primitiveCount{}
			, drawCount{}
			, drawIndexedCount{}
			, drawInstancedCount{}
			, isInitialized{ true }
		{}

		inline ~NullRenderer() {}

		bool SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample);
		bool BeginFrame();
		void EndFrame();
		void Clear(unsigned flags, const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), float depth = 1.0f, unsigned stencil = 0);

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

		void SetTexture(unsigned index, Texture2D* texture);

		void SetRenderTarget(unsigned index, RenderTarget* renderTarget);
		void SetDepthStencil(RenderTarget* depthStencil);

		void SetViewport(const glm::vec4& rect);

		void Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount);
		void DrawIndexed(PrimitiveType type, unsigned indexStart, unsigned indexCount);
		void DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned instanceCount);

		SDL_Window *GetWindow();

		void ImGuiNewFrame(); // Called before ImGui commands
		void ImGuiEndFrame(); // Called after ImGui commands

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
