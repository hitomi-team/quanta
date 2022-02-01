#include "level0/pch.h"

#include "api.h"

VulkanRenderPass::VulkanRenderPass(VulkanDevice *device, const std::vector< RenderAttachmentDescription > &attachments, const std::vector< RenderSubpassDescription > &subpasses, const std::vector< RenderSubpassDependency > &subpassDependencies)
{
	this->device = device;

	std::vector< VkAttachmentDescription > _attachments(attachments.size());
	std::vector< VkSubpassDependency > _subpassDependencies(subpassDependencies.size());

	for (size_t i = 0; i < attachments.size(); i++) {
		_attachments[i].flags = 0;
		_attachments[i].format = g_VulkanImageFormats2[static_cast< uint32_t >(attachments[i].format)];
		_attachments[i].samples = VK_SAMPLE_COUNT_1_BIT;
		_attachments[i].loadOp = g_VulkanAttachmentLoadOps[attachments[i].loadOp];
		_attachments[i].storeOp = g_VulkanAttachmentStoreOps[attachments[i].storeOp];
		_attachments[i].stencilLoadOp = g_VulkanAttachmentLoadOps[attachments[i].stencilLoadOp];
		_attachments[i].stencilStoreOp = g_VulkanAttachmentStoreOps[attachments[i].stencilStoreOp];
		_attachments[i].initialLayout = g_VulkanImageLayouts[attachments[i].initialLayout];
		_attachments[i].finalLayout = g_VulkanImageLayouts[attachments[i].finalLayout];
	}

	for (size_t i = 0; i < subpassDependencies.size(); i++) {
		_subpassDependencies[i].srcSubpass = subpassDependencies[i].sourceSubpass;
		_subpassDependencies[i].dstSubpass = subpassDependencies[i].destSubpass;
		_subpassDependencies[i].srcStageMask = static_cast< VkPipelineStageFlags >(subpassDependencies[i].sourceStageMask);
		_subpassDependencies[i].dstStageMask = static_cast< VkPipelineStageFlags >(subpassDependencies[i].destStageMask);
		_subpassDependencies[i].srcAccessMask = static_cast< VkAccessFlags >(subpassDependencies[i].sourceAccessMask);
		_subpassDependencies[i].dstAccessMask = static_cast< VkAccessFlags >(subpassDependencies[i].destAccessMask);
		_subpassDependencies[i].dependencyFlags = static_cast< VkDependencyFlags >(subpassDependencies[i].dependencyFlags);
	}

	// resize storage
	size_t totalInputs = 0, totalColors = 0, totalDepthStencils = 0;
	for (const auto &subpass : subpasses) {
		totalInputs += subpass.inputAttachments.size();
		totalColors += subpass.colorAttachments.size();
		totalDepthStencils += subpass.depthStencilEnable ? 1 : 0;
	}

	std::vector< std::vector< VkAttachmentReference > > refsInput(totalInputs), refsColor(totalColors);
	std::vector< std::shared_ptr< VkAttachmentReference > > refsDepthStencil(totalDepthStencils);
	totalDepthStencils = 0;

	std::vector< VkSubpassDescription > _subpasses(subpasses.size());

	for (size_t i = 0; i < subpasses.size(); i++) {
		std::vector< VkAttachmentReference > inputAttachments(subpasses[i].inputAttachments.size()), colorAttachments(subpasses[i].colorAttachments.size());
		std::shared_ptr< VkAttachmentReference > dsAttachment = nullptr;

		for (size_t j = 0; j < subpasses[i].inputAttachments.size(); j++) {
			inputAttachments[j].attachment = subpasses[i].inputAttachments[j].attachment;
			inputAttachments[j].layout = g_VulkanImageLayouts[subpasses[i].inputAttachments[j].layout];
		}

		for (size_t j = 0; j < subpasses[i].colorAttachments.size(); j++) {
			colorAttachments[j].attachment = subpasses[i].colorAttachments[j].attachment;
			colorAttachments[j].layout = g_VulkanImageLayouts[subpasses[i].colorAttachments[j].layout];
		}

		if (subpasses[i].depthStencilEnable) {
			dsAttachment = std::make_shared< VkAttachmentReference >();
			dsAttachment->attachment = subpasses[i].depthStencilAttachment.attachment;
			dsAttachment->layout = g_VulkanImageLayouts[subpasses[i].depthStencilAttachment.layout];
			refsDepthStencil[totalDepthStencils++] = dsAttachment;
		}

		_subpasses[i].flags = 0;
		_subpasses[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		_subpasses[i].inputAttachmentCount = static_cast< uint32_t >(inputAttachments.size());
		_subpasses[i].pInputAttachments = inputAttachments.data();
		_subpasses[i].colorAttachmentCount = static_cast< uint32_t >(colorAttachments.size());
		_subpasses[i].pColorAttachments = colorAttachments.data();
		_subpasses[i].pResolveAttachments = nullptr;
		_subpasses[i].pDepthStencilAttachment = dsAttachment.get();
		_subpasses[i].preserveAttachmentCount = static_cast< uint32_t >(subpasses[i].preserveAttachments.size());
		_subpasses[i].pPreserveAttachments = subpasses[i].preserveAttachments.data();

		if (inputAttachments.size() != 0)
			refsInput[i] = std::move(inputAttachments);
		if (colorAttachments.size() != 0)
			refsColor[i] = std::move(colorAttachments);
	}

	VkRenderPassCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.attachmentCount = static_cast< uint32_t >(_attachments.size());
	createInfo.pAttachments = _attachments.data();
	createInfo.subpassCount = static_cast< uint32_t >(_subpasses.size());
	createInfo.pSubpasses = _subpasses.data();
	createInfo.dependencyCount = static_cast< uint32_t >(_subpassDependencies.size());
	createInfo.pDependencies = _subpassDependencies.data();

	if (this->device->ftbl.vkCreateRenderPass(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanRenderPass: Failed to create render pass!");
}

VulkanRenderPass::~VulkanRenderPass()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyRenderPass(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}
