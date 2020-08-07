#include <cstdlib>

#include "vk_surface.h"

namespace Renderer {

#ifdef __DEBUG
	void __Debug_Print_VkSurfaceCapabilitiesKHR(VkSurfaceCapabilitiesKHR _c)
	{
		fprintf(stdout, 
			"VkSurfaceCapabilitiesKHR:\n\tminImageCount:  %u\n\tmaxImageCount:  %u\n\tcurrentExtent:  %u %u\n\tminImageExtent: %u %u\n\tmaxImageExtent: %u %u\n\tmaxImageArrayLayers: %u\n",
			_c.minImageCount,
			_c.maxImageCount,
			_c.currentExtent.width,
			_c.currentExtent.height,
			_c.minImageExtent.width,
			_c.minImageExtent.height,
			_c.maxImageExtent.width,
			_c.maxImageExtent.height,
			_c.maxImageArrayLayers
			);
	}

	void __Debug_Print_VkSurfaceFormatKHR(VkSurfaceFormatKHR _f, uint32_t index)
	{
		fprintf(stdout,
			"VkSurfaceFormatKHR [%u]:\n\tformat: %u\n\tcolorSpace: %u\n",
			index,
			_f.format,
			_f.colorSpace
		);
	}

	void __Debug_Print_VkPresentModeKHR(VkPresentModeKHR _p, uint32_t index)
	{
		const char *str;

		fprintf(stdout,
			"VkPresentModeKHR [%u]: ", 
			index);
	
		switch(_p) {
		case VK_PRESENT_MODE_IMMEDIATE_KHR:
			str = "VK_PRESENT_MODE_IMMEDIATE_KHR";
			break;
		case VK_PRESENT_MODE_FIFO_KHR:
			str = "VK_PRESENT_MODE_FIFO_KHR";
			break;
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
			str = "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
			break;
		case VK_PRESENT_MODE_MAILBOX_KHR:
			str = "VK_PRESENT_MODE_MAILBOX_KHR";
			break;
		default:
			str = "(undefined)";
			break;
		}

		fprintf(stdout, "%s\n", str);
	}
#endif

	void VulkanSurface::Load(SDL_Window *window, VulkanInstance &instance, const VulkanDevice &dev, std::string title, int w, int h)
	{
		this->dev = dev;

		/* Get Surface Information for creating Swap Chain */

		if (!SDL_Vulkan_CreateSurface(window, instance.get(), &surface))
			return;

		VkBool32 Supported;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(this->dev.getPhysicalDev(), this->dev.getQueueFamilyIndices()[0], surface, &Supported), "surface got lost somewhere")
		VK_FATAL(Supported == VK_FALSE, "VulkanDevice does not support presentation")

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->dev.getPhysicalDev(), surface, &capabilities);
#ifdef __DEBUG
		__Debug_Print_VkSurfaceCapabilitiesKHR(capabilities);
