#include "pch/pch.h"

#include "vulkanrenderer.h"

namespace Renderer {

	VulkanRenderer *g_vulkanRenderer = nullptr;

	VulkanRenderer::VulkanRenderer()
	{
		ResetCache();
	}

	bool VulkanRenderer::SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample)
	{
		(void)vsync;
		(void)multisample;

		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			global_log.Fatal("cannot init SDL2 Vulkan");
			return false;
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
		if (!this->ctx.Init(this->window))
			return false;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().DisplaySize.x = width;
		ImGui::GetIO().DisplaySize.y = height;
		ImGui::GetIO().Fonts->AddFontDefault();
		ImGui::GetIO().Fonts->Build();
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForVulkan(window);

		this->ImGuiInit();

		g_vulkanRenderer = this;

		this->viewport = { 0, (float)this->ctx.swapchain_extent.height, (float)this->ctx.swapchain_extent.width, -(float)this->ctx.swapchain_extent.height, 0.0f, 1.0f };
		this->scissor.offset = {};
		this->scissor.extent = this->ctx.swapchain_extent;

		return true;
	}

	void VulkanRenderer::PreDraw()
	{
		if (textureviews_dirty) {
			textureviews_dirty = false;
			for (auto &i : this->parameter_bufs)
				i->Update();
		}
	}

	bool VulkanRenderer::BeginFrame()
	{
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
			return false;

		this->PreDraw();

		VkResult res = vkAcquireNextImageKHR(this->ctx.device, this->ctx.swapchain, UINT64_MAX, this->ctx.swapchain_sync[this->ctx.current_image].wait_sync, VK_NULL_HANDLE, &this->ctx.acquire_image);
		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_SURFACE_LOST_KHR) {
			this->WaitForDevice();
			this->ctx.CloseSwapchain();
			this->ctx.InitSwapchain();
			this->SetViewport(glm::vec4(0, 0, static_cast< float >(this->ctx.swapchain_extent.width), static_cast< float >(this->ctx.swapchain_extent.height)));
			this->scissor.extent = this->ctx.swapchain_extent;

			return false;
		}

		vkWaitForFences(this->ctx.device, 1, &this->ctx.swapchain_sync[this->ctx.current_image].fence, VK_TRUE, UINT64_MAX);
		vkResetFences(this->ctx.device, 1, &this->ctx.swapchain_sync[this->ctx.current_image].fence);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		static const VkClearValue default_clear = {};

		VkRenderPassBeginInfo renderPassBegin = {};
		renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBegin.renderPass = this->ctx.swapchain_renderpass;
		renderPassBegin.framebuffer = this->ctx.swapchain_framebuffers[this->ctx.current_image];
		renderPassBegin.renderArea.extent = this->ctx.swapchain_extent;
		renderPassBegin.clearValueCount = 1;
		renderPassBegin.pClearValues = &default_clear;

		vkResetCommandPool(this->ctx.device, this->ctx.swapchain_command_pool[this->ctx.current_image], 0);
		vkBeginCommandBuffer(this->ctx.swapchain_command_bufs[this->ctx.current_image], &beginInfo);

		VkImageMemoryBarrier swapchainImageBarrier = {};
		swapchainImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		swapchainImageBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		swapchainImageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		swapchainImageBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		swapchainImageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		swapchainImageBarrier.srcQueueFamilyIndex = this->ctx.present_queue;
		swapchainImageBarrier.dstQueueFamilyIndex = this->ctx.queue_family_indices[0];
		swapchainImageBarrier.image = this->ctx.swapchain_images[this->ctx.current_image];
		swapchainImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		swapchainImageBarrier.subresourceRange.baseMipLevel = 0;
		swapchainImageBarrier.subresourceRange.layerCount = 1;
		swapchainImageBarrier.subresourceRange.baseArrayLayer = 0;
		swapchainImageBarrier.subresourceRange.levelCount = 1;

		vkCmdPipelineBarrier(this->ctx.swapchain_command_bufs[this->ctx.current_image], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &swapchainImageBarrier);

		vkCmdBeginRenderPass(this->ctx.swapchain_command_bufs[this->ctx.current_image], &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdSetViewport(this->ctx.swapchain_command_bufs[this->ctx.current_image], 0, 1, &this->viewport);
		vkCmdSetScissor(this->ctx.swapchain_command_bufs[this->ctx.current_image], 0, 1, &this->scissor);

		return true;
	}

	void VulkanRenderer::EndFrame()
	{
		vkCmdEndRenderPass(this->ctx.swapchain_command_bufs[this->ctx.current_image]);

		VkImageMemoryBarrier swapchainImageBarrier = {};
		swapchainImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		swapchainImageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		swapchainImageBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		swapchainImageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		swapchainImageBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		swapchainImageBarrier.srcQueueFamilyIndex = this->ctx.queue_family_indices[0];
		swapchainImageBarrier.dstQueueFamilyIndex = this->ctx.present_queue;
		swapchainImageBarrier.image = this->ctx.swapchain_images[this->ctx.current_image];
		swapchainImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		swapchainImageBarrier.subresourceRange.baseMipLevel = 0;
		swapchainImageBarrier.subresourceRange.layerCount = 1;
		swapchainImageBarrier.subresourceRange.baseArrayLayer = 0;
		swapchainImageBarrier.subresourceRange.levelCount = 1;

		vkCmdPipelineBarrier(this->ctx.swapchain_command_bufs[this->ctx.current_image], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &swapchainImageBarrier);

		vkEndCommandBuffer(this->ctx.swapchain_command_bufs[this->ctx.current_image]);

		VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &this->ctx.swapchain_sync[this->ctx.current_image].wait_sync;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &this->ctx.swapchain_sync[this->ctx.current_image].wake_sync;
		submitInfo.pWaitDstStageMask = &wait_stages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &this->ctx.swapchain_command_bufs[this->ctx.current_image];


		VK_ASSERT(vkQueueSubmit(this->ctx.queues[0], 1, &submitInfo, this->ctx.swapchain_sync[this->ctx.current_image].fence), "Failed to submit to graphics queue");


		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &this->ctx.swapchain_sync[this->ctx.current_image].wake_sync;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &this->ctx.swapchain;
		presentInfo.pImageIndices = &this->ctx.acquire_image;

		VkResult res = vkQueuePresentKHR(this->ctx.queues[this->ctx.present_queue], &presentInfo);
		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_SURFACE_LOST_KHR) {
			this->WaitForDevice();
			this->ctx.CloseSwapchain();
			this->ctx.InitSwapchain();
			this->SetViewport(glm::vec4(0, 0, static_cast< float >(this->ctx.swapchain_extent.width), static_cast< float >(this->ctx.swapchain_extent.height)));
			this->scissor.extent = this->ctx.swapchain_extent;

			return;
		}

		this->ctx.current_image = (this->ctx.current_image + 1) % this->ctx.num_swapchain_images;
	}

	void VulkanRenderer::Clear(unsigned flags, const glm::vec4& color, float depth, unsigned stencil)
	{
		VkClearValue values[2];
		values[0].color = { color.r, color.g, color.b, color.a };
		values[1].depthStencil.depth = depth;
		values[1].depthStencil.stencil = stencil;

		VkRect2D rect = {};
		rect.extent = this->ctx.swapchain_extent;

		VkClearRect clearRect;
		clearRect.rect = rect;
		clearRect.baseArrayLayer = 0;
		clearRect.layerCount = 1;

		if (flags & CLEAR_COLOR) {
			VkClearAttachment attachment;
			attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			attachment.colorAttachment = 0;
			attachment.clearValue = values[0];
			vkCmdClearAttachments(this->ctx.swapchain_command_bufs[this->ctx.current_image], 1, &attachment, 1, &clearRect);
		}

		if (flags & CLEAR_DEPTH) {
			VkClearAttachment attachment;
			attachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			attachment.clearValue = values[1];
			vkCmdClearAttachments(this->ctx.swapchain_command_bufs[this->ctx.current_image], 1, &attachment, 1, &clearRect);
		}

		if (flags & CLEAR_STENCIL) {
			VkClearAttachment attachment;
			attachment.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
			attachment.clearValue = values[1];
			vkCmdClearAttachments(this->ctx.swapchain_command_bufs[this->ctx.current_image], 1, &attachment, 1, &clearRect);
		}
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
		this->WaitForDevice();

		this->ImGuiClose();

		this->ctx.Close();
		this->ctx.ResetCache();

		this->ResetCache(); // just in case we want to initialize this renderer again

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
		size_t i;

		this->rasterizerstate_dirty = false;
		this->textureviews_dirty = false;

		this->current_shader = nullptr;
		this->current_vbo = nullptr;
		this->current_ibo = nullptr;

		for (i = 0; i < MAX_RENDERTARGETS; i++)
			this->rendertargets[i] = nullptr;

		for (i = 0; i < MAX_TEXTURE_UNITS; i++)
			this->textureviews[i] = nullptr;
	}


	bool VulkanRenderer::IsDeviceLost()
	{
		return false;
	}


	VertexBuffer* VulkanRenderer::CreateVertexBuffer(const Vertex *vertices, unsigned count)
	{
		if (vertices == nullptr || count == 0)
			return nullptr;

		auto out = new CVulkanVertexBuffer;
		if (!out->SetData(vertices, count)) {
			delete out;
			return nullptr;
		}

		return out;
	}

	IndexBuffer* VulkanRenderer::CreateIndexBuffer(const uint16_t *indices, unsigned count)
	{
		if (indices == nullptr || count == 0)
			return nullptr;

		auto out = new CVulkanIndexBuffer;
		if (!out->SetData(indices, count)) {
			delete out;
			return nullptr;
		}

		return out;
	}

	InputLayout* VulkanRenderer::CreateInputLayout(unsigned char *vsbytecode, unsigned vsbytecodelen)
	{
		auto inputlayout = new CVulkanInputLayout;

		if (!inputlayout->Setup(vsbytecode, vsbytecodelen)) {
			delete inputlayout;
			return nullptr;
		}

		return inputlayout;
	}

	Shader *VulkanRenderer::CreateShader(unsigned char *vs_bytecode, unsigned int vs_size,
				     unsigned char *fs_bytecode, unsigned int fs_size)
	{
		if (vs_bytecode == nullptr || vs_size == 0 || fs_bytecode == nullptr || fs_size == 0)
			return nullptr;

		auto shader = new CVulkanShader;
		if (!shader->Build(vs_bytecode, vs_size, fs_bytecode, fs_size)) {
			delete shader;
			return nullptr;
		}

		return shader;
	}

	Texture2D *VulkanRenderer::CreateTexture2D(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc)
	{
		if (data == nullptr || width == 0 || height == 0)
			return nullptr;

		auto texture2d = new CVulkanTexture2D;
		if (!texture2d->SetData(data, width, height, samplerstatedesc)) {
			delete texture2d;
			return nullptr;
		}

		return texture2d;
	}

	ShaderParameterBuffer *VulkanRenderer::CreateShaderParameterBuffer(std::vector<ShaderParameterElement> elements)
	{
		if (elements.size() == 0)
			return nullptr;

		auto parambuffer = new CVulkanParameterBuffer;
		if (!parambuffer->Setup(elements)) {
			delete parambuffer;
			return nullptr;
		}

		this->parameter_bufs.push_back(parambuffer);
		return parambuffer;
	}

	void VulkanRenderer::SetShaders(Shader *shader)
	{
		if (this->current_shader != shader)
			this->current_shader = shader;
	}

	void VulkanRenderer::SetVertexBuffer(VertexBuffer* buffer)
	{
		this->current_vbo = buffer;
	}

	void VulkanRenderer::SetIndexBuffer(IndexBuffer* buffer)
	{
		this->current_ibo = buffer;
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
		if (texture == nullptr || index >= MAX_TEXTURE_UNITS)
			return;

		if (this->textureviews[index] != texture) {
			this->textureviews[index] = texture;
			this->textureviews_dirty = true;
		}
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
		// in Vulkan, the Y coordinate starts at the top-left origin and points downwards.
		this->viewport.x = rect.x;
		this->viewport.y = (float)this->ctx.swapchain_extent.height - rect.y;
		this->viewport.width = rect.z;
		this->viewport.height = -rect.w;
	}


	void VulkanRenderer::Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount)
	{
		(void)type;
		(void)vertexStart;
		(void)vertexCount;
		this->PreDraw();
	}

	void VulkanRenderer::DrawIndexed(PrimitiveType type, unsigned indexStart, unsigned indexCount)
	{
		(void)type;

		if (this->current_vbo == nullptr || this->current_ibo == nullptr)
			return;

		this->PreDraw();

		vkCmdBindPipeline(this->ctx.swapchain_command_bufs[this->ctx.current_image], VK_PIPELINE_BIND_POINT_GRAPHICS, reinterpret_cast< VkPipeline * >(this->current_shader->GetProgram())[0]);

		VkBuffer vbo = reinterpret_cast< VkBuffer * >(this->current_vbo->GetBuffer())[0];
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(this->ctx.swapchain_command_bufs[this->ctx.current_image], 0, 1, &vbo, &offset);

		VkBuffer ibo = reinterpret_cast< VkBuffer * >(this->current_ibo->GetBuffer())[0];
		vkCmdBindIndexBuffer(this->ctx.swapchain_command_bufs[this->ctx.current_image], ibo, 0, VK_INDEX_TYPE_UINT16);

		vkCmdDrawIndexed(this->ctx.swapchain_command_bufs[this->ctx.current_image], indexCount, 1, indexStart, 0, 0);
	}

	void VulkanRenderer::DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned instanceCount)
	{
		(void)type;
		(void)indexStart;
		(void)indexCount;
		(void)instanceCount;

		this->PreDraw();
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
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), this->ctx.swapchain_command_bufs[this->ctx.current_image]);
	}

	RendererType VulkanRenderer::getRendererType()
	{
		return RENDERER_VULKAN;
	}

	void VulkanRenderer::WaitForDevice()
	{
		vkDeviceWaitIdle(this->ctx.device);
	}

	void VulkanRenderer::ImGuiInit()
	{
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = this->ctx.instance;
		initInfo.PhysicalDevice = this->ctx.adapter;
		initInfo.Device = this->ctx.device;
		initInfo.QueueFamily = this->ctx.queue_family_indices[0];
		initInfo.Queue = this->ctx.queues[0];
		initInfo.DescriptorPool = this->ctx.desc_pool;
		initInfo.MinImageCount = this->ctx.min_num_swapchain_images;
		initInfo.ImageCount = this->ctx.num_swapchain_images;

		ImGui_ImplVulkan_Init(&initInfo, this->ctx.swapchain_renderpass);

		VkCommandBuffer transfer = this->ctx.BeginSingleTimeCommands(this->ctx.graphics_command_pool);
		ImGui_ImplVulkan_CreateFontsTexture(transfer);
		this->ctx.EndSingleTimeCommands(this->ctx.graphics_command_pool, this->ctx.queues[0], transfer);
	}

	void VulkanRenderer::ImGuiClose()
	{
		ImGui_ImplVulkan_Shutdown();
	}

}
