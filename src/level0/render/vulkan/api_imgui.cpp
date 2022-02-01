#include "level0/pch.h"

#include "api.h"

static PFN_vkVoidFunction vk_imgui_loader_func(const char *function_name, void *user_data)
{
	(void)user_data;
	return reinterpret_cast< PFN_vkGetInstanceProcAddr >(SDL_Vulkan_GetVkGetInstanceProcAddr())(g_VulkanAPI->inst, function_name);
}

VulkanImGui::VulkanImGui(VulkanDevice *device, std::shared_ptr< IRenderPass > _renderPass, uint32_t minImageCount, uint32_t imageCount)
{
	this->device = device;
	m_renderPass = std::dynamic_pointer_cast< VulkanRenderPass >(_renderPass);
	m_imageCount = imageCount;

	m_commandPools.resize(m_imageCount);
	m_commandBuffers.resize(m_imageCount);
	m_semaphores.resize(m_imageCount);

	for (uint32_t i = 0; i < m_imageCount; i++) {
		m_commandPools[i] = std::dynamic_pointer_cast< VulkanCommandPool >(this->device->CreateCommandPool(DEVICE_QUEUE_GRAPHICS, COMMAND_POOL_USAGE_TRANSIENT));
		m_commandBuffers[i] = std::dynamic_pointer_cast< VulkanCommandBuffer >(m_commandPools[i]->AllocateSingle(COMMAND_BUFFER_LEVEL_PRIMARY));
		m_semaphores[i] = this->device->CreateSemaphore();
	}

	static const std::array< VkDescriptorPoolSize, 11 > poolSizes {
		VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo poolCreateInfo;
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.pNext = nullptr;
	poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolCreateInfo.maxSets = 1000;
	poolCreateInfo.poolSizeCount = static_cast< uint32_t >(poolSizes.size());
	poolCreateInfo.pPoolSizes = poolSizes.data();

	if (this->device->ftbl.vkCreateDescriptorPool(this->device->handle, &poolCreateInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		throw std::runtime_error("VulkanImGui: Failed to create descriptor pool!");

	ImGui_ImplVulkan_InitInfo imguiInfo = {};
	imguiInfo.Instance = g_VulkanAPI->inst;
	imguiInfo.PhysicalDevice = this->device->phy->handle;
	imguiInfo.Device = this->device->handle;
	imguiInfo.Queue = this->device->GetQueue(DEVICE_QUEUE_GRAPHICS);
	imguiInfo.DescriptorPool = m_descriptorPool;
	imguiInfo.MinImageCount = minImageCount;
	imguiInfo.ImageCount = imageCount;
	imguiInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	m_context = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_context);
	ImGui_ImplSDL2_InitForVulkan(g_VulkanAPI->m_window);
	ImGui_ImplVulkan_LoadFunctions(vk_imgui_loader_func);
	ImGui_ImplVulkan_Init(&imguiInfo, m_renderPass->handle);

	auto transferCommandPool = this->device->CreateCommandPool(DEVICE_QUEUE_GRAPHICS, COMMAND_POOL_USAGE_TRANSIENT);
	auto transferCommandBuffer = std::dynamic_pointer_cast< VulkanCommandBuffer >(transferCommandPool->AllocateSingle(COMMAND_BUFFER_LEVEL_PRIMARY));
	auto fence = this->device->CreateFence(false);

	transferCommandBuffer->Begin(COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT);
	ImGui_ImplVulkan_CreateFontsTexture(transferCommandBuffer->handle);
	transferCommandBuffer->End();
	this->device->Submit(DEVICE_QUEUE_GRAPHICS, std::dynamic_pointer_cast< IRenderCommandBuffer >(transferCommandBuffer), nullptr, PIPELINE_STAGE_TRANSFER, nullptr, fence);
	fence->Wait(UINT64_MAX);

	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

VulkanImGui::~VulkanImGui()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext(m_context);

	this->device->ftbl.vkDestroyDescriptorPool(this->device->handle, m_descriptorPool, nullptr);
}

void VulkanImGui::NewFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(g_VulkanAPI->m_window);
	ImGui::NewFrame();

}

void VulkanImGui::Draw(std::shared_ptr< IRenderFramebuffer > _framebuffer, uint32_t imageIndex)
{
	ImGui::EndFrame();
	ImGui::Render();

	auto framebuffer = std::dynamic_pointer_cast< VulkanFramebuffer >(_framebuffer);

	this->device->ftbl.vkResetCommandPool(this->device->handle, m_commandPools[imageIndex]->handle, 0);

	m_commandBuffers[imageIndex]->Begin(COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT);
	m_commandBuffers[imageIndex]->BeginRenderPass(m_renderPass, framebuffer, [&]() -> RenderRectangle {
		return RenderRectangle { RenderOffset2D { 0, 0 }, { 1, 1 } };
	}(), std::vector< RenderClearValue >(1), SUBPASS_CONTENTS_INLINE);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_commandBuffers[imageIndex]->handle);
	m_commandBuffers[imageIndex]->EndRenderPass();
	m_commandBuffers[imageIndex]->End();
}

std::shared_ptr< IRenderCommandBuffer > VulkanImGui::GetCommandBuffer(uint32_t imageIndex)
{
	return std::dynamic_pointer_cast< IRenderCommandBuffer >(m_commandBuffers[imageIndex]);
}

std::shared_ptr< IRenderSemaphore > VulkanImGui::GetSemaphore(uint32_t imageIndex)
{
	return m_semaphores[imageIndex];
}
