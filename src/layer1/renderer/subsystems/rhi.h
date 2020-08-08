#ifndef RENDERER_RHI_H
#define RENDERER_RHI_H

// Renderer Handler Implementation

#include "required.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Renderer {

	class RHI {
	public:
		RHI() {}
		virtual ~RHI() {}
		
		virtual bool SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample) { return true; }
		virtual bool BeginFrame() { return true; };
		virtual void EndFrame() { };
		virtual void Clear(unsigned flags, const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), float depth = 1.0f, unsigned stencil = 0) { };

		virtual bool IsInitialized() { return isInitialized; }


		virtual glm::vec2 GetRenderTargetDimensions() const { return glm::vec2(0, 0); }
		virtual void CreateRendererCapabilities() { };
		virtual std::vector<int> GetMultiSampleLevels() const { std::vector<int> a; return a; };

		virtual void Close() { };

		virtual void AddGpuResource(GPUResource* object) { };
		virtual void RemoveGpuResource(GPUResource* object) { };

		virtual void ResetCache() { };

		virtual bool IsDeviceLost() const { return false; } //Always false on Direct3D


		virtual VertexBuffer* CreateVertexBuffer(Vertex *vertices, unsigned count) { return 0; }
		virtual IndexBuffer* CreateIndexBuffer(unsigned *indices, unsigned count) { return 0; }
		virtual InputLayout* CreateInputLayout(unsigned char *vsbytecode, unsigned vsbytecodelen) { return 0; }
		virtual Shader *CreateShader(unsigned char *vs_bytecode, unsigned int vs_size,
					     unsigned char *fs_bytecode, unsigned int fs_size) { return 0; }
		Texture2D *CreateTexture2D(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc) { return 0; }
		
		virtual void SetShaders(Shader *shader) { };
		virtual void SetShaderParameter(unsigned param, const float* data, unsigned count) { }
		virtual void SetShaderParameter(unsigned param, float value) { };
		virtual void SetShaderParameter(unsigned param, bool value) { };
		virtual void SetShaderParameter(unsigned param, const glm::vec2& vector) { };
		virtual void SetShaderParameter(unsigned param, const glm::mat3& matrix) { };
		virtual void SetShaderParameter(unsigned param, const glm::vec3& vector) { };
		virtual void SetShaderParameter(unsigned param, const glm::mat4& matrix) { };
		virtual void SetShaderParameter(unsigned param, const glm::vec4& vector) { };

		virtual void SetVertexBuffer(VertexBuffer* buffer) { };
		virtual void SetIndexBuffer(IndexBuffer* buffer) { };
		virtual bool SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, const std::vector<unsigned>& elementMasks, unsigned instanceOffset = 0) { return true; };

		virtual bool NeedParameterUpdate(ShaderParameterGroup group, const void* source) { return true; };
		virtual void SetFlushGPU(bool flushGpu) { };
		virtual void SetBlendMode(BlendMode mode) { };
		virtual void SetColorWrite(bool enable) { };
		virtual void SetCullMode(CullMode mode) { };
		virtual void SetDepthBias(float constantBias, float slopeScaledBias) { };
		virtual void SetDepthTest(CompareMode mode) { };
		virtual void SetDepthWrite(bool enable) { };
		virtual void SetFillMode(FillMode mode) { };
		virtual void SetScissorTest(bool enable, const glm::vec2& rect) { };
		virtual void SetStencilTest(bool enable, CompareMode mode = CMP_ALWAYS, StencilOp pass = OP_KEEP, StencilOp fail = OP_KEEP, StencilOp zFail = OP_KEEP, unsigned stencilRef = 0, unsigned compareMask = UINT32_MAX, unsigned writeMask = UINT32_MAX) { };	

		virtual void SetTexture(unsigned index, Texture2D* texture) { };

		virtual void SetRenderTarget(unsigned index, RenderTarget* renderTarget) { };
		virtual void SetDepthStencil(RenderTarget* depthStencil) { };

		virtual void SetViewport(const glm::vec4& rect) { };

		virtual void Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount) { };
		virtual void DrawIndexed(PrimitiveType type, unsigned indexStart, unsigned indexCount) { };
		virtual void DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned instanceCount) { };

		virtual void ClearParameterSource(ShaderParameterGroup group) { };
		virtual void ClearParameterSources() { };
		virtual void ClearTransformSources() { };
		virtual void CleanupShaderPrograms(Shader* variation) { };

		virtual SDL_Window *GetWindow() { return nullptr; }

		virtual void ImGuiNewFrame() { }; // Called before ImGui commands
		virtual void ImGuiEndFrame() { }; // Called after ImGui commands

		// For Profiler
		unsigned getPrimitiveCount() { return primitiveCount; } // Reset to 0 every frame.
		unsigned getDrawCount() { return drawCount; }
		unsigned getDrawIndexedCount() { return drawIndexedCount; }
		unsigned getDrawInstancedCount() { return drawInstancedCount; }
		unsigned getTotalDrawCallCount() { return drawCount + drawIndexedCount + drawInstancedCount; }

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
