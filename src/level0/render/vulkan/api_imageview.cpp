#include "level0/pch.h"

#include "api.h"

VulkanImageView::VulkanImageView(VulkanDevice *device, std::shared_ptr< VulkanImage > image, eRenderImageViewType imageViewType, eRenderImageFormat imageFormat, const RenderImageComponentMapping &componentMapping, const RenderImageSubresourceRange &subresourceRange)
{
	static const std::array< EImageType, static_cast< size_t >(eRenderImageViewType::MaxEnum) > imageViewTypeToImageType {
		IMAGE_TYPE_1D,
		IMAGE_TYPE_2D,
		IMAGE_TYPE_3D,
		IMAGE_TYPE_CUBEMAP,
		IMAGE_TYPE_1D,
		IMAGE_TYPE_2D,
		IMAGE_TYPE_CUBEMAP,
	};

	this->device = device;
	this->image = std::dynamic_pointer_cast< IRenderImage >(image);
	this->imageType = imageViewTypeToImageType[static_cast< uint32_t >(imageViewType)];
	this->imageViewType = imageViewType;
	this->imageFormat = imageFormat;

	VkImageViewCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.image = image->handle;
	createInfo.viewType = g_VulkanImageViewTypes2[static_cast< uint32_t >(imageViewType)];
	createInfo.format = g_VulkanImageFormats2[static_cast< uint32_t >(imageFormat)];
	createInfo.components.r = g_VulkanComponentSwizzleTypes[static_cast< uint32_t >(componentMapping.r)];
	createInfo.components.g = g_VulkanComponentSwizzleTypes[static_cast< uint32_t >(componentMapping.g)];
	createInfo.components.b = g_VulkanComponentSwizzleTypes[static_cast< uint32_t >(componentMapping.b)];
	createInfo.components.a = g_VulkanComponentSwizzleTypes[static_cast< uint32_t >(componentMapping.a)];
	createInfo.subresourceRange.aspectMask = g_VulkanImageAspectFlags[subresourceRange.aspect];
	createInfo.subresourceRange.baseMipLevel = subresourceRange.baseMipLevel;
	createInfo.subresourceRange.levelCount = subresourceRange.levelCount;
	createInfo.subresourceRange.baseArrayLayer = subresourceRange.baseArrayLayer;
	createInfo.subresourceRange.layerCount = subresourceRange.layerCount;

	VK_CHECK(this->device->ftbl.vkCreateImageView(this->device->handle, &createInfo, nullptr, &this->handle), "VulkanImageView: Failed to create image view!");
}

VulkanImageView::~VulkanImageView()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyImageView(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}

	this->image.reset();
}
