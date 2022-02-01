#include "level0/pch.h"

#include "api.h"

VulkanSwapchain::VulkanSwapchain(VulkanDevice *device, ESwapchainPresentMode presentMode)
{
	this->device = device;
	this->Init(presentMode, false);
}

VulkanSwapchain::~VulkanSwapchain()
{
	if (this->numImages != 0) {
		this->device->ftbl.vkDestroyRenderPass(this->device->handle, this->defaultRenderPass, nullptr);

		for (uint32_t i = 0; i < this->numImages; i++) {
			this->device->ftbl.vkDestroyFramebuffer(this->device->handle, this->framebuffers[i], nullptr);
			this->device->ftbl.vkDestroyImageView(this->device->handle, this->imageViews[i], nullptr);
		}

		IO_EraseCPPVector(this->images);
		IO_EraseCPPVector(this->imageViews);
		IO_EraseCPPVector(this->framebuffers);
	}

	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroySwapchainKHR(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}

	this->numImages = 0;
}

ESwapchainResult VulkanSwapchain::GetAvailableImage(std::shared_ptr< IRenderSemaphore > semaphore, std::shared_ptr< IRenderFence > fence, uint64_t timeout, uint32_t &index)
{
	VkFence fenceHandle = fence != nullptr ? std::dynamic_pointer_cast< VulkanFence >(fence)->handle : VK_NULL_HANDLE;
	VkResult result = this->device->ftbl.vkAcquireNextImageKHR(this->device->handle, this->handle, timeout, std::dynamic_pointer_cast< VulkanSemaphore >(semaphore)->handle, fenceHandle, &index);

	if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
		return SWAPCHAIN_RESULT_SUBOPTIMAL;
	else if (result == VK_NOT_READY)
		return SWAPCHAIN_RESULT_NOT_READY;
	else if (result != VK_SUCCESS)
		return SWAPCHAIN_RESULT_DEVICE_ERROR;

	return SWAPCHAIN_RESULT_SUCCESS;
}

ESwapchainResult VulkanSwapchain::PresentImage(std::shared_ptr< IRenderSemaphore > waitSemaphore, uint32_t index)
{
	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &std::dynamic_pointer_cast< VulkanSemaphore >(waitSemaphore)->handle;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &this->handle;
	presentInfo.pImageIndices = &index;
	presentInfo.pResults = nullptr;

	// TODO: get the presenting queue from somewhere else
	VkResult result = this->device->ftbl.vkQueuePresentKHR(this->device->GetQueue(DEVICE_QUEUE_GRAPHICS), &presentInfo);
	if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
		return SWAPCHAIN_RESULT_SUBOPTIMAL;
	else if (result != VK_SUCCESS)
		return SWAPCHAIN_RESULT_DEVICE_ERROR;

	return SWAPCHAIN_RESULT_SUCCESS;
}

void VulkanSwapchain::Recreate(ESwapchainPresentMode presentMode)
{
	if (this->numImages != 0) {
		this->device->ftbl.vkDestroyRenderPass(this->device->handle, this->defaultRenderPass, nullptr);

		for (uint32_t i = 0; i < this->numImages; i++) {
			this->device->ftbl.vkDestroyFramebuffer(this->device->handle, this->framebuffers[i], nullptr);
			this->device->ftbl.vkDestroyImageView(this->device->handle, this->imageViews[i], nullptr);
		}

		IO_EraseCPPVector(this->images);
		IO_EraseCPPVector(this->imageViews);
		IO_EraseCPPVector(this->framebuffers);
	}

	this->Init(presentMode, true);
}

