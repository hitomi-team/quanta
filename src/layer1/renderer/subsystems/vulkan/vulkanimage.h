#ifndef LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANIMAGE_H
#define LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANIMAGE_H

#include "vulkanbuffer.h"

class CVulkanImage {
public:
	inline CVulkanImage() { this->ResetCache(); }

	inline void ResetCache()
	{
		this->image = VK_NULL_HANDLE;
		this->image_view = VK_NULL_HANDLE;
		this->image_format = VK_FORMAT_UNDEFINED;
		this->width = 0;
		this->height = 0;
	}

	bool Setup(unsigned w, unsigned h, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect_mask);
	void Release();

	void TransitionLayout(VkAccessFlags src_access, VkAccessFlags dst_access, VkImageLayout old_layout, VkImageLayout new_layout, VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage);
	void CopyBufferToImage(CVulkanBuffer &buf);

	inline VkImage GetImage() { return this->image; }
	inline VkImageView GetView() { return this->image_view; }

protected:
	VkImage image;
	VkImageView image_view;
	VmaAllocation allocation;

	VkFormat image_format;
	unsigned width;
	unsigned height;

};

#endif
