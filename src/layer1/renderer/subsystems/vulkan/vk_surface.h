#ifndef VK_WINDOW_H
#define VK_WINDOW_H

#include "vk_instance.h"
#include "vk_device.h"

#include <string>
#include <cstdio>

// these can be defined manually, but these are the defaults.

#ifndef VK_SURFACE_FORMAT
#define VK_SURFACE_FORMAT VK_FORMAT_B8G8R8A8_SRGB
#endif

#ifndef VK_SURFACE_COLORSPACE
#define VK_SURFACE_COLORSPACE VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
#endif

#ifndef VK_PRESENT_MODE
#define VK_PRESENT_MODE VK_PRESENT_MODE_FIFO_KHR
#endif

namespace Renderer {

	class VulkanSurface {
	public:
		VulkanSurface() {}
		VulkanSurface(SDL_Window *window, VulkanInstance &instance, const VulkanDevice &dev) { Load(window, instance, dev); }
		void Load(SDL_Window *window, VulkanInstance &instance, const VulkanDevice &dev);
		void Release(VulkanInstance instance);

		VkCommandBuffer getPresentCommandBuffer();
		void Present();
		void Clear(float r, float g, float b, float a);

	protected:
		VulkanDevice dev;
		VkSurfaceKHR surface;
		VkSwapchainKHR swapchain;
		std::vector<VkImage> swapchainimages;
		std::vector<VkImageView> swapchainimageViews;
		std::vector<VkCommandBuffer> presentcommandbuffers;
		uint32_t current_image;

		// Surface Properties
		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;
		VkExtent2D swapExtent;

		// Synchronization Primitives
		std::vector<VkSemaphore> available;
		std::vector<VkSemaphore> finished;
		std::vector<VkFence> inflight;
		std::vector<VkFence> imagesinflight;
	};

}

#endif
