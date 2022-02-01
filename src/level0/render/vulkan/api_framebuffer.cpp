#include "level0/pch.h"

#include "api.h"

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice *device, std::shared_ptr< IRenderPass > renderPass, const std::vector< std::shared_ptr< IRenderImageView > > &images, const RenderExtent2D &extent)
{
	this->device = device;

	std::vector< VkImageView > imageViews(images.size());

	for (size_t i = 0; i < images.size(); i++)
		imageViews[i] = std::dynamic_pointer_cast< VulkanImageView >(images[i])->handle;

	VkFramebufferCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.renderPass = std::dynamic_pointer_cast< VulkanRenderPass >(renderPass)->handle;
	createInfo.attachmentCount = static_cast< uint32_t >(imageViews.size());
	createInfo.pAttachments = imageViews.data();
	createInfo.width = extent.width;
	createInfo.height = extent.height;
	createInfo.layers = this->images.size();

	if (this->device->ftbl.vkCreateFramebuffer(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanFramebuffer: Failed to create framebuffer!");
}

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice *device, std::shared_ptr< IRenderPass > renderPass, std::shared_ptr< IRenderImageView > image, const RenderExtent2D &extent)
{
	this->device = device;

	VkFramebufferCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.renderPass = std::dynamic_pointer_cast< VulkanRenderPass >(renderPass)->handle;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &std::dynamic_pointer_cast< VulkanImageView >(image)->handle;
	createInfo.width = extent.width;
	createInfo.height = extent.height;
	createInfo.layers = this->images.size();

	if (this->device->ftbl.vkCreateFramebuffer(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanFramebuffer: Failed to create framebuffer!");
}

VulkanFramebuffer::~VulkanFramebuffer()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyFramebuffer(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}
