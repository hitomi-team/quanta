#include "pch/pch.h"

#include "vulkanrenderer.h"

namespace Renderer {

	VulkanRenderer::VulkanRenderer()
	{
		SDL_Init(SDL_INIT_VIDEO);
		ResetCache();
	}

	bool VulkanRenderer::SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample)
	{
		(void)vsync;
		(void)multisample;

		if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO)
			SDL_Init(SDL_INIT_VIDEO);

		int x = fullscreen ? 0 : SDL_WINDOWPOS_CENTERED;
		int y = fullscreen ? 0 : SDL_WINDOWPOS_CENTERED;

		unsigned flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

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

		// init device and other stuff
		if (!instance.Load(window))
			return false;

		device = QueryDevice(instance);
		surface.Load(window, instance, device);
		surface.Clear(0.0, 0.0, 0.0, 0.0);
		surface.Present();

		// init imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().DisplaySize.x = width;
		ImGui::GetIO().DisplaySize.y = height;
		ImGui::GetIO().Fonts->AddFontDefault();
		ImGui::GetIO().Fonts->Build();
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForVulkan(window);

		return true;
	}

	bool VulkanRenderer::BeginFrame()
	{
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
			return false;

		// do prerender stuff here

		return true;
	}

	void VulkanRenderer::EndFrame()
	{
		surface.Present();
		// present swapchain
	}

	void VulkanRenderer::Clear(unsigned flags, const glm::vec4& color, float depth, unsigned stencil)
	{
		(void)flags;
		(void)depth;
		(void)stencil;

		surface.Clear(color.r, color.g, color.b, color.a);
	}


	glm::vec2 VulkanRenderer::GetRenderTargetDimensions()
	{
		return glm::vec2(0, 0);
	}

	void VulkanRenderer::CreateRendererCapabilities()
	{
	}

	std::vector<int> VulkanRenderer::GetMultiSampleLevels()
	{
		std::vector< int > a;
		return a;
	}


	void VulkanRenderer::Close()
	{
//		ImGui_ImplVulkan_Shutdown()

		// vulkan object releasing stuff
		surface.Release(instance);
		device.Release();
		instance.Release();

		ResetCache(); // just in case we want to initialize this renderer again

		SDL_ShowCursor(SDL_TRUE);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}


	void VulkanRenderer::AddGpuResource(GPUResource* object)
	{
		(void)object;
	}

	void VulkanRenderer::RemoveGpuResource(GPUResource* object)
	{
		(void)object;
	}


	void VulkanRenderer::ResetCache()
	{

	}


	bool VulkanRenderer::IsDeviceLost()
	{
		return false;
	}


	VertexBuffer* VulkanRenderer::CreateVertexBuffer(Vertex *vertices, unsigned count)
	{
		(void)vertices;
		(void)count;
		return nullptr;
	}

	IndexBuffer* VulkanRenderer::CreateIndexBuffer(unsigned *indices, unsigned count)
	{
		(void)indices;
		(void)count;
		return nullptr;
	}

	InputLayout* VulkanRenderer::CreateInputLayout(unsigned char *vsbytecode, unsigned vsbytecodelen)
	{
		(void)vsbytecode;
		(void)vsbytecodelen;
		return nullptr;
	}

	Shader *VulkanRenderer::CreateShader(unsigned char *vs_bytecode, unsigned int vs_size,
				     unsigned char *fs_bytecode, unsigned int fs_size)
	{
		(void)vs_bytecode;
		(void)vs_size;
		(void)fs_bytecode;
		(void)fs_size;
		return nullptr;
	}

	Texture2D *VulkanRenderer::CreateTexture2D(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc)
	{
		(void)data;
		(void)width;
		(void)height;
		(void)samplerstatedesc;
		return nullptr;
	}

	ShaderParameterBuffer *VulkanRenderer::CreateShaderParameterBuffer(std::vector<ShaderParameterElement> elements)
	{
		(void)elements;
		return nullptr;
	}

	void VulkanRenderer::SetShaders(Shader *shader)
	{
		(void)shader;
	}

	void VulkanRenderer::SetVertexBuffer(VertexBuffer* buffer)
	{
		(void)buffer;
	}

	void VulkanRenderer::SetIndexBuffer(IndexBuffer* buffer)
	{
		(void)buffer;
	}

	bool VulkanRenderer::SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, const std::vector<unsigned>& elementMasks, unsigned instanceOffset)
	{
		(void)buffers;
		(void)elementMasks;
		(void)instanceOffset;
		return true;
	}

	void VulkanRenderer::SetFlushGPU(bool flushGpu)
	{
		(void)flushGpu;
	}

	void VulkanRenderer::SetBlendMode(BlendMode mode)
	{
		(void)mode;
	}

	void VulkanRenderer::SetColorWrite(bool enable)
	{
		(void)enable;
	}

	void VulkanRenderer::SetCullMode(CullMode mode)
	{
		(void)mode;
	}

	void VulkanRenderer::SetDepthBias(float constantBias, float slopeScaledBias)
	{
		(void)constantBias;
		(void)slopeScaledBias;
	}

	void VulkanRenderer::SetDepthTest(CompareMode mode)
	{
		(void)mode;
	}

	void VulkanRenderer::SetDepthWrite(bool enable)
	{
		(void)enable;
	}

	void VulkanRenderer::SetFillMode(FillMode mode)
	{
		(void)mode;
	}

	void VulkanRenderer::SetScissorTest(bool enable, const glm::vec2& rect)
	{
		(void)enable;
		(void)rect;
	}

	void VulkanRenderer::SetStencilTest(bool enable, CompareMode mode, StencilOp pass, StencilOp fail, StencilOp zFail, unsigned stencilRef, unsigned compareMask, unsigned writeMask)
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


	void VulkanRenderer::SetTexture(unsigned index, Texture2D* texture)
	{
		(void)index;
		(void)texture;
	}


	void VulkanRenderer::SetRenderTarget(unsigned index, RenderTarget* renderTarget)
	{
		(void)index;
		(void)renderTarget;
	}

	void VulkanRenderer::SetDepthStencil(RenderTarget* depthStencil)
	{
		(void)depthStencil;
	}


	void VulkanRenderer::SetViewport(const glm::vec4& rect)
	{
		(void)rect;
	}


	void VulkanRenderer::Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount)
	{
		(void)type;
		(void)vertexStart;
		(void)vertexCount;
	}

	void VulkanRenderer::DrawIndexed(PrimitiveType type, unsigned indexStart, unsigned indexCount)
	{
		(void)type;
		(void)indexStart;
		(void)indexCount;
	}

	void VulkanRenderer::DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned instanceCount)
	{
		(void)type;
		(void)indexStart;
		(void)indexCount;
		(void)instanceCount;
	}


	void VulkanRenderer::ImGuiNewFrame()
	{
//		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
	}

	void VulkanRenderer::ImGuiEndFrame()
	{
		ImGui::End();
		ImGui::Render();
//		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
	}

}
