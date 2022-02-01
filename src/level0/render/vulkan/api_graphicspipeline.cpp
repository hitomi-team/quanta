#include "level0/pch.h"

#include "api.h"

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice *device, const std::vector< std::shared_ptr< IRenderShaderModule > > &shaderModules, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderGraphicsPipeline > basePipeline, std::shared_ptr< IRenderPass > renderPass, const RenderGraphicsPipelineDesc &desc, uint32_t subpass)
{
	this->device = device;

	std::vector< VkVertexInputBindingDescription > vertexInputBindingDescriptions(desc.vertexInputState.bindings.size());

	for (size_t i = 0; i < desc.vertexInputState.bindings.size(); i++) {
		vertexInputBindingDescriptions[i].binding = desc.vertexInputState.bindings[i].binding;
		vertexInputBindingDescriptions[i].stride = desc.vertexInputState.bindings[i].stride;
		vertexInputBindingDescriptions[i].inputRate = g_VulkanVertexInputRates[desc.vertexInputState.bindings[i].inputRate];
	}

	std::vector< VkVertexInputAttributeDescription > vertexInputAttributeDescriptions(desc.vertexInputState.attributes.size());

	for (size_t i = 0; i < desc.vertexInputState.attributes.size(); i++) {
		vertexInputAttributeDescriptions[i].location = desc.vertexInputState.attributes[i].location;
		vertexInputAttributeDescriptions[i].binding = desc.vertexInputState.attributes[i].binding;
		vertexInputAttributeDescriptions[i].format = g_VulkanImageFormats2[static_cast< uint32_t >(desc.vertexInputState.attributes[i].format)];
		vertexInputAttributeDescriptions[i].offset = desc.vertexInputState.attributes[i].offset;
	}

	std::vector< VkViewport > viewports(desc.viewportState.viewports.size());

	for (size_t i = 0; i < desc.viewportState.viewports.size(); i++) {
		viewports[i].x = desc.viewportState.viewports[i].x;
		viewports[i].y = desc.viewportState.viewports[i].y;
		viewports[i].width = desc.viewportState.viewports[i].width;
		viewports[i].height = desc.viewportState.viewports[i].height;
		viewports[i].minDepth = desc.viewportState.viewports[i].minDepth;
		viewports[i].maxDepth = desc.viewportState.viewports[i].maxDepth;
	}

	std::vector< VkRect2D > scissors(desc.viewportState.scissors.size());

	for (size_t i = 0; i < desc.viewportState.scissors.size(); i++) {
		scissors[i].offset = { desc.viewportState.scissors[i].offset.x, desc.viewportState.scissors[i].offset.y };
		scissors[i].extent = { desc.viewportState.scissors[i].extent.width, desc.viewportState.scissors[i].extent.height };
	}

	std::vector< VkPipelineColorBlendAttachmentState > blendAttachmentStates(desc.colorBlendState.attachments.size());

	for (size_t i = 0; i < desc.colorBlendState.attachments.size(); i++) {
		blendAttachmentStates[i].blendEnable = desc.colorBlendState.attachments[i].enable ? VK_TRUE : VK_FALSE;
		blendAttachmentStates[i].srcColorBlendFactor = g_VulkanBlendFactors[desc.colorBlendState.attachments[i].sourceColorBlendFactor];
		blendAttachmentStates[i].dstColorBlendFactor = g_VulkanBlendFactors[desc.colorBlendState.attachments[i].destColorBlendFactor];
		blendAttachmentStates[i].colorBlendOp = g_VulkanBlendOps[desc.colorBlendState.attachments[i].colorBlendOp];
		blendAttachmentStates[i].srcAlphaBlendFactor = g_VulkanBlendFactors[desc.colorBlendState.attachments[i].sourceAlphaBlendFactor];
		blendAttachmentStates[i].dstAlphaBlendFactor = g_VulkanBlendFactors[desc.colorBlendState.attachments[i].destAlphaBlendFactor];
		blendAttachmentStates[i].alphaBlendOp = g_VulkanBlendOps[desc.colorBlendState.attachments[i].alphaBlendOp];
		blendAttachmentStates[i].colorWriteMask = static_cast< VkColorComponentFlags >(desc.colorBlendState.attachments[i].colorWriteMask);
	}

	std::vector< VkPipelineShaderStageCreateInfo > shaderStages(shaderModules.size());

	for (size_t i = 0; i < shaderModules.size(); i++) {
		shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[i].pNext = nullptr;
		shaderStages[i].flags = 0;
		shaderStages[i].stage = g_VulkanShaderTypes[shaderModules[i]->GetType()];
		shaderStages[i].module = std::dynamic_pointer_cast< VulkanShaderModule >(shaderModules[i])->handle;
		shaderStages[i].pName = "main";
		shaderStages[i].pSpecializationInfo = nullptr;
	}

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.pNext = nullptr;
	vertexInputStateCreateInfo.flags = 0;
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast< uint32_t >(vertexInputBindingDescriptions.size());
	vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast< uint32_t >(vertexInputAttributeDescriptions.size());
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.pNext = nullptr;
	inputAssemblyStateCreateInfo.flags = 0;
	inputAssemblyStateCreateInfo.topology = g_VulkanPrimitiveTypes[desc.inputAssemblyState.primitiveType];
	inputAssemblyStateCreateInfo.primitiveRestartEnable = desc.inputAssemblyState.primitiveRestartEnable ? VK_TRUE : VK_FALSE;

	VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo;
	tessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	tessellationStateCreateInfo.pNext = nullptr;
	tessellationStateCreateInfo.flags = 0;
	tessellationStateCreateInfo.patchControlPoints = desc.tessellationState.patchControlPoints;

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.pNext = nullptr;
	viewportStateCreateInfo.flags = 0;
	viewportStateCreateInfo.viewportCount = static_cast< uint32_t >(viewports.size());
	viewportStateCreateInfo.pViewports = viewports.data();
	viewportStateCreateInfo.scissorCount = static_cast< uint32_t >(scissors.size());
	viewportStateCreateInfo.pScissors = scissors.data();

	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
	rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateCreateInfo.pNext = nullptr;
	rasterizationStateCreateInfo.flags = 0;
	rasterizationStateCreateInfo.depthClampEnable = desc.rasterizationState.depthClampEnable ? VK_TRUE : VK_FALSE;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = desc.rasterizationState.rasterizerDiscardEnable ? VK_TRUE : VK_FALSE;
	rasterizationStateCreateInfo.polygonMode = g_VulkanPolygonModes[desc.rasterizationState.fillMode];
	rasterizationStateCreateInfo.cullMode = g_VulkanCullModes[desc.rasterizationState.cullMode];
	rasterizationStateCreateInfo.frontFace = g_VulkanFrontFaces[desc.rasterizationState.frontFace];
	rasterizationStateCreateInfo.depthBiasEnable = desc.rasterizationState.depthBiasEnable ? VK_TRUE : VK_FALSE;
	rasterizationStateCreateInfo.depthBiasConstantFactor = desc.rasterizationState.depthBiasConstantFactor;
	rasterizationStateCreateInfo.depthBiasClamp = desc.rasterizationState.depthBiasClamp;
	rasterizationStateCreateInfo.depthBiasSlopeFactor = desc.rasterizationState.depthBiasSlopeFactor;
	rasterizationStateCreateInfo.lineWidth = desc.rasterizationState.lineWidth;

	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
	multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleStateCreateInfo.pNext = nullptr;
	multisampleStateCreateInfo.flags = 0;
	multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleStateCreateInfo.minSampleShading = 0.f;
	multisampleStateCreateInfo.pSampleMask = nullptr;
	multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
	depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilStateCreateInfo.pNext = nullptr;
	depthStencilStateCreateInfo.flags = 0;
	depthStencilStateCreateInfo.depthTestEnable = desc.depthStencilState.depthTestEnable ? VK_TRUE : VK_FALSE;
	depthStencilStateCreateInfo.depthWriteEnable = desc.depthStencilState.depthWriteEnable ? VK_TRUE : VK_FALSE;
	depthStencilStateCreateInfo.depthCompareOp = g_VulkanSamplerCompareOps[desc.depthStencilState.compareMode];
	depthStencilStateCreateInfo.depthBoundsTestEnable = desc.depthStencilState.depthBoundsTestEnable ? VK_TRUE : VK_FALSE;
	depthStencilStateCreateInfo.stencilTestEnable = desc.depthStencilState.stencilTestEnable ? VK_TRUE : VK_FALSE;
	depthStencilStateCreateInfo.front.failOp = g_VulkanStencilOps[desc.depthStencilState.front.failOp];
	depthStencilStateCreateInfo.front.passOp = g_VulkanStencilOps[desc.depthStencilState.front.passOp];
	depthStencilStateCreateInfo.front.depthFailOp = g_VulkanStencilOps[desc.depthStencilState.front.depthFailOp];
	depthStencilStateCreateInfo.front.compareOp = g_VulkanSamplerCompareOps[desc.depthStencilState.front.compareMode];
	depthStencilStateCreateInfo.front.compareMask = desc.depthStencilState.front.compareMask;
	depthStencilStateCreateInfo.front.writeMask = desc.depthStencilState.front.writeMask;
	depthStencilStateCreateInfo.front.reference = desc.depthStencilState.front.reference;
	depthStencilStateCreateInfo.back.failOp = g_VulkanStencilOps[desc.depthStencilState.back.failOp];
	depthStencilStateCreateInfo.back.passOp = g_VulkanStencilOps[desc.depthStencilState.back.passOp];
	depthStencilStateCreateInfo.back.depthFailOp = g_VulkanStencilOps[desc.depthStencilState.back.depthFailOp];
	depthStencilStateCreateInfo.back.compareOp = g_VulkanSamplerCompareOps[desc.depthStencilState.back.compareMode];
	depthStencilStateCreateInfo.back.compareMask = desc.depthStencilState.back.compareMask;
	depthStencilStateCreateInfo.back.writeMask = desc.depthStencilState.back.writeMask;
	depthStencilStateCreateInfo.back.reference = desc.depthStencilState.back.reference;
	depthStencilStateCreateInfo.minDepthBounds = desc.depthStencilState.minDepthBounds;
	depthStencilStateCreateInfo.maxDepthBounds = desc.depthStencilState.maxDepthBounds;

	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
	colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStateCreateInfo.pNext = nullptr;
	colorBlendStateCreateInfo.flags = 0;
	colorBlendStateCreateInfo.logicOpEnable = desc.colorBlendState.logicOpEnable ? VK_TRUE : VK_FALSE;
	colorBlendStateCreateInfo.logicOp = g_VulkanLogicOps[desc.colorBlendState.logicOp];
	colorBlendStateCreateInfo.attachmentCount = static_cast< uint32_t >(blendAttachmentStates.size());
	colorBlendStateCreateInfo.pAttachments = blendAttachmentStates.data();
	std::copy(desc.colorBlendState.blendConstants, desc.colorBlendState.blendConstants + 4, colorBlendStateCreateInfo.blendConstants);

	VkGraphicsPipelineCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.stageCount = static_cast< uint32_t >(shaderStages.size());
	createInfo.pStages = shaderStages.data();
	createInfo.pVertexInputState = &vertexInputStateCreateInfo;
	createInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	createInfo.pTessellationState = &tessellationStateCreateInfo;
	createInfo.pViewportState = &viewportStateCreateInfo;
	createInfo.pRasterizationState = &rasterizationStateCreateInfo;
	createInfo.pMultisampleState = &multisampleStateCreateInfo;
	createInfo.pDepthStencilState = &depthStencilStateCreateInfo;
	createInfo.pColorBlendState = &colorBlendStateCreateInfo;
	createInfo.pDynamicState = nullptr;
	createInfo.layout = std::dynamic_pointer_cast< VulkanPipelineLayout >(pipelineLayout)->handle;
	createInfo.renderPass = std::dynamic_pointer_cast< VulkanRenderPass >(renderPass)->handle;
	createInfo.subpass = subpass;
	createInfo.basePipelineHandle = basePipeline != nullptr ? std::dynamic_pointer_cast< VulkanGraphicsPipeline >(basePipeline)->handle : VK_NULL_HANDLE;
	createInfo.basePipelineIndex = basePipeline != nullptr ? -1 : 0;

	if (this->device->ftbl.vkCreateGraphicsPipelines(this->device->handle, VK_NULL_HANDLE, 1, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanGraphicsPipeline: Failed to create graphics pipeline!");

}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyPipeline(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}
