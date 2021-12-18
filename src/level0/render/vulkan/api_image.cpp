#include "level0/pch.h"

#include "api.h"

const std::array< VkImageAspectFlags, MAX_IMAGE_ASPECT_ENUM > g_VulkanImageAspectFlags {
	VK_IMAGE_ASPECT_COLOR_BIT,
	VK_IMAGE_ASPECT_DEPTH_BIT,
	VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
	VK_IMAGE_ASPECT_STENCIL_BIT
};

const std::array< VkImageType, MAX_IMAGE_TYPE_ENUM > g_VulkanImageTypes {
	VK_IMAGE_TYPE_1D,
	VK_IMAGE_TYPE_2D,
	VK_IMAGE_TYPE_3D,
	VK_IMAGE_TYPE_2D
};

const std::array< VkImageViewType, MAX_IMAGE_TYPE_ENUM > g_VulkanImageViewTypes {
	VK_IMAGE_VIEW_TYPE_1D,
	VK_IMAGE_VIEW_TYPE_2D,
	VK_IMAGE_VIEW_TYPE_3D,
	VK_IMAGE_VIEW_TYPE_CUBE
};

const std::array< VulkanFormatAlphaProperties, MAX_IMAGE_FORMAT_ENUM > g_VulkanImageFormats {
	VulkanFormatAlphaProperties { VK_FORMAT_UNDEFINED, false },

	VulkanFormatAlphaProperties { VK_FORMAT_R8_UINT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_R8G8_UINT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_R8G8B8_UINT, true }, // RGBX8888
	VulkanFormatAlphaProperties { VK_FORMAT_R8G8B8A8_UINT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_R8G8B8A8_SRGB, false },
	VulkanFormatAlphaProperties { VK_FORMAT_R8G8B8A8_UINT, true }, // RGBX8888
	VulkanFormatAlphaProperties { VK_FORMAT_R8G8B8A8_SRGB, true }, // RGBX8888

	VulkanFormatAlphaProperties { VK_FORMAT_R16_SFLOAT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_R32_SFLOAT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_R16G16_SFLOAT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_R32G32_SFLOAT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_R16G16B16_SFLOAT, true }, // RGBX32323232
	VulkanFormatAlphaProperties { VK_FORMAT_R32G32B32_SFLOAT, true }, // RGBX32323232
	VulkanFormatAlphaProperties { VK_FORMAT_R16G16B16A16_SFLOAT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_R32G32B32A32_SFLOAT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_R16G16B16A16_SFLOAT, true }, // RGBX16161616
	VulkanFormatAlphaProperties { VK_FORMAT_R32G32B32A32_SFLOAT, true }, // RGBX32323232

	VulkanFormatAlphaProperties { VK_FORMAT_B8G8R8_UINT, true }, // BGRX8888
	VulkanFormatAlphaProperties { VK_FORMAT_B8G8R8A8_UINT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_B8G8R8A8_SRGB, false },
	VulkanFormatAlphaProperties { VK_FORMAT_B8G8R8A8_UINT, true }, // BGRX8888
	VulkanFormatAlphaProperties { VK_FORMAT_B8G8R8A8_SRGB, true }, // BGRX8888

	VulkanFormatAlphaProperties { VK_FORMAT_D16_UNORM, false },
	VulkanFormatAlphaProperties { VK_FORMAT_D32_SFLOAT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_S8_UINT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_D16_UNORM_S8_UINT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_D24_UNORM_S8_UINT, false },
	VulkanFormatAlphaProperties { VK_FORMAT_X8_D24_UNORM_PACK32, false },
	VulkanFormatAlphaProperties { VK_FORMAT_D32_SFLOAT_S8_UINT, false },
};

