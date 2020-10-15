#include "pch/pch.h"

#include "vk_buffer.h"

namespace Renderer {

	void VulkanBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageflags, VmaMemoryUsage memusageflags, VkBuffer &buffer, VmaAllocation &alloc, VmaAllocationInfo *allocinfo)
	{
		const uint32_t *families = dev->getQueueFamilyIndices();

		VkBufferCreateInfo BufferCreateInfo = {};
		BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		BufferCreateInfo.size = size;
		BufferCreateInfo.usage = usageflags;
		BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo AllocInfo = {};
		AllocInfo.usage = memusageflags;
		if (memusageflags == VMA_MEMORY_USAGE_CPU_ONLY)
			AllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		vmaCreateBuffer(this->dev->getAllocator(), &BufferCreateInfo, &AllocInfo, &buffer, &alloc, allocinfo);
	}

	void VulkanBuffer::CopyBuffer(VkBuffer src, VkBuffer dst)
	{
		VkCommandBufferAllocateInfo cmdbufinfo = {};
		cmdbufinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdbufinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdbufinfo.commandPool = dev->getTransferCommandPool();
		cmdbufinfo.commandBufferCount = 1;

		VkCommandBuffer cmdbuf;
		VK_ASSERT(dev->fn.vkAllocateCommandBuffers(dev->get(), &cmdbufinfo, &cmdbuf), "Failed to allocate command buffer for transfer operation")

		VkCommandBufferBeginInfo begininfo = {};
		begininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		dev->fn.vkBeginCommandBuffer(cmdbuf, &begininfo);

		// VK COMMANDS START

		VkBufferCopy copyregion = {};
		copyregion.srcOffset = 0;
		copyregion.dstOffset = 0;
		copyregion.size = size;
		dev->fn.vkCmdCopyBuffer(cmdbuf, src, dst, 1, &copyregion);

		// VK COMMANDS END

		dev->fn.vkEndCommandBuffer(cmdbuf);

		VkSubmitInfo submitinfo = {};
		submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitinfo.commandBufferCount = 1;
		submitinfo.pCommandBuffers = &cmdbuf;

		VkFence fence;
		VkFenceCreateInfo fenceinfo = {};
		fenceinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceinfo.flags = 0;
		dev->fn.vkCreateFence(dev->get(), &fenceinfo, nullptr, &fence);

		dev->fn.vkQueueSubmit(dev->getTransferQueue(), 1, &submitinfo, fence);
		dev->fn.vkWaitForFences(dev->get(), 1, &fence, VK_TRUE, UINT64_MAX);

		dev->fn.vkDestroyFence(dev->get(), fence, nullptr);
		dev->fn.vkFreeCommandBuffers(dev->get(), dev->getTransferCommandPool(), 1, &cmdbuf);
	}

	void VulkanBuffer::Load(VulkanDevice *dev, VkDeviceSize size)
	{
		this->dev = dev;
		this->size = size;

#define VULKAN_GPU_ONLY_FLAGS (VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
		this->CreateBuffer(size, VULKAN_GPU_ONLY_FLAGS, VMA_MEMORY_USAGE_GPU_ONLY, this->device_buffer, this->device_alloc, nullptr);

		freed = false;
		released = true;
	}

	void VulkanBuffer::Delete()
	{
		vmaDestroyBuffer(this->dev->getAllocator(), this->device_buffer, this->device_alloc);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		if (!freed) {
			if (!released) {
				this->ReleaseData();
				released = true;
			}
			this->Delete();
			freed = true;
		}
	}

	void VulkanBuffer::UploadData(void *data)
	{
		VkBuffer HostBuffer;
		VmaAllocation HostAlloc;
		VmaAllocationInfo AllocInfo;

		this->CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, HostBuffer, HostAlloc, &AllocInfo);

		std::memcpy(AllocInfo.pMappedData, data, static_cast< size_t >(size));
		this->CopyBuffer(HostBuffer, this->device_buffer);

		vmaDestroyBuffer(dev->getAllocator(), HostBuffer, HostAlloc);
	}

	void *VulkanBuffer::GetData()
	{
		VmaAllocationInfo AllocInfo;
		this->CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_ONLY, this->host_buffer, this->host_alloc, &AllocInfo);
/*
		this->CopyBuffer(DevBuffer, HostReadBuffer);

		void *data_loc;
		dev->fn.vkMapMemory(dev->get(), HostReadBufferMemory, 0, size, 0, &data_loc);

		released = false;

		return data_loc; // not implemented yet
*/
		return nullptr;
	}

	void VulkanBuffer::ReleaseData()
	{
		if (!this->released) {
			vmaDestroyBuffer(this->dev->getAllocator(), this->host_buffer, this->host_alloc);
			this->released = true;
		}
	}

}