void VulkanSwapchain::BeginRenderPass(std::shared_ptr< IRenderCommandBuffer > _commandBuffer, uint32_t index)
{
	VkCommandBuffer handle = std::dynamic_pointer_cast< VulkanCommandBuffer >(_commandBuffer)->handle;

	VkClearValue value = {};

	VkRenderPassBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.renderPass = this->defaultRenderPass;
	beginInfo.framebuffer = this->framebuffers[index];
	beginInfo.renderArea.offset = {};
	beginInfo.renderArea.extent = { this->extent2D.width, this->extent2D.height };
	beginInfo.clearValueCount = 1;
	beginInfo.pClearValues = &value;

	this->device->ftbl.vkCmdBeginRenderPass(handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanSwapchain::Copy2DImageToSwapchainImage(std::shared_ptr< IRenderCommandBuffer > commandBuffer, std::shared_ptr< IRenderImage > image, uint32_t index, const RenderImageCopy &region)
{
	VkCommandBuffer handle = std::dynamic_pointer_cast< VulkanCommandBuffer >(commandBuffer)->handle;
	VkImage srcImage = std::dynamic_pointer_cast< VulkanImage >(image)->handle;

	VkImageMemoryBarrier swapchainImageBarrierCopy;
	swapchainImageBarrierCopy.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	swapchainImageBarrierCopy.pNext = nullptr;
	swapchainImageBarrierCopy.srcAccessMask = 0;
	swapchainImageBarrierCopy.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	swapchainImageBarrierCopy.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	swapchainImageBarrierCopy.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	swapchainImageBarrierCopy.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	swapchainImageBarrierCopy.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	swapchainImageBarrierCopy.image = this->images[index];
	swapchainImageBarrierCopy.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	swapchainImageBarrierCopy.subresourceRange.baseMipLevel = 0;
	swapchainImageBarrierCopy.subresourceRange.levelCount = 1;
	swapchainImageBarrierCopy.subresourceRange.baseArrayLayer = 0;
	swapchainImageBarrierCopy.subresourceRange.layerCount = 1;

	VkImageMemoryBarrier swapchainImageBarrierPresent;
	swapchainImageBarrierPresent.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	swapchainImageBarrierPresent.pNext = nullptr;
	swapchainImageBarrierPresent.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	swapchainImageBarrierPresent.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	swapchainImageBarrierPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	swapchainImageBarrierPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	swapchainImageBarrierPresent.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	swapchainImageBarrierPresent.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	swapchainImageBarrierPresent.image = this->images[index];
	swapchainImageBarrierPresent.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	swapchainImageBarrierPresent.subresourceRange.baseMipLevel = 0;
	swapchainImageBarrierPresent.subresourceRange.levelCount = 1;
	swapchainImageBarrierPresent.subresourceRange.baseArrayLayer = 0;
	swapchainImageBarrierPresent.subresourceRange.layerCount = 1;

	VkImageCopy copyRegion;
	copyRegion.srcSubresource.aspectMask = g_VulkanImageAspectFlags[region.sourceSubresource.aspect];
	copyRegion.srcSubresource.mipLevel = region.sourceSubresource.mipLevel;
	copyRegion.srcSubresource.baseArrayLayer = region.sourceSubresource.baseArrayLayer;
	copyRegion.srcSubresource.layerCount = region.sourceSubresource.layerCount;
	copyRegion.srcOffset = { region.sourceOffset.x, region.sourceOffset.y, 0 };
	copyRegion.dstSubresource.aspectMask = g_VulkanImageAspectFlags[region.destSubresource.aspect];
	copyRegion.dstSubresource.mipLevel = region.destSubresource.mipLevel;
	copyRegion.dstSubresource.baseArrayLayer = region.destSubresource.baseArrayLayer;
	copyRegion.dstSubresource.layerCount = region.destSubresource.layerCount;
	copyRegion.dstOffset = { region.destOffset.x, region.destOffset.y, 0 };
	copyRegion.extent = { region.extent.width, region.extent.height, 1 };

	this->device->ftbl.vkCmdPipelineBarrier(handle, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &swapchainImageBarrierCopy);
	this->device->ftbl.vkCmdCopyImage(handle, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, this->images[index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
	this->device->ftbl.vkCmdPipelineBarrier(handle, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &swapchainImageBarrierPresent);
}

void VulkanSwapchain::Init(ESwapchainPresentMode _presentMode, bool useOldSwapchain)
{
	uint32_t numSurfaceFormats, numPresentModes;
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->device->phy->handle, g_VulkanAPI->surface, &numSurfaceFormats, nullptr);
	vkGetPhysicalDeviceSurfacePresentModesKHR(this->device->phy->handle, g_VulkanAPI->surface, &numPresentModes, nullptr);

	std::vector< VkSurfaceFormatKHR > surfaceFormats(numSurfaceFormats);
	std::vector< VkPresentModeKHR > presentModes(numPresentModes);
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->device->phy->handle, g_VulkanAPI->surface, &numSurfaceFormats, surfaceFormats.data());
	vkGetPhysicalDeviceSurfacePresentModesKHR(this->device->phy->handle, g_VulkanAPI->surface, &numPresentModes, presentModes.data());

	VkSurfaceCapabilitiesKHR caps;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device->phy->handle, g_VulkanAPI->surface, &caps);

	auto doesQueueSupportPresent = [&](EDeviceQueue queue) -> bool {
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(this->device->phy->handle, this->device->GetQueueFamilyIndex(queue), g_VulkanAPI->surface, &presentSupport);
		return presentSupport != VK_FALSE;
	};

	// TODO: prefer a surface format supplied by the user
	auto chosenSurfaceFormat = [&]() -> VkSurfaceFormatKHR {
		for (auto &surfaceFormat : surfaceFormats) {
			if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return surfaceFormat;
			else if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return surfaceFormat;
		}
		return surfaceFormats[0];
	}();

	auto chosenPresentMode = [&]() -> VkPresentModeKHR {
		for (auto &presentMode : presentModes) {
			if (presentMode == g_VulkanPresentModes[_presentMode])
				return presentMode;
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}();

	auto chosenExtent = [&]() -> VkExtent2D {
		if (caps.currentExtent.width != UINT32_MAX) {
			return caps.currentExtent;
		} else {
			int width, height;
			SDL_Vulkan_GetDrawableSize(g_VulkanAPI->m_window, &width, &height);

			VkExtent2D actualExtent = { static_cast< uint32_t >(width), static_cast< uint32_t >(height) };
			actualExtent.width = std::clamp(actualExtent.width, caps.minImageExtent.width, caps.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, caps.minImageExtent.height, caps.maxImageExtent.height);

			return actualExtent;
		}
	}();

	this->extent2D.width = chosenExtent.width;
	this->extent2D.height = chosenExtent.height;

	this->minNumImages = caps.minImageCount;
	if (this->minNumImages < 2)
		this->minNumImages = 2;

	if (caps.maxImageCount > 0 && this->minNumImages > caps.maxImageCount)
		throw std::runtime_error("VulkanSwapchain: Unsupported surface capabilities!");

	this->numImages = caps.minImageCount + 1;
	if (caps.maxImageCount > 0 && this->numImages > caps.maxImageCount)
		this->numImages = caps.maxImageCount;

	// TODO: share swapchain with different queue family indices
	VkSwapchainCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.surface = g_VulkanAPI->surface;
	createInfo.minImageCount = this->numImages;
	createInfo.imageFormat = chosenSurfaceFormat.format;
	createInfo.imageColorSpace = chosenSurfaceFormat.colorSpace;
	createInfo.imageExtent = chosenExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.preTransform = caps.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = chosenPresentMode;
	createInfo.clipped = VK_TRUE;

	VkSwapchainKHR oldSwapchain = this->handle;
	createInfo.oldSwapchain = oldSwapchain;

	if (this->device->ftbl.vkCreateSwapchainKHR(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanSwapchain: Failed to create swapchain!");

	if (useOldSwapchain)
		this->device->ftbl.vkDestroySwapchainKHR(this->device->handle, oldSwapchain, nullptr);

	this->images = [&]() -> std::vector< VkImage > {
		uint32_t count;
		this->device->ftbl.vkGetSwapchainImagesKHR(this->device->handle, this->handle, &count, nullptr);

		std::vector< VkImage > images(count);
		this->device->ftbl.vkGetSwapchainImagesKHR(this->device->handle, this->handle, &count, images.data());

		this->numImages = count;
		return images;
	}();

	VkAttachmentReference colorAttachment;
	colorAttachment.attachment = 0;
	colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription attachmentDescription;
	attachmentDescription.flags = 0;
	attachmentDescription.format = chosenSurfaceFormat.format;
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkSubpassDescription subpassDescription;
	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorAttachment;
	subpassDescription.pResolveAttachments = 0;
	subpassDescription.pDepthStencilAttachment = nullptr;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;

	VkSubpassDependency subpassDependency;
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassDependency.dependencyFlags = 0;

	VkRenderPassCreateInfo renderPassCreateInfo;
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext = nullptr;
	renderPassCreateInfo.flags = 0;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &attachmentDescription;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpassDescription;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &subpassDependency;

	VK_CHECK(this->device->ftbl.vkCreateRenderPass(this->device->handle, &renderPassCreateInfo, nullptr, &this->defaultRenderPass),
		"VulkanSwapchain: Failed to create default render pass!");

	VkImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = nullptr;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = chosenSurfaceFormat.format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	VkFramebufferCreateInfo framebufferCreateInfo;
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.pNext = nullptr;
	framebufferCreateInfo.flags = 0;
	framebufferCreateInfo.renderPass = this->defaultRenderPass;
	framebufferCreateInfo.attachmentCount = 1;
	framebufferCreateInfo.width = chosenExtent.width;
	framebufferCreateInfo.height = chosenExtent.height;
	framebufferCreateInfo.layers = 1;

	this->imageViews.resize(this->numImages);
	this->framebuffers.resize(this->numImages);

	for (uint32_t i = 0; i < this->numImages; i++) {
		imageViewCreateInfo.image = this->images[i];
		VK_CHECK(this->device->ftbl.vkCreateImageView(this->device->handle, &imageViewCreateInfo, nullptr, &this->imageViews[i]),
			"VulkanSwapchain: Failed to create image view!");

		framebufferCreateInfo.pAttachments = &this->imageViews[i];
		VK_CHECK(this->device->ftbl.vkCreateFramebuffer(this->device->handle, &framebufferCreateInfo, nullptr, &this->framebuffers[i]),
			"VulkanSwapchain: Failed to create framebuffer!");
	}
}
