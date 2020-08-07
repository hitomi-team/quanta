#ifndef RENDERER_VULKANRENDERER_H
#define RENDERER_VULKANRENDERER_H

#include "../required.h"
#include "vulkanrequired.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../imgui/imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"

#include "vk_instance.h"
#include "vk_device.h"
#include "vk_surface.h"

namespace Renderer {

	class VulkanRenderer : public RHI {
	public:
		VulkanRenderer();
		virtual ~VulkanRenderer() {}
		
		bool SetGraphicsMode(int width = WINDOW_DEFAULT_WIDTH, int height = WINDOW_DEFAULT_HEIGHT, bool fullscreen = false, bool borderless = false, bool resizable = false, bool vsync = true, int multisample = 0);
		void Clear(unsigned flags, const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), float depth = 1.0f, unsigned stencil = 0);

// TO BE IMPLEMENTED

		bool BeginFrame(); // true = continue rendering, false = dont render
		void EndFrame();

		bool IsInitialized() { return isInitialized; }


//		glm::vec2 GetRenderTargetDimensions() const { return glm::vec2(0, 0); }
//		void CreateRendererCapabilities() { };
//		std::vector<int> GetMultiSampleLevels() const { std::vector<int> a; return a; };

		void Close();

//		void AddGpuResource(GPUResource* object) { };
//		void RemoveGpuResource(GPUResource* object) { };

		void ResetCache();

		bool IsDeviceLost();

//		VertexBuffer* CreateVertexBuffer(Vertex *vertices, unsigned count);
//		IndexBuffer* CreateIndexBuffer(unsigned *indices, unsigned count);
//		InputLayout* CreateInputLayout(Shader* vertexShader, VertexBuffer** buffers, unsigned* elementMasks) { return 0; }

		
//		void SetShaders(Shader shader);
//		void SetShaderParameter(unsigned param, const float* data, unsigned count) { }
//		void SetShaderParameter(unsigned param, float value) { };
//		void SetShaderParameter(unsigned param, bool value) { };
//		void SetShaderParameter(unsigned param, const glm::vec2& vector) { };
//		void SetShaderParameter(unsigned param, const glm::mat3& matrix) { };
//		void SetShaderParameter(unsigned param, const glm::vec3& vector) { };
//		void SetShaderParameter(unsigned param, const glm::mat4& matrix) { };
//		void SetShaderParameter(unsigned param, const glm::vec4& vector) { };

//		void SetVertexBuffer(VertexBuffer* buffer);
//		void SetIndexBuffer(IndexBuffer* buffer) { };
//		bool SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, const std::vector<unsigned>& elementMasks, unsigned instanceOffset = 0) { return true; };

//		bool NeedParameterUpdate(ShaderParameterGroup group, const void* source) { return true; };
//		void SetFlushGPU(bool flushGpu) { };
//		void SetBlendMode(BlendMode mode);
//		void SetColorWrite(bool enable);
//		void SetCullMode(CullMode mode);
//		void SetDepthBias(float constantBias, float slopeScaledBias);
//		void SetDepthTest(CompareMode mode);
//		void SetDepthWrite(bool enable);
//		void SetFillMode(FillMode mode);
//		void SetScissorTest(bool enable, const glm::vec2& rect) { };
//		void SetStencilTest(bool enable, CompareMode mode = CMP_ALWAYS, StencilOp pass = OP_KEEP, StencilOp fail = OP_KEEP, StencilOp zFail = OP_KEEP, unsigned stencilRef = 0, unsigned compareMask = UINT32_MAX, unsigned writeMask = UINT32_MAX) { };	

//		void SetTexture(unsigned index, Texture* texture) { };

//		void SetRenderTarget(unsigned index, RenderTarget* renderTarget) { };
//		void SetDepthStencil(RenderTarget* depthStencil) { };

//		void SetViewport(const glm::vec4& rect);

//		void Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount);
//		void Draw(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount);
//		void DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount, unsigned instanceCount);

//		void ClearParameterSource(ShaderParameterGroup group) { };
//		void ClearParameterSources() { };
//		void ClearTransformSources() { };
//		void CleanupShaderPrograms(Shader* variation) { };

		SDL_Window *GetWindow() { return window; }
		
		void ImGuiNewFrame();
		void ImGuiEndFrame();

	private:
		SDL_Window *window;
		SDL_DisplayMode display;
		bool isInitialized;

		// Internal cache
		VulkanInstance instance;
		VulkanDevice device;
		VulkanSurface surface;

		// internal functions
		bool UpdateSwapchain(int width, int height, int multisample);
		void PreDraw();
	};

}

#endif
