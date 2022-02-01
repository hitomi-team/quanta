#include "level0/pch.h"

#include "api.h"

VulkanImage::VulkanImage(VulkanDevice *device, VmaPool pool, EImageType type, eRenderImageFormat format, EImageUsage usage, uint32_t numMipLevels, uint32_t numArrayLayers, const RenderExtent3D &extent)
{
	this->device = device;
	this->extent2D = { extent.width, extent.height };
	this->extent3D = extent;
	this->numPixels = extent.width*extent.height*extent.depth;

	VkImageCreateInfo imageInfo;
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = nullptr;
	imageInfo.flags = 0;
	imageInfo.imageType = g_VulkanImageTypes[type];
	imageInfo.format = g_VulkanImageFormats2[static_cast< uint32_t >(format)];
	imageInfo.extent = VkExtent3D { extent.width, extent.height, extent.depth };
	imageInfo.mipLevels = numMipLevels;
	imageInfo.arrayLayers = numArrayLayers;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = static_cast< VkImageUsageFlags >(usage);
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.pool = pool;

	if (vmaCreateImage(this->device->allocator, &imageInfo, &allocInfo, &this->handle, &this->allocation, nullptr) != VK_SUCCESS)
		throw std::runtime_error("VulkanImage: Cannot create image handle!");
}

VulkanImage::VulkanImage(VulkanDevice *device, VmaPool pool, EImageType type, eRenderImageFormat format, EImageUsage usage, uint32_t numMipLevels, uint32_t numArrayLayers, const RenderExtent2D &extent)
{
	this->device = device;
	this->extent2D = extent;
	this->extent3D = { extent.width, extent.height, 1 };
	this->numPixels = extent.width*extent.height;

	VkImageCreateInfo imageInfo;
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = nullptr;
	imageInfo.flags = 0;
	imageInfo.imageType = g_VulkanImageTypes[type];
	imageInfo.format = g_VulkanImageFormats2[static_cast< uint32_t >(format)];
	imageInfo.extent = VkExtent3D { extent.width, extent.height, 1 };
	imageInfo.mipLevels = numMipLevels;
	imageInfo.arrayLayers = numArrayLayers;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = static_cast< VkImageUsageFlags >(usage);
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.pool = pool;

	if (vmaCreateImage(this->device->allocator, &imageInfo, &allocInfo, &this->handle, &this->allocation, nullptr) != VK_SUCCESS)
		throw std::runtime_error("VulkanImage: Cannot create image handle!");
}

VulkanImage::~VulkanImage()
{
	if (this->handle != VK_NULL_HANDLE) {
		vmaDestroyImage(this->device->allocator, this->handle, this->allocation);
		this->handle = VK_NULL_HANDLE;
	}
}

void *VulkanImage::Map()
{
	vmaMapMemory(this->device->allocator, this->allocation, &this->mem_ptr);
	return this->mem_ptr;
}

void VulkanImage::Unmap()
{
	vmaUnmapMemory(this->device->allocator, this->allocation);
	this->mem_ptr = nullptr;
}
