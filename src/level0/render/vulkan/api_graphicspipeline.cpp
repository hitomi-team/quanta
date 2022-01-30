#include "level0/pch.h"

#include "api.h"

#include "level0/crypto/md5.h"

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice *device, const std::vector< std::shared_ptr< IRenderShaderModule > > &shaderModules, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderGraphicsPipeline > basePipeline, std::shared_ptr< IRenderPass > renderPass, uint32_t subpass)
{
	this->device = device;

	m_shaderModules.resize(shaderModules.size());

	for (size_t i = 0; i < m_shaderModules.size(); i++)
		m_shaderModules[i] = std::dynamic_pointer_cast< VulkanShaderModule >(shaderModules[i]);

	m_pipelineLayout = std::dynamic_pointer_cast< VulkanPipelineLayout >(pipelineLayout);
	m_basePipeline = std::dynamic_pointer_cast< VulkanGraphicsPipeline >(basePipeline);
	m_renderPass = std::dynamic_pointer_cast< VulkanRenderPass >(renderPass);
	m_subpass = subpass;
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	for (auto &hash : m_hashmap) {
		if (hash.pipeline != VK_NULL_HANDLE) {
			this->device->ftbl.vkDestroyPipeline(this->device->handle, hash.pipeline, nullptr);
			hash.pipeline = VK_NULL_HANDLE;
		}
	}

	if (this->handle != VK_NULL_HANDLE)
		this->handle = VK_NULL_HANDLE;
}

#define SizeInBytesVector(x) (x.size()*sizeof(decltype(x)::value_type))

