#ifndef NULL_RENDERIMP_H
#define NULL_RENDERIMP_H

#include "../required.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Renderer {

	class NullRendererImp : public RHI {
	public:
		NullRendererImp();
		virtual ~NullRendererImp() {}
		
		bool SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample) { return true; }
		bool BeginFrame() { return true; };
		void EndFrame() { };
		void Clear(unsigned flags, const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), float depth = 1.0f, unsigned stencil = 0) { };

		bool IsInitialized() { return true; }


		glm::vec2 GetRenderTargetDimensions() const { return glm::vec2(0, 0); }
		void CreateRendererCapabilities() { };
		std::vector<int> GetMultiSampleLevels() const { std::vector<int> a; return a; };


		void Close() { };
		void AdjustWindow(int& newWidth, int& newHeight, bool& newFullscreen, bool& newBorderless) { };
		bool OpenWindow(int width, int height, bool resizable, bool borderless) { return true; };
		void Maximize() { };
		void SetWindowPos(const glm::vec2& pos) { };
		void SetWindowTitle(const std::string& string) { };

		void AddGpuResource(GPUResource* object) { };
		void RemoveGpuResource(GPUResource* object) { };

		void ResetCache() { };

		bool IsDeviceLost() const { return false; } //Always false on Direct3D


		VertexBuffer* CreateVertexBuffer() { return 0; }
		IndexBuffer* CreateIndexBuffer() { return 0; }
		InputLayout* CreateInputLayout(Shader* vertexShader, VertexBuffer** buffers, unsigned* elementMasks) { return 0; }

		
		void SetShaders(std::string vs, std::string fs) { };
		void SetShaderParameter(unsigned param, const float* data, unsigned count) { }
		void SetShaderParameter(unsigned param, float value) { };
		void SetShaderParameter(unsigned param, bool value) { };
		void SetShaderParameter(unsigned param, const glm::vec2& vector) { };
		void SetShaderParameter(unsigned param, const glm::mat3& matrix) { };
		void SetShaderParameter(unsigned param, const glm::vec3& vector) { };
		void SetShaderParameter(unsigned param, const glm::mat4& matrix) { };
		void SetShaderParameter(unsigned param, const glm::vec4& vector) { };

		void SetVertexBuffer(VertexBuffer* buffer) { };
		void SetIndexBuffer(IndexBuffer* buffer) { };
		bool SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, const std::vector<unsigned>& elementMasks, unsigned instanceOffset = 0) { return true; };

		bool NeedParameterUpdate(ShaderParameterGroup group, const void* source) { return true; };
		void SetFlushGPU(bool flushGpu) { };
		void SetBlendMode(BlendMode mode) { };
		void SetColorWrite(bool enable) { };
		void SetCullMode(CullMode mode) { };
		void SetDepthBias(float constantBias, float slopeScaledBias) { };
		void SetDepthTest(CompareMode mode) { };
		void SetDepthWrite(bool enable) { };
		void SetFillMode(FillMode mode) { };
		void SetScissorTest(bool enable, const glm::vec2& rect) { };
		void SetStencilTest(bool enable, CompareMode mode = CMP_ALWAYS, StencilOp pass = OP_KEEP, StencilOp fail = OP_KEEP, StencilOp zFail = OP_KEEP, unsigned stencilRef = 0, unsigned compareMask = UINT32_MAX, unsigned writeMask = UINT32_MAX) { };	

		void SetTexture(unsigned index, Texture* texture) { };

		void SetRenderTarget(unsigned index, RenderTarget* renderTarget) { };
		void SetDepthStencil(RenderTarget* depthStencil) { };

		void SetViewport(const glm::vec4& rect) { };

		void Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount) { };
		void Draw(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount) { };
		void DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount, unsigned instanceCount) { };

		void ClearParameterSource(ShaderParameterGroup group) { };
		void ClearParameterSources() { };
		void ClearTransformSources() { };
		void CleanupShaderPrograms(Shader* variation) { };
	};

}

#endif