#endif

		swapExtent = capabilities.currentExtent;

		uint32_t formatcount;
		std::vector<VkSurfaceFormatKHR> formats;
		vkGetPhysicalDeviceSurfaceFormatsKHR(this->dev.getPhysicalDev(), surface, &formatcount, nullptr);
		formats.reserve(formatcount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(this->dev.getPhysicalDev(), surface, &formatcount, formats.data());

		surfaceFormat = {};

		for (uint32_t i = 0; i < formatcount; i++) {
#ifdef __DEBUG
			__Debug_Print_VkSurfaceFormatKHR(formats[i], i);
#endif
			if ((formats[i].format == VK_SURFACE_FORMAT) && (formats[i].colorSpace == VK_SURFACE_COLORSPACE)) {
				surfaceFormat = formats[i];
				break;
			}
		}

		VK_FATAL(surfaceFormat.format == 0, "cannot find supported surface format")

		uint32_t presentmodecount;
		std::vector<VkPresentModeKHR> presentmodes;
		vkGetPhysicalDeviceSurfacePresentModesKHR(this->dev.getPhysicalDev(), surface, &presentmodecount, nullptr);
		presentmodes.reserve(presentmodecount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(this->dev.getPhysicalDev(), surface, &presentmodecount, presentmodes.data());

		presentMode = (VkPresentModeKHR)0xFF; // some garbage value that isnt used, this is for checking.

		for (uint32_t i = 0; i < presentmodecount; i++) {
#ifdef __DEBUG
			__Debug_Print_VkPresentModeKHR(presentmodes[i], i);
#endif
			if (presentmodes[i] == VK_PRESENT_MODE) {
				presentMode = VK_PRESENT_MODE;
				break;
			}
		}

		VK_FATAL(presentMode == 0xFF, "cannot find supported surface present mode")

		uint32_t images = capabilities.minImageCount + 1;

		VkSwapchainCreateInfoKHR createinfo = {};
		createinfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createinfo.pNext = nullptr;
		createinfo.flags = 0;
		createinfo.surface = surface;
		createinfo.minImageCount = images;
		createinfo.imageFormat = surfaceFormat.format;
		createinfo.imageColorSpace = surfaceFormat.colorSpace;
		createinfo.imageExtent = swapExtent;
		createinfo.imageArrayLayers = 1;
		createinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		createinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createinfo.queueFamilyIndexCount = 0;
		createinfo.pQueueFamilyIndices = nullptr;
		createinfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createinfo.presentMode = presentMode;
		createinfo.clipped = VK_TRUE;
		createinfo.oldSwapchain = VK_NULL_HANDLE;

		VK_ASSERT(vkCreateSwapchainKHR(this->dev.get(), &createinfo, nullptr, &swapchain), "failed to create swapchain")

		vkGetSwapchainImagesKHR(this->dev.get(), swapchain, &images, nullptr);
		swapchainimages.resize(images);
		vkGetSwapchainImagesKHR(this->dev.get(), swapchain, &images, swapchainimages.data());

		/* Setup image views for viewing swapchain images */
		swapchainimageViews.resize(images);

		for (uint32_t i = 0; i < images; i++) {
			VkImageViewCreateInfo view_createinfo = {};
			view_createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_createinfo.image = swapchainimages[i];
			view_createinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_createinfo.format = surfaceFormat.format;
			view_createinfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
			view_createinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_createinfo.subresourceRange.baseMipLevel = 0;
			view_createinfo.subresourceRange.levelCount = 1;
			view_createinfo.subresourceRange.baseArrayLayer = 0;
			view_createinfo.subresourceRange.layerCount = 1;

			VK_ASSERT(vkCreateImageView(this->dev.get(), &view_createinfo, nullptr, &swapchainimageViews[i]), "failed to create swapchain image views")
		}

		/* Setup synchronization primitives */

		available.reserve(images);
		finished.reserve(images);
		inflight.reserve(images);
		imagesinflight.resize(images, VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreinfo{};
		semaphoreinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceinfo{};
		fenceinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < images; i++) {
			VK_ASSERT(vkCreateSemaphore(this->dev.get(), &semaphoreinfo, nullptr, &available[i]) || 
				  vkCreateSemaphore(this->dev.get(), &semaphoreinfo, nullptr, &finished[i])  || 
				  vkCreateFence(this->dev.get(), &fenceinfo, nullptr, &inflight[i]), "failed to create synchronization objects")
		}

		VkCommandBufferAllocateInfo commandbufferinfo = {};
		commandbufferinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandbufferinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandbufferinfo.commandPool = this->dev.getGraphicsCommandPool();
		commandbufferinfo.commandBufferCount = images;

		presentcommandbuffers.reserve(images);

		VK_ASSERT(vkAllocateCommandBuffers(this->dev.get(), &commandbufferinfo, presentcommandbuffers.data()), "Failed to allocate command buffers for the presenter")

		current_image = 0;
	}

	void VulkanSurface::Release(VulkanInstance instance)
	{
		dev.WaitIdle();

		for (uint32_t i = 0; i < swapchainimageViews.size(); i++) {
			vkDestroySemaphore(dev.get(), available[i], nullptr);
			vkDestroySemaphore(dev.get(), finished[i], nullptr);
			vkDestroyFence(dev.get(), inflight[i], nullptr);
		}

		for (auto i : swapchainimageViews)
			vkDestroyImageView(dev.get(), i, nullptr);
		
		vkDestroySwapchainKHR(dev.get(), swapchain, nullptr);
		vkDestroySurfaceKHR(instance.get(), surface, nullptr);
	}

	VkCommandBuffer VulkanSurface::getPresentCommandBuffer()
	{
		return presentcommandbuffers[current_image];
	}

	void VulkanSurface::Present()
	{
		vkWaitForFences(dev.get(), 1, &inflight[current_image], VK_TRUE, UINT64_MAX);

		uint32_t image_index;
		VK_ASSERT(vkAcquireNextImageKHR(dev.get(), swapchain, UINT64_MAX, available[current_image], VK_NULL_HANDLE, &image_index), "failed to get image from swapchain")

		if (imagesinflight[image_index] != VK_NULL_HANDLE) {
			vkWaitForFences(dev.get(), 1, &imagesinflight[image_index], VK_TRUE, UINT64_MAX);
		}
		imagesinflight[image_index] = inflight[current_image];

		VkPipelineStageFlags waitDst = VK_PIPELINE_STAGE_TRANSFER_BIT;

		VkSubmitInfo submitinfo = {};
		submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitinfo.waitSemaphoreCount = 1;
		submitinfo.pWaitSemaphores = &available[current_image];
		submitinfo.pWaitDstStageMask = &waitDst;
		submitinfo.commandBufferCount = 1;
		submitinfo.pCommandBuffers = &presentcommandbuffers[image_index];
		submitinfo.signalSemaphoreCount = 1;
		submitinfo.pSignalSemaphores = &finished[current_image]; // when done, signal rendering finished semaphore

		vkResetFences(dev.get(), 1, &inflight[current_image]);

		VK_ASSERT(vkQueueSubmit(dev.getGraphicsQueue(), 1, &submitinfo, inflight[current_image]), "failed to submit presenter command buffer")

		VkPresentInfoKHR presentinfo = {};
		presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentinfo.waitSemaphoreCount = 1;
		presentinfo.pWaitSemaphores = &finished[current_image];
		presentinfo.swapchainCount = 1;
		presentinfo.pSwapchains = &swapchain;
		presentinfo.pImageIndices = (const uint32_t *)&image_index;

		VK_ASSERT(vkQueuePresentKHR(dev.getGraphicsQueue(), &presentinfo), "failed to present")
	
		current_image = (current_image + 1) % swapchainimages.size();
	}

	void VulkanSurface::Clear(float r, float g, float b, float a)
	{
		uint32_t queuefamily = dev.getQueueFamilyIndices()[0];

		VkImageSubresourceRange subResourceRange = {};
		subResourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subResourceRange.baseMipLevel = 0;
		subResourceRange.levelCount = 1;
		subResourceRange.baseArrayLayer = 0;
		subResourceRange.layerCount = 1;

		VkClearColorValue clearColor = {
			{ r, g, b, a } // R, G, B, A
		};

		VkImageMemoryBarrier presentToClearBarrier = {};
		presentToClearBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		presentToClearBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		presentToClearBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		presentToClearBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		presentToClearBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		presentToClearBarrier.srcQueueFamilyIndex = queuefamily;
		presentToClearBarrier.dstQueueFamilyIndex = queuefamily;
		presentToClearBarrier.image = swapchainimages[current_image];
		presentToClearBarrier.subresourceRange = subResourceRange;

		// Change layout of image to be optimal for presenting
		VkImageMemoryBarrier clearToPresentBarrier = {};
		clearToPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		clearToPresentBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		clearToPresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		clearToPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		clearToPresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		clearToPresentBarrier.srcQueueFamilyIndex = queuefamily;
		clearToPresentBarrier.dstQueueFamilyIndex = queuefamily;
		clearToPresentBarrier.image = swapchainimages[current_image];
		clearToPresentBarrier.subresourceRange = subResourceRange;

		VkCommandBufferBeginInfo begininfo = {};
		begininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begininfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		vkBeginCommandBuffer(getPresentCommandBuffer(), &begininfo);

		vkCmdPipelineBarrier(getPresentCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &presentToClearBarrier);
		vkCmdClearColorImage(getPresentCommandBuffer(), swapchainimages[current_image], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &subResourceRange);
		vkCmdPipelineBarrier(getPresentCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &clearToPresentBarrier);
		vkEndCommandBuffer(getPresentCommandBuffer());

		dev.WaitIdle();
	}

}
