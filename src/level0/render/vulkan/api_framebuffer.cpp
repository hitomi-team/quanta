#include "level0/pch.h"

#include "api.h"

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice *device, std::shared_ptr< IRenderPass > renderPass, const std::vector< std::shared_ptr< IRenderImage > > &images, const RenderExtent2D &extent)
{
	this->device = device;

	// do a copy.
	this->images.resize(images.size());
	this->renderPass = std::dynamic_pointer_cast< VulkanRenderPass >(renderPass);

	m_extent = extent;

	std::vector< VkImageView > imageViews(images.size());

	for (size_t i = 0; i < images.size(); i++) {
		this->images[i] = std::dynamic_pointer_cast< VulkanImage >(images[i]);
		imageViews[i] = this->images[i]->imageView;
	}

	VkFramebufferCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.renderPass = this->renderPass->handle;
	createInfo.attachmentCount = static_cast< uint32_t >(imageViews.size());
	createInfo.pAttachments = imageViews.data();
	createInfo.width = extent.width;
	createInfo.height = extent.height;
	createInfo.layers = this->images.size();

	if (this->device->ftbl.vkCreateFramebuffer(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanFramebuffer: Failed to create framebuffer!");
}

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice *device, std::shared_ptr< IRenderPass > renderPass, std::shared_ptr< IRenderImage > image, const RenderExtent2D &extent)
{
	this->device = device;

	// do a copy.
	this->images.push_back(std::dynamic_pointer_cast< VulkanImage >(image));
	this->renderPass = std::dynamic_pointer_cast< VulkanRenderPass >(renderPass);

	m_extent = extent;

	VkFramebufferCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.renderPass = this->renderPass->handle;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &this->images[0]->imageView;
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

std::vector< std::shared_ptr< IRenderImage > > VulkanFramebuffer::GetImages()
{
	std::vector< std::shared_ptr< IRenderImage > > images(this->images.size());
	for (size_t i = 0; i < this->images.size(); i++)
		images[i] = std::dynamic_pointer_cast< IRenderImage >(this->images[i]);
	return images;
}

std::shared_ptr< IRenderPass > VulkanFramebuffer::GetRenderPass()
{
	return std::dynamic_pointer_cast< IRenderPass >(this->renderPass);
}

RenderExtent2D VulkanFramebuffer::GetExtent()
{
	return m_extent;
}

bool VulkanFramebuffer::IsMultiView()
{
	return this->images.size() > 1;
}
