#ifndef LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANCTX_H
#define LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANCTX_H

#include "vulkanrequired.h"
#include "vulkanbuffer.h"

struct CVulkanSwapchainSync {
	VkFence fence;
	VkSemaphore wait_sync;
	VkSemaphore wake_sync;
};

struct CVulkanSwapchainDetails {
	VkSurfaceCapabilitiesKHR caps;
	std::vector< VkSurfaceFormatKHR > formats;
	std::vector< VkPresentModeKHR > presentmodes;

	void Query(VkPhysicalDevice adapter, VkSurfaceKHR surface);
};

class CVulkanCtx {
public:
	SDL_Window *window;

	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice adapter;
	VkPhysicalDeviceProperties adapter_props;
	VkDevice device;
	VmaAllocator allocator;

	VkSwapchainKHR swapchain;
	VkFormat swapchain_format;
	VkExtent2D swapchain_extent;
	VkRenderPass swapchain_renderpass;
	std::vector< VkImage > swapchain_images;
	std::vector< VkImageView > swapchain_imageviews;
	std::vector< VkFramebuffer > swapchain_framebuffers;
	std::vector< VkCommandPool > swapchain_command_pool;
	std::vector< VkCommandBuffer > swapchain_command_bufs;
	std::vector< CVulkanSwapchainSync > swapchain_sync;
	uint32_t num_swapchain_images;
	uint32_t current_image;
	uint32_t acquire_image;
	uint32_t present_queue;

	// [0] - graphics; [1] - transfer; [2] - compute
	uint32_t queue_family_indices[3];
	VkQueue queues[3];

	VkDescriptorPool desc_pool;
	VkDescriptorSetLayout desc_layout;

	VkPipelineLayout pipeline_layout;

	VkCommandPool graphics_command_pool;
	VkCommandPool transfer_command_pool;
	VkCommandPool compute_command_pool;

#ifdef __DEBUG
	VkDebugUtilsMessengerEXT debug_messenger;
#endif

	inline CVulkanCtx() { this->ResetCache(); }

	inline void ResetCache()
	{
		this->window = nullptr;
		this->instance = VK_NULL_HANDLE;
		this->surface = VK_NULL_HANDLE;
		this->adapter = VK_NULL_HANDLE;
		this->device = VK_NULL_HANDLE;
		this->swapchain = VK_NULL_HANDLE;
		this->desc_pool = VK_NULL_HANDLE;
		this->graphics_command_pool = VK_NULL_HANDLE;
		this->transfer_command_pool = VK_NULL_HANDLE;
		this->compute_command_pool = VK_NULL_HANDLE;
		this->current_image = 0;
		this->acquire_image = 0;
#ifdef __DEBUG
		this->debug_messenger = VK_NULL_HANDLE;
#endif
	}

	bool Init(SDL_Window *window);
	bool InitSwapchain();
	void CloseSwapchain();
	void Close();

	VkCommandBuffer BeginSingleTimeCommands(VkCommandPool pool);
	void EndSingleTimeCommands(VkCommandPool pool, VkQueue queue, VkCommandBuffer command_buf);

	// these get disabled and turned into dummy inline functions during release - should hopefully be optimized out.
#ifdef __DEBUG
	void DebugSetObjectName(void *obj, VkObjectType objtype, const char *objname);
	void DebugSetObjectTag(void *obj, VkObjectType objtype, uint64_t tagname, size_t tagsize, const void *tagdata);
	void DebugLabelBegin(VkCommandBuffer, const char *labelname, float r, float g, float b);
	void DebugLabelEnd(VkCommandBuffer);
	void DebugLabelInsert(VkCommandBuffer, const char *labelname, float r, float g, float b);
#else
	inline void DebugSetObjectName(void *, VkObjectType, const char *) {}
	inline void DebugSetObjectTag(void *, VkObjectType, uint64_t, size_t, const void *) {}
	inline void DebugLabelBegin(VkCommandBuffer, const char *, float, float, float) {}
	inline void DebugLabelEnd(VkCommandBuffer) {}
	inline void DebugLabelInsert(VkCommandBuffer, const char *, float, float, float) {}
#endif

private:
	bool InitInstance();
	bool InitDevice();

};

extern CVulkanCtx *g_vulkanCtx;

#endif
