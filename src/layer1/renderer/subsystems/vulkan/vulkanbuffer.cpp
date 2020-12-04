#include "pch/pch.h"

#include "vulkanbuffer.h"
#include "vulkanctx.h"

void CVulkanBuffer::CopyBuffer(VkBuffer src, VkBuffer dst, uint64_t size, uint64_t offset)
{
	VkCommandBuffer transfer = g_vulkanCtx->BeginSingleTimeCommands();

	VkBufferCopy region = {};
	region.size = size;
	region.dstOffset = offset;
	vkCmdCopyBuffer(transfer, src, dst, 1, &region);

	g_vulkanCtx->EndSingleTimeCommands(transfer);
}

bool CVulkanBuffer::Setup(VkBufferUsageFlags usage, VmaMemoryUsage mem_usage, uint64_t size)
{
	this->buf_size = size;
	this->mem_usage = mem_usage;

	VkBufferCreateInfo bufCreateInfo = {};
	bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufCreateInfo.size = size;
	bufCreateInfo.usage = usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = mem_usage;

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

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

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
	if (host_mem != nullptr)
		return host_mem;

	vmaMapMemory(g_vulkanCtx->allocator, this->alloc, &this->host_mem);
	return this->host_mem;
}

void CVulkanBuffer::Unmap()
{
	vmaUnmapMemory(g_vulkanCtx->allocator, this->alloc);
	this->host_mem = nullptr;
}
