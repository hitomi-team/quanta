#include "level0/pch.h"

#include "api.h"

VulkanBuffer::VulkanBuffer(VulkanDevice *device, VmaPool pool, EResourceMemoryUsage memoryUsage, EBufferUsage usage, uint64_t size)
{
	this->device = device;

	m_memoryUsage = memoryUsage;
	m_usage = usage;
	m_size = size;

	VkBufferCreateInfo bufferInfo;
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;
	bufferInfo.flags = 0;
	bufferInfo.size = size;
	bufferInfo.usage = static_cast< VkBufferUsageFlags >(usage);
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.queueFamilyIndexCount = 0;
	bufferInfo.pQueueFamilyIndices = nullptr;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.pool = pool;

	if (vmaCreateBuffer(this->device->allocator, &bufferInfo, &allocInfo, &this->handle, &this->allocation, nullptr) != VK_SUCCESS)
		throw std::runtime_error("VulkanBuffer: Cannot create buffer handle!");
}

VulkanBuffer::~VulkanBuffer()
{
	if (this->handle != VK_NULL_HANDLE) {
		vmaDestroyBuffer(this->device->allocator, this->handle, this->allocation);
		this->handle = VK_NULL_HANDLE;
	}
}

void *VulkanBuffer::Map()
{
	if (mem_ptr != nullptr)
		return mem_ptr;

	vmaMapMemory(this->device->allocator, this->allocation, &this->mem_ptr);
	return this->mem_ptr;
}

void VulkanBuffer::Unmap()
{
	vmaUnmapMemory(this->device->allocator, this->allocation);
	this->mem_ptr = nullptr;
}

EResourceMemoryUsage VulkanBuffer::GetResourceMemoryUsage()
{
	return m_memoryUsage;
}

EBufferUsage VulkanBuffer::GetUsage()
{
	return m_usage;
}

uint64_t VulkanBuffer::GetSize()
{
	return m_size;
}
