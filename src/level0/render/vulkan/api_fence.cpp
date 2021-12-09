#include "level0/pch.h"

#include "api.h"

VulkanFence::VulkanFence(VulkanDevice *device, bool signaled)
{
	this->device = device;

	VkFenceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	if (this->device->ftbl.vkCreateFence(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanFence: Failed to create fence!");
}

VulkanFence::~VulkanFence()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyFence(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}

bool VulkanFence::IsSignaled()
{
	return this->device->ftbl.vkGetFenceStatus(this->device->handle, this->handle) == VK_SUCCESS;
}

void VulkanFence::Reset()
{
	this->device->ftbl.vkResetFences(this->device->handle, 1, &this->handle);
}

void VulkanFence::Wait(uint64_t timeout)
{
	this->device->ftbl.vkWaitForFences(this->device->handle, 1, &this->handle, VK_FALSE, timeout);
}
