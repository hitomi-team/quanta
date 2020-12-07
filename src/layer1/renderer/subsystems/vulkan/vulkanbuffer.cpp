#include "pch/pch.h"

#include "vulkanbuffer.h"
#include "vulkanctx.h"

void CVulkanBuffer::CopyBuffer(VkBuffer src, VkBuffer dst, uint64_t size, uint64_t offset)
{
	VkCommandBuffer transfer = g_vulkanCtx->BeginSingleTimeCommands(g_vulkanCtx->transfer_command_pool);

	VkBufferCopy region = {};
	region.size = size;
	region.dstOffset = offset;
	vkCmdCopyBuffer(transfer, src, dst, 1, &region);

	VkBufferMemoryBarrier barrier1;
	barrier1.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	barrier1.pNext = nullptr;
	barrier1.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier1.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	barrier1.srcQueueFamilyIndex = g_vulkanCtx->queue_family_indices[1];
	barrier1.dstQueueFamilyIndex = g_vulkanCtx->queue_family_indices[0];
	barrier1.buffer = dst;
	barrier1.offset = offset;
	barrier1.size = size;
	vkCmdPipelineBarrier(transfer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 1, &barrier1, 0, nullptr);

	g_vulkanCtx->EndSingleTimeCommands(g_vulkanCtx->transfer_command_pool, g_vulkanCtx->queues[1], transfer);
}

bool CVulkanBuffer::Setup(CVulkanBufferInitInfo &initInfo, uint64_t size)
{
	this->buf_size = size;
	this->mem_usage = initInfo.vmaUsage;

	VkBufferCreateInfo bufCreateInfo = {};
	bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufCreateInfo.size = size;
	bufCreateInfo.usage = initInfo.usage;
	bufCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = initInfo.vmaUsage;
	allocCreateInfo.flags = initInfo.vmaFlags;
	allocCreateInfo.requiredFlags = initInfo.reqMemFlags;
	allocCreateInfo.preferredFlags = initInfo.prefMemFlags;

	vmaCreateBuffer(g_vulkanCtx->allocator, &bufCreateInfo, &allocCreateInfo, &this->buf, &this->alloc, nullptr);

	return true;
}

void CVulkanBuffer::Release()
{
	if (this->host_mem != nullptr)
		this->Unmap();

	vmaDestroyBuffer(g_vulkanCtx->allocator, this->buf, this->alloc);
	this->ResetCache();
}

void CVulkanBuffer::Upload(const void *data, uint64_t size, uint64_t offset)
{
	if (this->mem_usage == VMA_MEMORY_USAGE_GPU_ONLY) {
		VkBuffer staging_buf;
		VmaAllocation staging_alloc;
		VmaAllocationInfo staging_alloc_info;

		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
		allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		allocCreateInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

		vmaCreateBuffer(g_vulkanCtx->allocator, &createInfo, &allocCreateInfo, &staging_buf, &staging_alloc, &staging_alloc_info);
		std::memcpy(staging_alloc_info.pMappedData, data, size);

		this->CopyBuffer(staging_buf, this->buf, size, offset);

		vmaDestroyBuffer(g_vulkanCtx->allocator, staging_buf, staging_alloc);
	} else {
		this->Map();
		std::memcpy((char *)this->host_mem + offset, data, size);
		this->Unmap();
	}
}

void *CVulkanBuffer::Map()
{
	vmaMapMemory(g_vulkanCtx->allocator, this->alloc, &this->host_mem);
	return this->host_mem;
}

void CVulkanBuffer::Unmap()
{
	vmaUnmapMemory(g_vulkanCtx->allocator, this->alloc);
	this->host_mem = nullptr;
}

void CVulkanBuffer::Invalidate()
{
	vmaInvalidateAllocation(g_vulkanCtx->allocator, this->alloc, 0, this->buf_size);
}

void CVulkanBuffer::Flush()
{
	vmaFlushAllocation(g_vulkanCtx->allocator, this->alloc, 0, this->buf_size);
}