VulkanImage::VulkanImage(VulkanDevice *device, VmaPool pool, EResourceMemoryUsage memoryUsage, EImageType type, EImageFormat format, EImageUsage usage, const RenderExtent3D &extent, const RenderImageSubresourceRange &subresourceRange)
{
	this->device = device;

	m_memoryUsage = memoryUsage;
	m_type = type;
	m_format = format;
	m_usage = usage;
	m_extent = extent;
	m_subresourceRange = subresourceRange;

	VkImageCreateInfo imageInfo;
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = nullptr;
	imageInfo.flags = 0;
	imageInfo.imageType = g_VulkanImageTypes[m_type];
	imageInfo.format = g_VulkanImageFormats[m_format].format;
	imageInfo.extent = VkExtent3D { extent.width, extent.height, extent.depth };
	imageInfo.mipLevels = subresourceRange.levelCount;
	imageInfo.arrayLayers = subresourceRange.layerCount;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = static_cast< VkImageUsageFlags >(m_usage);
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.pool = pool;

	if (vmaCreateImage(this->device->allocator, &imageInfo, &allocInfo, &this->handle, &this->allocation, nullptr) != VK_SUCCESS)
		throw std::runtime_error("VulkanImage: Cannot create image handle!");

	VkImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = nullptr;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = this->handle;
	imageViewCreateInfo.viewType = g_VulkanImageViewTypes[m_type];
	imageViewCreateInfo.format = g_VulkanImageFormats[m_format].format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = g_VulkanImageFormats[m_format].ignoreAlpha ? VK_COMPONENT_SWIZZLE_ONE : VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = g_VulkanImageAspectFlags[m_subresourceRange.aspect];
	imageViewCreateInfo.subresourceRange.baseMipLevel = m_subresourceRange.baseMipLevel;
	imageViewCreateInfo.subresourceRange.levelCount = m_subresourceRange.levelCount;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = m_subresourceRange.baseArrayLayer;
	imageViewCreateInfo.subresourceRange.layerCount = m_subresourceRange.layerCount;

	if (this->device->ftbl.vkCreateImageView(this->device->handle, &imageViewCreateInfo, nullptr, &this->imageView) != VK_SUCCESS)
		throw std::runtime_error("VulkanImage: Cannot create image view!");
}

VulkanImage::VulkanImage(VulkanDevice *device, VkImage image, EImageFormat format, const RenderExtent2D &extent, const RenderImageSubresourceRange &subresourceRange)
{
	this->device = device;
	this->handle = image;

	m_memoryUsage = RESOURCE_MEMORY_USAGE_NONE;
	m_type = IMAGE_TYPE_2D;
	m_format = format;
	m_usage = IMAGE_USAGE_COLOR_ATTACHMENT;
	m_extent = { extent.width, extent.height, 1 };
	m_subresourceRange = subresourceRange;

	VkImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = nullptr;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = this->handle;
	imageViewCreateInfo.viewType = g_VulkanImageViewTypes[m_type];
	imageViewCreateInfo.format = g_VulkanImageFormats[m_format].format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = g_VulkanImageAspectFlags[m_subresourceRange.aspect];
	imageViewCreateInfo.subresourceRange.baseMipLevel = m_subresourceRange.baseMipLevel;
	imageViewCreateInfo.subresourceRange.levelCount = m_subresourceRange.levelCount;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = m_subresourceRange.baseArrayLayer;
	imageViewCreateInfo.subresourceRange.layerCount = m_subresourceRange.layerCount;

	if (this->device->ftbl.vkCreateImageView(this->device->handle, &imageViewCreateInfo, nullptr, &this->imageView) != VK_SUCCESS)
		throw std::runtime_error("VulkanImage: Cannot create image view!");

	this->swapchainImage = true;
}

VulkanImage::~VulkanImage()
{
	if (this->imageView != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyImageView(this->device->handle, this->imageView, nullptr);
		this->imageView = VK_NULL_HANDLE;
	}

	if (this->handle != VK_NULL_HANDLE) {
		if (this->swapchainImage)
			return;

		vmaDestroyImage(this->device->allocator, this->handle, this->allocation);
		this->handle = VK_NULL_HANDLE;
	}
}

void *VulkanImage::Map()
{
	if (this->swapchainImage)
		return nullptr;

	if (mem_ptr != nullptr)
		return mem_ptr;

	vmaMapMemory(this->device->allocator, this->allocation, &this->mem_ptr);
	return this->mem_ptr;
}

void VulkanImage::Unmap()
{
	if (this->swapchainImage)
		return;

	vmaUnmapMemory(this->device->allocator, this->allocation);
	this->mem_ptr = nullptr;
}

EResourceMemoryUsage VulkanImage::GetResourceMemoryUsage()
{
	return m_memoryUsage;
}

EImageUsage VulkanImage::GetUsage()
{
	return m_usage;
}

EImageFormat VulkanImage::GetFormat()
{
	return m_format;
}

RenderExtent3D VulkanImage::GetExtent()
{
	return m_extent;
}

RenderImageSubresourceRange VulkanImage::GetSubresourceRange()
{
	return m_subresourceRange;
}
