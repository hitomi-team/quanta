#include "level0/pch.h"

#include "api.h"

#include "level0/crypto/md5.h"
#include "physfs.h"

const std::array< VkBlendFactor, MAX_BLEND_FACTORS > g_VulkanBlendFactors {
	VK_BLEND_FACTOR_ZERO,
	VK_BLEND_FACTOR_ONE,
	VK_BLEND_FACTOR_SRC_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
	VK_BLEND_FACTOR_DST_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
	VK_BLEND_FACTOR_SRC_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	VK_BLEND_FACTOR_DST_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	VK_BLEND_FACTOR_CONSTANT_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
	VK_BLEND_FACTOR_CONSTANT_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
	VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
	VK_BLEND_FACTOR_SRC1_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
	VK_BLEND_FACTOR_SRC1_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA
};

const std::array< VkBlendOp, MAX_BLEND_OPS > g_VulkanBlendOps {
	VK_BLEND_OP_ADD,
	VK_BLEND_OP_SUBTRACT,
	VK_BLEND_OP_REVERSE_SUBTRACT,
	VK_BLEND_OP_MIN,
	VK_BLEND_OP_MAX,
	VK_BLEND_OP_ZERO_EXT,
	VK_BLEND_OP_SRC_EXT,
	VK_BLEND_OP_DST_EXT,
	VK_BLEND_OP_SRC_OVER_EXT,
	VK_BLEND_OP_DST_OVER_EXT,
	VK_BLEND_OP_SRC_IN_EXT,
	VK_BLEND_OP_DST_IN_EXT,
	VK_BLEND_OP_SRC_OUT_EXT,
	VK_BLEND_OP_DST_OUT_EXT,
	VK_BLEND_OP_SRC_ATOP_EXT,
	VK_BLEND_OP_DST_ATOP_EXT,
	VK_BLEND_OP_XOR_EXT,
	VK_BLEND_OP_MULTIPLY_EXT,
	VK_BLEND_OP_SCREEN_EXT,
	VK_BLEND_OP_OVERLAY_EXT,
	VK_BLEND_OP_DARKEN_EXT,
	VK_BLEND_OP_LIGHTEN_EXT,
	VK_BLEND_OP_COLORDODGE_EXT,
	VK_BLEND_OP_COLORBURN_EXT,
	VK_BLEND_OP_HARDLIGHT_EXT,
	VK_BLEND_OP_SOFTLIGHT_EXT,
	VK_BLEND_OP_DIFFERENCE_EXT,
	VK_BLEND_OP_EXCLUSION_EXT,
	VK_BLEND_OP_INVERT_EXT,
	VK_BLEND_OP_INVERT_RGB_EXT,
	VK_BLEND_OP_LINEARDODGE_EXT,
	VK_BLEND_OP_LINEARBURN_EXT,
	VK_BLEND_OP_VIVIDLIGHT_EXT,
	VK_BLEND_OP_LINEARLIGHT_EXT,
	VK_BLEND_OP_PINLIGHT_EXT,
	VK_BLEND_OP_HARDMIX_EXT,
	VK_BLEND_OP_HSL_HUE_EXT,
	VK_BLEND_OP_HSL_SATURATION_EXT,
	VK_BLEND_OP_HSL_COLOR_EXT,
	VK_BLEND_OP_HSL_LUMINOSITY_EXT,
	VK_BLEND_OP_PLUS_EXT,
	VK_BLEND_OP_PLUS_CLAMPED_EXT,
	VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT,
	VK_BLEND_OP_PLUS_DARKER_EXT,
	VK_BLEND_OP_MINUS_EXT,
	VK_BLEND_OP_MINUS_CLAMPED_EXT,
	VK_BLEND_OP_CONTRAST_EXT,
	VK_BLEND_OP_INVERT_OVG_EXT,
	VK_BLEND_OP_RED_EXT,
	VK_BLEND_OP_GREEN_EXT,
	VK_BLEND_OP_BLUE_EXT
};

const std::array< VkCullModeFlags, MAX_CULLMODES > g_VulkanCullModes {
	VK_CULL_MODE_NONE,
	VK_CULL_MODE_FRONT_BIT,
	VK_CULL_MODE_BACK_BIT,
	VK_CULL_MODE_FRONT_AND_BACK
};

const std::array< VkFrontFace, MAX_FRONT_FACE_ENUM > g_VulkanFrontFaces {
	VK_FRONT_FACE_COUNTER_CLOCKWISE,
	VK_FRONT_FACE_CLOCKWISE
};

const std::array< VkLogicOp, MAX_LOGIC_OP_ENUM > g_VulkanLogicOps {
	VK_LOGIC_OP_CLEAR,
	VK_LOGIC_OP_AND,
	VK_LOGIC_OP_AND_REVERSE,
	VK_LOGIC_OP_COPY,
	VK_LOGIC_OP_AND_INVERTED,
	VK_LOGIC_OP_NO_OP,
	VK_LOGIC_OP_XOR,
	VK_LOGIC_OP_OR,
	VK_LOGIC_OP_NOR,
	VK_LOGIC_OP_EQUIVALENT,
	VK_LOGIC_OP_INVERT,
	VK_LOGIC_OP_OR_REVERSE,
	VK_LOGIC_OP_COPY_INVERTED,
	VK_LOGIC_OP_OR_INVERTED,
	VK_LOGIC_OP_NAND,
	VK_LOGIC_OP_SET
};

