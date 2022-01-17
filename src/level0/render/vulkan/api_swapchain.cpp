#include "level0/pch.h"

#include "api.h"

const std::array< VkPresentModeKHR, MAX_PRESENT_MODE_ENUM > g_VulkanPresentModes {
	VK_PRESENT_MODE_IMMEDIATE_KHR,
	VK_PRESENT_MODE_FIFO_KHR,
	VK_PRESENT_MODE_FIFO_RELAXED_KHR,
	VK_PRESENT_MODE_MAILBOX_KHR
};

VulkanSwapchain::VulkanSwapchain(VulkanDevice *device, ESwapchainPresentMode presentMode, EDeviceQueue preferPresentQueue)
{
	this->device = device;
	this->Init(presentMode, false, preferPresentQueue);
}

VulkanSwapchain::~VulkanSwapchain()
{
	for (auto &image : this->images)
		image.reset();

	std::vector< std::shared_ptr< IRenderImage > >().swap(this->images);

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

RenderExtent2D VulkanSwapchain::GetExtent()
{
	return m_extent;
}

std::shared_ptr< IRenderImage > VulkanSwapchain::GetImage(uint32_t index)
{
	return this->images[index];
}

std::vector< std::shared_ptr< IRenderImage > > VulkanSwapchain::GetImages()
{
	return this->images;
}


uint32_t VulkanSwapchain::GetMaxImages()
{
	return this->numImages;
}

uint32_t VulkanSwapchain::GetMinImages()
{
	return this->minNumImages;
}

EDeviceQueue VulkanSwapchain::GetPresentingQueue()
{
	return m_presentingQueue;
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

	VkResult result = this->device->ftbl.vkQueuePresentKHR(this->device->GetQueue(m_presentingQueue), &presentInfo);
	if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
		return SWAPCHAIN_RESULT_SUBOPTIMAL;
	else if (result != VK_SUCCESS)
		return SWAPCHAIN_RESULT_DEVICE_ERROR;

	return SWAPCHAIN_RESULT_SUCCESS;
}

void VulkanSwapchain::Recreate(ESwapchainPresentMode presentMode, EDeviceQueue preferPresentQueue)
{
	for (auto &image : this->images)
		image.reset();

	std::vector< std::shared_ptr< IRenderImage > >().swap(this->images);

	this->Init(presentMode, true, preferPresentQueue);
}

void VulkanSwapchain::Init(ESwapchainPresentMode _presentMode, bool useOldSwapchain, EDeviceQueue preferPresentQueue)
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

	m_presentingQueue = [&]() -> EDeviceQueue {
		if (preferPresentQueue == DEVICE_QUEUE_UNKNOWN)
			return DEVICE_QUEUE_GRAPHICS;

		if (!doesQueueSupportPresent(preferPresentQueue))
			return DEVICE_QUEUE_GRAPHICS;

		return preferPresentQueue;
	}();

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

	m_extent.width = chosenExtent.width;
	m_extent.height = chosenExtent.height;

	this->minNumImages = caps.minImageCount;
	if (this->minNumImages < 2)
		this->minNumImages = 2;

	if (caps.maxImageCount > 0 && this->minNumImages > caps.maxImageCount)
		throw std::runtime_error("VulkanSwapchain: Unsupported surface capabilities!");

	this->numImages = caps.minImageCount + 1;
	if (caps.maxImageCount > 0 && this->numImages > caps.maxImageCount)
		this->numImages = caps.maxImageCount;

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
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
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

	auto images = [&]() {
		uint32_t count;
		this->device->ftbl.vkGetSwapchainImagesKHR(this->device->handle, this->handle, &count, nullptr);

		std::vector< VkImage > images(count);
		this->device->ftbl.vkGetSwapchainImagesKHR(this->device->handle, this->handle, &count, images.data());

		return images;
	}();

	RenderImageSubresourceRange range;
	range.aspect = IMAGE_ASPECT_COLOR;
	range.baseMipLevel = 0;
	range.levelCount = 1;
	range.baseArrayLayer = 0;
	range.layerCount = 1;

	this->images.resize(images.size());
	for (size_t i = 0; i < images.size(); i++)
		this->images[i] = std::dynamic_pointer_cast< IRenderImage >(std::make_shared< VulkanImage >(
			this->device,
			images[i],
			[&]() -> EImageFormat {
				switch (chosenSurfaceFormat.format) {
				case VK_FORMAT_R8G8B8A8_SRGB:
					return IMAGE_FORMAT_RGBX_UINT_8_8_8_8_SRGB;
				case VK_FORMAT_R8G8B8A8_UNORM:
					return IMAGE_FORMAT_RGBX_UINT_8_8_8_8;
				case VK_FORMAT_B8G8R8A8_SRGB:
					return IMAGE_FORMAT_BGRX_UINT_8_8_8_8_SRGB;
				case VK_FORMAT_B8G8R8A8_UNORM:
					return IMAGE_FORMAT_BGRX_UINT_8_8_8_8;
				default:
					return IMAGE_FORMAT_UNKNOWN;
				}
			}(),
			m_extent,
			range
		));
}
