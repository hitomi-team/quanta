#include "level0/pch.h"

#include "api.h"

VulkanSemaphore::VulkanSemaphore(VulkanDevice *device)
{
	this->device = device;

	VkSemaphoreCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	if (this->device->ftbl.vkCreateSemaphore(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanSemaphore: Failed to create semaphore!");
}

VulkanSemaphore::~VulkanSemaphore()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroySemaphore(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}
