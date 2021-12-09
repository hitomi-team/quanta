#include "level0/pch.h"

#include "api.h"

static inline uint32_t count_bits(uint32_t v)
{
	uint32_t c = v - ((v >> 1) & 0x55555555);
	c = ((c >>  2) & 0x33333333) + (c & 0x33333333);
	c = ((c >>  4) + c) & 0x0F0F0F0F;
	c = ((c >>  8) + c) & 0x00FF00FF;
	c = ((c >> 16) + c) & 0x0000FFFF;
	return c;
}

VulkanAllocator::VulkanAllocator(VulkanDevice *device, const std::string &name, EResourceMemoryUsage memoryUsage, uint64_t minAlign, uint64_t blockSize, size_t minBlockCount, size_t maxBlockCount)
{
	this->name = name;
	this->device = device;
	m_memoryUsage = memoryUsage;

	VkMemoryPropertyFlags requiredFlags = 0, preferredFlags = 0, mask = 0;

	switch (m_memoryUsage) {
	case RESOURCE_MEMORY_USAGE_NONE:
		break;
	case RESOURCE_MEMORY_USAGE_CPU_ONLY:
		requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
	case RESOURCE_MEMORY_USAGE_CPU_COPY:
		preferredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		mask = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case RESOURCE_MEMORY_USAGE_CPU_TO_DEVICE:
	case RESOURCE_MEMORY_USAGE_DEVICE_TO_CPU:
		requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

		if (memoryUsage == RESOURCE_MEMORY_USAGE_DEVICE_TO_CPU)
			preferredFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

		if (memoryUsage == RESOURCE_MEMORY_USAGE_CPU_TO_DEVICE)
			if (this->device->phy->cachedInfo.hardware.type == PHYSICAL_DEVICE_TYPE_INTEGRATED)
				preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case RESOURCE_MEMORY_USAGE_DEVICE_ONLY:
		preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	default:
		throw std::runtime_error("VulkanAllocator: Invalid Resource Usage!");
	}

	uint32_t memoryTypeIndex = UINT32_MAX, lastCost = UINT32_MAX;

	for (uint32_t i = 0; i < this->device->phy->memProperties.memoryTypeCount; i++) {
		VkMemoryPropertyFlags flags = this->device->phy->memProperties.memoryTypes[i].propertyFlags;
		if (flags & requiredFlags) {
			uint32_t cost = count_bits(preferredFlags & ~flags) + count_bits(flags & mask);
			if (cost < lastCost) {
				memoryTypeIndex = i;
				cost = lastCost;
			}
		}
	}

	if (memoryTypeIndex == UINT32_MAX)
		throw std::runtime_error("VulkanAllocator: Cannot find requested memory type index!");

	VmaPoolCreateInfo createInfo = {};
	createInfo.memoryTypeIndex = memoryTypeIndex;
	createInfo.flags = VMA_POOL_CREATE_BUDDY_ALGORITHM_BIT;
	createInfo.blockSize = blockSize;
	createInfo.minBlockCount = minBlockCount;
	createInfo.maxBlockCount = maxBlockCount;
	createInfo.minAllocationAlignment = minAlign;

	if (vmaCreatePool(this->device->allocator, &createInfo, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanAllocator: Cannot create memory pool!");
}

VulkanAllocator::~VulkanAllocator()
{
	if (this->handle != VK_NULL_HANDLE) {
		vmaDestroyPool(this->device->allocator, this->handle);
		this->handle = VK_NULL_HANDLE;
	}
}

EResourceMemoryUsage VulkanAllocator::GetResourceMemoryUsage()
{
	return m_memoryUsage;
}

std::shared_ptr< IRenderBuffer > VulkanAllocator::AllocateBuffer(EBufferUsage usage, uint64_t size)
{
	return std::dynamic_pointer_cast< IRenderBuffer >(std::make_shared< VulkanBuffer >(this->device, this->handle, m_memoryUsage, usage, size));
}

std::shared_ptr< IRenderImage > VulkanAllocator::AllocateImage(EImageType type, EImageFormat format, EImageUsage usage, RenderExtent3D extent3d, const RenderImageSubresourceRange &range)
{
	return std::dynamic_pointer_cast< IRenderImage >(std::make_shared< VulkanImage >(this->device, this->handle, m_memoryUsage, type, format, usage, extent3d, range));
}

void VulkanAllocator::Compactify()
{
}

#ifndef NDEBUG
void VulkanAllocator::DumpInfo(const std::string &filename)
{
	(void)filename;
}
#endif
