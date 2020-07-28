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
		virtual void AdjustWindow(int& newWidth, int& newHeight, bool& newFullscreen, bool& newBorderless) { };
		virtual bool OpenWindow(int width, int height, bool resizable, bool borderless) { return true; };
		virtual void Maximize() { };
		virtual void SetWindowPos(const glm::vec2& pos) { };
		virtual void SetWindowTitle(const std::string& string) { };

		virtual void AddGpuResource(GPUResource* object) { };
		virtual void RemoveGpuResource(GPUResource* object) { };

		virtual void ResetCache() { };

		virtual bool IsDeviceLost() const { return false; } //Always false on Direct3D


		virtual VertexBuffer* CreateVertexBuffer() { return 0; }
		virtual IndexBuffer* CreateIndexBuffer() { return 0; }
		virtual InputLayout* CreateInputLayout(Shader* vertexShader, VertexBuffer** buffers, unsigned* elementMasks) { return 0; }

		
		virtual void SetShaders(std::string vs, std::string fs) { };
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

		virtual void SetTexture(unsigned index, Texture* texture) { };

		virtual void SetRenderTarget(unsigned index, RenderTarget* renderTarget) { };
		virtual void SetDepthStencil(RenderTarget* depthStencil) { };

		virtual void SetViewport(const glm::vec4& rect) { };

		virtual void Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount) { };
		virtual void Draw(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount) { };
		virtual void DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount, unsigned instanceCount) { };

		virtual void ClearParameterSource(ShaderParameterGroup group) { };
		virtual void ClearParameterSources() { };
		virtual void ClearTransformSources() { };
		virtual void CleanupShaderPrograms(Shader* variation) { };

		virtual SDL_Window *GetWindow() { return nullptr; }

	protected:
		bool isInitialized;
	};

}

#endif
