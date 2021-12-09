#include "level0/pch.h"

#include "api.h"

VulkanCommandPool::VulkanCommandPool(VulkanDevice *device, EDeviceQueue queue, ECommandPoolUsage usage)
{
	this->device = device;

	VkCommandPoolCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = static_cast< VkCommandPoolCreateFlags >(usage);
	createInfo.queueFamilyIndex = this->device->GetQueueFamilyIndex(queue);

	if (this->device->ftbl.vkCreateCommandPool(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanCommandPool: Failed to create command pool!");
}

VulkanCommandPool::~VulkanCommandPool()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyCommandPool(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}

std::shared_ptr< IRenderCommandBuffer > VulkanCommandPool::AllocateSingle(ECommandBufferLevel level)
{
	return std::dynamic_pointer_cast< IRenderCommandBuffer >(std::make_shared< VulkanCommandBuffer >(this->device, this, level));
}

void VulkanCommandPool::AllocateBulk(std::vector< std::shared_ptr< IRenderCommandBuffer > > &commandBuffers, ECommandBufferLevel level)
{
	for (auto &commandBuffer : commandBuffers)
		commandBuffer = std::dynamic_pointer_cast< IRenderCommandBuffer >(std::make_shared< VulkanCommandBuffer >(this->device, this, level));
}
