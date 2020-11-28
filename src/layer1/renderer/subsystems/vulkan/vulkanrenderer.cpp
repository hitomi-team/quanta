#include "pch/pch.h"

#include "vulkanrenderer.h"

namespace Renderer {

	VulkanRenderer::VulkanRenderer()
	{
		ResetCache();
	}

	bool VulkanRenderer::SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample)
	{
		(void)vsync;
		(void)multisample;

		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			global_log.Fatal("couldn't init SDL2!");
		}

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

		// implement instance

		return true;
	}

	bool VulkanRenderer::BeginFrame()
	{
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
			return false;

		return true;
	}

	void VulkanRenderer::EndFrame()
	{
	}

	void VulkanRenderer::Clear(unsigned flags, const glm::vec4& color, float depth, unsigned stencil)
	{
		(void)flags;
		(void)depth;
		(void)stencil;
		(void)color;
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


	VertexBuffer* VulkanRenderer::CreateVertexBuffer(const Vertex *vertices, unsigned count)
	{
		(void)vertices;
		(void)count;
		return nullptr;
	}

	IndexBuffer* VulkanRenderer::CreateIndexBuffer(const uint16_t *indices, unsigned count)
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
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
	}

	void VulkanRenderer::ImGuiEndFrame()
	{
		ImGui::End();
		ImGui::Render();
		//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), this->surface.getPresentCommandBuffer());
	}

	RendererType VulkanRenderer::getRendererType()
	{
		return RENDERER_VULKAN;
	}

	void VulkanRenderer::InitializeImGui(int width, int height)
	{
		(void)width; (void)height;
#if 0
		VkAttachmentDescription attachment = {};
		attachment.format = this->surface.getSurfaceFormat().format;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachment = {};
		colorAttachment.attachment = 0;
		colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachment;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &attachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependency;

		VK_ASSERT(this->device.fn.vkCreateRenderPass(this->device.get(), &renderPassCreateInfo, nullptr, &this->imGuiRenderPass), "Failed to create ImGui render pass");

		VkDescriptorPoolSize poolSizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.maxSets = 16;
		descriptorPoolCreateInfo.poolSizeCount = ARRAY_SIZE(poolSizes);
		descriptorPoolCreateInfo.pPoolSizes = poolSizes;

		VK_ASSERT(this->device.fn.vkCreateDescriptorPool(this->device.get(), &descriptorPoolCreateInfo, nullptr, &this->imGuiDescriptorPool), "Failed to create ImGui descriptor pool");

		VkPipelineCacheCreateInfo cacheCreateInfo = {};
		cacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

		VK_ASSERT(this->device.fn.vkCreatePipelineCache(this->device.get(), &cacheCreateInfo, nullptr, &this->imGuiPipelineCache), "Failed to create ImGui pipeline cache");

		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = this->instance.get();
		initInfo.PhysicalDevice = this->device.getPhysicalDev();
		initInfo.Device = this->device.get();
		initInfo.QueueFamily = this->device.getQueueFamilyIndices()[0];
		initInfo.Queue = this->device.getGraphicsQueue();
		initInfo.PipelineCache = this->imGuiPipelineCache;
		initInfo.DescriptorPool = this->imGuiDescriptorPool;
		initInfo.MinImageCount = this->surface.getMinNumImages();
		initInfo.ImageCount = this->surface.getNumImages();
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().DisplaySize.x = width;
		ImGui::GetIO().DisplaySize.y = height;
		ImGui::GetIO().Fonts->AddFontDefault();
		ImGui::GetIO().Fonts->Build();
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForVulkan(window);
		ImGui_ImplVulkan_Init(&initInfo, this->imGuiRenderPass);

		VkCommandBufferAllocateInfo cmdbufinfo = {};
		cmdbufinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdbufinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdbufinfo.commandPool = this->device.getGraphicsCommandPool();
		cmdbufinfo.commandBufferCount = 1;

		VkCommandBuffer cmdbuf;
		VK_ASSERT(this->device.fn.vkAllocateCommandBuffers(this->device.get(), &cmdbufinfo, &cmdbuf), "Failed to allocate command buffer for transfer operation")

		VkCommandBufferBeginInfo begininfo = {};
		begininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		this->device.fn.vkBeginCommandBuffer(cmdbuf, &begininfo);

		// VK COMMANDS START

		ImGui_ImplVulkan_CreateFontsTexture(cmdbuf);

		// VK COMMANDS END

		this->device.fn.vkEndCommandBuffer(cmdbuf);

		VkSubmitInfo submitinfo = {};
		submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitinfo.commandBufferCount = 1;
		submitinfo.pCommandBuffers = &cmdbuf;

		VkFence fence;
		VkFenceCreateInfo fenceinfo = {};
		fenceinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceinfo.flags = 0;
		this->device.fn.vkCreateFence(this->device.get(), &fenceinfo, nullptr, &fence);

		this->device.fn.vkQueueSubmit(this->device.getGraphicsQueue(), 1, &submitinfo, fence);
		this->device.fn.vkWaitForFences(this->device.get(), 1, &fence, VK_TRUE, UINT64_MAX);

		this->device.fn.vkDestroyFence(this->device.get(), fence, nullptr);
		this->device.fn.vkFreeCommandBuffers(this->device.get(), this->device.getGraphicsCommandPool(), 1, &cmdbuf);
#endif
	}

	void VulkanRenderer::CloseImGui()
	{
#if 0
		ImGui_ImplVulkan_Shutdown();
		this->device.fn.vkDestroyPipelineCache(this->device.get(), this->imGuiPipelineCache, nullptr);
		this->device.fn.vkDestroyDescriptorPool(this->device.get(), this->imGuiDescriptorPool, nullptr);
		this->device.fn.vkDestroyRenderPass(this->device.get(), this->imGuiRenderPass, nullptr);
#endif
	}
}