void VulkanGraphicsPipeline::Compile()
{
	uint8_t checksum[16];

	{
		auto md5Ctx = std::make_unique< MD5Context >();
		md5Init(md5Ctx.get());
		md5Update(md5Ctx.get(), reinterpret_cast< const uint8_t * >(this->vertexInputState.bindings.data()), SizeInBytesVector(this->vertexInputState.bindings));
		md5Update(md5Ctx.get(), reinterpret_cast< const uint8_t * >(this->vertexInputState.attributes.data()), SizeInBytesVector(this->vertexInputState.attributes));
		md5Update(md5Ctx.get(), reinterpret_cast< const uint8_t * >(&this->inputAssemblyState), sizeof(this->inputAssemblyState));
		md5Update(md5Ctx.get(), reinterpret_cast< const uint8_t * >(&this->tessellationState), sizeof(this->tessellationState));
		md5Update(md5Ctx.get(), reinterpret_cast< const uint8_t * >(this->viewportState.viewports.data()), SizeInBytesVector(this->viewportState.viewports));
		md5Update(md5Ctx.get(), reinterpret_cast< const uint8_t * >(this->viewportState.scissors.data()), SizeInBytesVector(this->viewportState.scissors));
		md5Update(md5Ctx.get(), reinterpret_cast< const uint8_t * >(&this->rasterizationState), sizeof(this->rasterizationState));
		md5Update(md5Ctx.get(), reinterpret_cast< const uint8_t * >(&this->depthStencilState), sizeof(this->depthStencilState));
		md5Update(md5Ctx.get(), reinterpret_cast< const uint8_t * >(&this->colorBlendState), sizeof(this->colorBlendState));
		md5Update(md5Ctx.get(), reinterpret_cast< const uint8_t * >(this->colorBlendAttachments.data()), SizeInBytesVector(this->colorBlendAttachments));
		md5Finalize(md5Ctx.get(), checksum);
	}

	for (auto &hash : m_hashmap) {
		if (SDL_memcmp(hash.checksum, checksum, sizeof(checksum)) == 0) {
			this->handle = hash.pipeline;
			return;
		}
	}

	std::vector< VkVertexInputBindingDescription > vertexInputBindingDescriptions(this->vertexInputState.bindings.size());

	for (size_t i = 0; i < this->vertexInputState.bindings.size(); i++) {
		vertexInputBindingDescriptions[i].binding = this->vertexInputState.bindings[i].binding;
		vertexInputBindingDescriptions[i].stride = this->vertexInputState.bindings[i].stride;
		vertexInputBindingDescriptions[i].inputRate = g_VulkanVertexInputRates[this->vertexInputState.bindings[i].inputRate];
	}

	std::vector< VkVertexInputAttributeDescription > vertexInputAttributeDescriptions(this->vertexInputState.attributes.size());

	for (size_t i = 0; i < this->vertexInputState.attributes.size(); i++) {
		vertexInputAttributeDescriptions[i].location = this->vertexInputState.attributes[i].location;
		vertexInputAttributeDescriptions[i].binding = this->vertexInputState.attributes[i].binding;
		vertexInputAttributeDescriptions[i].format = g_VulkanImageFormats[this->vertexInputState.attributes[i].format].format;
		vertexInputAttributeDescriptions[i].offset = this->vertexInputState.attributes[i].offset;
	}

	std::vector< VkViewport > viewports(this->viewportState.viewports.size());

	for (size_t i = 0; i < this->viewportState.viewports.size(); i++) {
		viewports[i].x = this->viewportState.viewports[i].x;
		viewports[i].y = this->viewportState.viewports[i].y;
		viewports[i].width = this->viewportState.viewports[i].width;
		viewports[i].height = this->viewportState.viewports[i].height;
		viewports[i].minDepth = this->viewportState.viewports[i].minDepth;
		viewports[i].maxDepth = this->viewportState.viewports[i].maxDepth;
	}

	std::vector< VkRect2D > scissors(this->viewportState.scissors.size());

	for (size_t i = 0; i < this->viewportState.scissors.size(); i++) {
		scissors[i].offset = { this->viewportState.scissors[i].offset.x, this->viewportState.scissors[i].offset.y };
		scissors[i].extent = { this->viewportState.scissors[i].extent.width, this->viewportState.scissors[i].extent.height };
	}

	std::vector< VkPipelineColorBlendAttachmentState > blendAttachmentStates(this->colorBlendAttachments.size());

	for (size_t i = 0; i < this->colorBlendAttachments.size(); i++) {
		blendAttachmentStates[i].blendEnable = this->colorBlendAttachments[i].enable ? VK_TRUE : VK_FALSE;
		blendAttachmentStates[i].srcColorBlendFactor = g_VulkanBlendFactors[this->colorBlendAttachments[i].sourceColorBlendFactor];
		blendAttachmentStates[i].dstColorBlendFactor = g_VulkanBlendFactors[this->colorBlendAttachments[i].destColorBlendFactor];
		blendAttachmentStates[i].colorBlendOp = g_VulkanBlendOps[this->colorBlendAttachments[i].colorBlendOp];
		blendAttachmentStates[i].srcAlphaBlendFactor = g_VulkanBlendFactors[this->colorBlendAttachments[i].sourceAlphaBlendFactor];
		blendAttachmentStates[i].dstAlphaBlendFactor = g_VulkanBlendFactors[this->colorBlendAttachments[i].destAlphaBlendFactor];
		blendAttachmentStates[i].alphaBlendOp = g_VulkanBlendOps[this->colorBlendAttachments[i].alphaBlendOp];
		blendAttachmentStates[i].colorWriteMask = static_cast< VkColorComponentFlags >(this->colorBlendAttachments[i].colorWriteMask);
	}

	std::vector< VkPipelineShaderStageCreateInfo > shaderStages(m_shaderModules.size());

	for (size_t i = 0; i < m_shaderModules.size(); i++) {
		shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[i].pNext = nullptr;
		shaderStages[i].flags = 0;
		shaderStages[i].stage = g_VulkanShaderTypes[m_shaderModules[i]->GetType()];
		shaderStages[i].module = m_shaderModules[i]->handle;
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
	inputAssemblyStateCreateInfo.topology = g_VulkanPrimitiveTypes[this->inputAssemblyState.primitiveType];
	inputAssemblyStateCreateInfo.primitiveRestartEnable = this->inputAssemblyState.primitiveRestartEnable ? VK_TRUE : VK_FALSE;

	VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo;
	tessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	tessellationStateCreateInfo.pNext = nullptr;
	tessellationStateCreateInfo.flags = 0;
	tessellationStateCreateInfo.patchControlPoints = this->tessellationState.patchControlPoints;

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
	rasterizationStateCreateInfo.depthClampEnable = this->rasterizationState.depthClampEnable ? VK_TRUE : VK_FALSE;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = this->rasterizationState.rasterizerDiscardEnable ? VK_TRUE : VK_FALSE;
	rasterizationStateCreateInfo.polygonMode = g_VulkanPolygonModes[this->rasterizationState.fillMode];
	rasterizationStateCreateInfo.cullMode = g_VulkanCullModes[this->rasterizationState.cullMode];
	rasterizationStateCreateInfo.frontFace = g_VulkanFrontFaces[this->rasterizationState.frontFace];
	rasterizationStateCreateInfo.depthBiasEnable = this->rasterizationState.depthBiasEnable ? VK_TRUE : VK_FALSE;
	rasterizationStateCreateInfo.depthBiasConstantFactor = this->rasterizationState.depthBiasConstantFactor;
	rasterizationStateCreateInfo.depthBiasClamp = this->rasterizationState.depthBiasClamp;
	rasterizationStateCreateInfo.depthBiasSlopeFactor = this->rasterizationState.depthBiasSlopeFactor;
	rasterizationStateCreateInfo.lineWidth = this->rasterizationState.lineWidth;

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
	depthStencilStateCreateInfo.depthTestEnable = this->depthStencilState.depthTestEnable ? VK_TRUE : VK_FALSE;
	depthStencilStateCreateInfo.depthWriteEnable = this->depthStencilState.depthWriteEnable ? VK_TRUE : VK_FALSE;
	depthStencilStateCreateInfo.depthCompareOp = g_VulkanSamplerCompareOps[this->depthStencilState.compareMode];
	depthStencilStateCreateInfo.depthBoundsTestEnable = this->depthStencilState.depthBoundsTestEnable ? VK_TRUE : VK_FALSE;
	depthStencilStateCreateInfo.stencilTestEnable = this->depthStencilState.stencilTestEnable ? VK_TRUE : VK_FALSE;
	depthStencilStateCreateInfo.front.failOp = g_VulkanStencilOps[this->depthStencilState.front.failOp];
	depthStencilStateCreateInfo.front.passOp = g_VulkanStencilOps[this->depthStencilState.front.passOp];
	depthStencilStateCreateInfo.front.depthFailOp = g_VulkanStencilOps[this->depthStencilState.front.depthFailOp];
	depthStencilStateCreateInfo.front.compareOp = g_VulkanSamplerCompareOps[this->depthStencilState.front.compareMode];
	depthStencilStateCreateInfo.front.compareMask = this->depthStencilState.front.compareMask;
	depthStencilStateCreateInfo.front.writeMask = this->depthStencilState.front.writeMask;
	depthStencilStateCreateInfo.front.reference = this->depthStencilState.front.reference;
	depthStencilStateCreateInfo.back.failOp = g_VulkanStencilOps[this->depthStencilState.back.failOp];
	depthStencilStateCreateInfo.back.passOp = g_VulkanStencilOps[this->depthStencilState.back.passOp];
	depthStencilStateCreateInfo.back.depthFailOp = g_VulkanStencilOps[this->depthStencilState.back.depthFailOp];
	depthStencilStateCreateInfo.back.compareOp = g_VulkanSamplerCompareOps[this->depthStencilState.back.compareMode];
	depthStencilStateCreateInfo.back.compareMask = this->depthStencilState.back.compareMask;
	depthStencilStateCreateInfo.back.writeMask = this->depthStencilState.back.writeMask;
	depthStencilStateCreateInfo.back.reference = this->depthStencilState.back.reference;
	depthStencilStateCreateInfo.minDepthBounds = this->depthStencilState.minDepthBounds;
	depthStencilStateCreateInfo.maxDepthBounds = this->depthStencilState.maxDepthBounds;

	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
	colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStateCreateInfo.pNext = nullptr;
	colorBlendStateCreateInfo.flags = 0;
	colorBlendStateCreateInfo.logicOpEnable = this->colorBlendState.logicOpEnable ? VK_TRUE : VK_FALSE;
	colorBlendStateCreateInfo.logicOp = g_VulkanLogicOps[this->colorBlendState.logicOp];
	colorBlendStateCreateInfo.attachmentCount = static_cast< uint32_t >(blendAttachmentStates.size());
	colorBlendStateCreateInfo.pAttachments = blendAttachmentStates.data();
	std::copy(this->colorBlendState.blendConstants, this->colorBlendState.blendConstants + 4, colorBlendStateCreateInfo.blendConstants);

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
	createInfo.layout = m_pipelineLayout->handle;
	createInfo.renderPass = m_renderPass->handle;
	createInfo.subpass = m_subpass;
	createInfo.basePipelineHandle = m_basePipeline != nullptr ? m_basePipeline->handle : VK_NULL_HANDLE;
	createInfo.basePipelineIndex = m_basePipeline != nullptr ? -1 : 0;

	if (this->device->ftbl.vkCreateGraphicsPipelines(this->device->handle, VK_NULL_HANDLE, 1, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanGraphicsPipeline: Failed to create graphics pipeline!");

	VulkanGraphicsPipelineHash hash;
	std::copy(checksum, checksum + 16, hash.checksum);
	hash.pipeline = this->handle;
	m_hashmap.push_back(std::move(hash));
}
