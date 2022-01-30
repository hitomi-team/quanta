#include "level0/pch.h"

#include "api.h"

VulkanBufferView::VulkanBufferView(VulkanDevice *device, std::shared_ptr< VulkanBuffer > buffer, EImageFormat bufferFormat, uint64_t offset, uint64_t range)
{
	this->device = device;
	this->buffer = std::dynamic_pointer_cast< IRenderBuffer >(buffer);
	this->bufferFormat = bufferFormat;
	this->offset = offset;
	this->range = range;

	VkBufferViewCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.buffer = buffer->handle;
	createInfo.format = g_VulkanImageFormats[bufferFormat].format;
	createInfo.offset = offset;
	createInfo.range = range;

	VK_CHECK(this->device->ftbl.vkCreateBufferView(this->device->handle, &createInfo, nullptr, &this->handle), "VulkanBufferView: Failed to create buffer view!");
}

VulkanBufferView::~VulkanBufferView()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyBufferView(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}

	this->buffer.reset();
}