const std::array< VkPolygonMode, MAX_FILL_MODE_ENUM > g_VulkanPolygonModes {
	VK_POLYGON_MODE_FILL,
	VK_POLYGON_MODE_LINE,
	VK_POLYGON_MODE_POINT
};

const std::array< VkPrimitiveTopology, MAX_PRIMITIVE_TYPE_ENUM > g_VulkanPrimitiveTypes {
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
	VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
	VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN
};

const std::array< VkShaderStageFlagBits, MAX_SHADER_TYPES > g_VulkanShaderTypes {
	VK_SHADER_STAGE_VERTEX_BIT,
	VK_SHADER_STAGE_FRAGMENT_BIT,
	VK_SHADER_STAGE_GEOMETRY_BIT,
	VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
	VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
};

const std::array< VkStencilOp, MAX_STENCIL_OP_ENUM > g_VulkanStencilOps {
	VK_STENCIL_OP_KEEP,
	VK_STENCIL_OP_ZERO,
	VK_STENCIL_OP_REPLACE,
	VK_STENCIL_OP_INCREMENT_AND_CLAMP,
	VK_STENCIL_OP_DECREMENT_AND_CLAMP,
	VK_STENCIL_OP_INVERT,
	VK_STENCIL_OP_INCREMENT_AND_WRAP,
	VK_STENCIL_OP_DECREMENT_AND_WRAP
};

const std::array< VkVertexInputRate, MAX_VERTEX_INPUT_RATE_ENUM > g_VulkanVertexInputRates {
	VK_VERTEX_INPUT_RATE_VERTEX,
	VK_VERTEX_INPUT_RATE_INSTANCE
};

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice *device, const std::vector< RenderPipelineShaderInfo > &shaders, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderGraphicsPipeline > basePipeline, std::shared_ptr< IRenderPass > renderPass, uint32_t subpass)
{
	this->device = device;

	m_shaderModules.resize(shaders.size());

	for (size_t i = 0; i < shaders.size(); i++) {
		std::unique_ptr< PHYSFS_File, decltype(&PHYSFS_close) > file(PHYSFS_openRead(shaders[i].filePath.data()), PHYSFS_close);
		if (file == nullptr)
			throw std::runtime_error(fmt::format(FMT_COMPILE("VulkanGraphicsPipeline: Cannot open file \"{}\"!"), shaders[i].filePath));

		std::vector< uint8_t > shaderBinary(static_cast< size_t >(PHYSFS_fileLength(file.get())));
		if (PHYSFS_readBytes(file.get(), shaderBinary.data(), shaderBinary.size()) == -1)
			throw std::runtime_error(fmt::format(FMT_COMPILE("VulkanGraphicsPipeline: Cannot read file \"{}\" into memory!"), shaders[i].filePath));

		VkShaderModuleCreateInfo createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.codeSize = static_cast< uint32_t >(shaderBinary.size());
		createInfo.pCode = reinterpret_cast< const uint32_t * >(shaderBinary.data());

		VkPipelineShaderStageCreateInfo shaderStageCreateInfo;
		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfo.pNext = nullptr;
		shaderStageCreateInfo.flags = 0;
		shaderStageCreateInfo.stage = g_VulkanShaderTypes[shaders[i].type];
		shaderStageCreateInfo.pName = "main";
		shaderStageCreateInfo.pSpecializationInfo = nullptr;

		if (this->device->ftbl.vkCreateShaderModule(this->device->handle, &createInfo, nullptr, &shaderStageCreateInfo.module) != VK_SUCCESS)
			throw std::runtime_error(fmt::format(FMT_COMPILE("VulkanGraphicsPipeline: Cannot create shader module from file \"{}\"!"), shaders[i].filePath));

		m_shaderModules[i] = shaderStageCreateInfo;
	}

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

	for (auto &shaderModule : m_shaderModules) {
		if (shaderModule.module != VK_NULL_HANDLE) {
			this->device->ftbl.vkDestroyShaderModule(this->device->handle, shaderModule.module, nullptr);
			shaderModule.module = VK_NULL_HANDLE;
		}
	}
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

	for (size_t i = 0; i < this->viewportState.scissors.size(); i++) {
		blendAttachmentStates[i].blendEnable = this->colorBlendAttachments[i].enable ? VK_TRUE : VK_FALSE;
		blendAttachmentStates[i].srcColorBlendFactor = g_VulkanBlendFactors[this->colorBlendAttachments[i].sourceColorBlendFactor];
		blendAttachmentStates[i].dstColorBlendFactor = g_VulkanBlendFactors[this->colorBlendAttachments[i].destColorBlendFactor];
		blendAttachmentStates[i].colorBlendOp = g_VulkanBlendOps[this->colorBlendAttachments[i].colorBlendOp];
		blendAttachmentStates[i].srcAlphaBlendFactor = g_VulkanBlendFactors[this->colorBlendAttachments[i].sourceAlphaBlendFactor];
		blendAttachmentStates[i].dstAlphaBlendFactor = g_VulkanBlendFactors[this->colorBlendAttachments[i].destAlphaBlendFactor];
		blendAttachmentStates[i].alphaBlendOp = g_VulkanBlendOps[this->colorBlendAttachments[i].alphaBlendOp];
		blendAttachmentStates[i].colorWriteMask = static_cast< VkColorComponentFlags >(this->colorBlendAttachments[i].colorWriteMask);
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
	createInfo.stageCount = static_cast< uint32_t >(m_shaderModules.size());
	createInfo.pStages = m_shaderModules.data();
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
