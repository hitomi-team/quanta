#include "pch/pch.h"

#include "vulkanimage.h"
#include "vulkanctx.h"

bool CVulkanImage::Setup(unsigned w, unsigned h, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect_mask)
{
	this->width = w;
	this->height = h;
	this->image_format = format;

	VkImageCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.format = format;
	createInfo.extent.width = w;
	createInfo.extent.height = h;
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage = usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	vmaCreateImage(g_vulkanCtx->allocator, &createInfo, &allocCreateInfo, &this->image, &this->allocation, nullptr);

	VkImageViewCreateInfo viewCreateInfo = {};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = this->image;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = format;
	viewCreateInfo.subresourceRange.aspectMask = aspect_mask;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;

	VK_ASSERT(vkCreateImageView(g_vulkanCtx->device, &viewCreateInfo, nullptr, &this->image_view), "Failed to create VkImageView");

	return true;
}

void CVulkanImage::Release()
{
	vkDestroyImageView(g_vulkanCtx->device, this->image_view, nullptr);
	vmaDestroyImage(g_vulkanCtx->allocator, this->image, this->allocation);
	this->ResetCache();
}

void CVulkanImage::TransitionLayout(VkAccessFlags src_access, VkAccessFlags dst_access, VkImageLayout old_layout, VkImageLayout new_layout, VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage)
{
	VkCommandBuffer transfer = g_vulkanCtx->BeginSingleTimeCommands(g_vulkanCtx->graphics_command_pool);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcAccessMask = src_access;
	barrier.dstAccessMask = dst_access;
	barrier.oldLayout = old_layout;
	barrier.newLayout = new_layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = this->image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(transfer, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	g_vulkanCtx->EndSingleTimeCommands(g_vulkanCtx->graphics_command_pool, g_vulkanCtx->queues[0], transfer);
}

void CVulkanImage::CopyBufferToImage(CVulkanBuffer &buf)
{
	VkCommandBuffer transfer = g_vulkanCtx->BeginSingleTimeCommands(g_vulkanCtx->transfer_command_pool);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { this->width, this->height, 1 };

	vkCmdCopyBufferToImage(transfer, buf.GetBuffer(), this->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	g_vulkanCtx->EndSingleTimeCommands(g_vulkanCtx->transfer_command_pool, g_vulkanCtx->queues[1], transfer);
}
