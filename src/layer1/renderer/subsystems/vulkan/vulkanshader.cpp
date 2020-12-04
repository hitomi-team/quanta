#include "pch/pch.h"

#include "vulkanshader.h"
#include "vulkaninputlayout.h"
#include "vulkanctx.h"

static VkShaderModule VK_CreateShaderModule(unsigned char *bytecode, unsigned bc_size)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = bc_size;
	createInfo.pCode = reinterpret_cast< const uint32_t * >(bytecode);

	VkShaderModule shader_module;
	VK_ASSERT(vkCreateShaderModule(g_vulkanCtx->device, &createInfo, nullptr, &shader_module), "Failed to create Vulkan shader module");

	return shader_module;
}

bool CVulkanShader::Build(unsigned char *vs_bytecode, unsigned vs_size, unsigned char *fs_bytecode, unsigned fs_size)
{
	static const VkDynamicState dynamic_states[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	static const VkViewport blank_viewport = {};
	static const VkRect2D blank_scissor = {};

	this->inputlayout = new CVulkanInputLayout;
	this->inputlayout->Setup(vs_bytecode, vs_size);

	VkShaderModule vert_shader = VK_CreateShaderModule(vs_bytecode, vs_size);
	VkShaderModule frag_shader = VK_CreateShaderModule(fs_bytecode, fs_size);

	VkPipelineShaderStageCreateInfo stageInfos[] = {
		{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_VERTEX_BIT, vert_shader, "main", nullptr },
		{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_FRAGMENT_BIT, frag_shader, "main", nullptr }
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &blank_viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &blank_scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisample = {};
	multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.sampleShadingEnable = VK_FALSE;
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlend = {};
	colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlend.logicOpEnable = VK_FALSE;
	colorBlend.logicOp = VK_LOGIC_OP_COPY;
	colorBlend.attachmentCount = 1;
	colorBlend.pAttachments = &colorBlendAttachment;
	colorBlend.blendConstants[0] = 0.0f;
	colorBlend.blendConstants[1] = 0.0f;
	colorBlend.blendConstants[2] = 0.0f;
	colorBlend.blendConstants[3] = 0.0f;

	VkPipelineDynamicStateCreateInfo dynamicCreateInfo;
	dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicCreateInfo.pNext = nullptr;
	dynamicCreateInfo.flags = 0;
	dynamicCreateInfo.dynamicStateCount = 2;
	dynamicCreateInfo.pDynamicStates = dynamic_states;

	VkGraphicsPipelineCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = stageInfos;
	createInfo.pVertexInputState = reinterpret_cast< VkPipelineVertexInputStateCreateInfo * >(this->inputlayout->GetInputLayout());
	createInfo.pInputAssemblyState = &inputAssembly;
	createInfo.pViewportState = &viewportState;
	createInfo.pRasterizationState = &rasterizer;
	createInfo.pMultisampleState = &multisample;
	createInfo.pDepthStencilState = &depthStencil;
	createInfo.pColorBlendState = &colorBlend;
	createInfo.pDynamicState = &dynamicCreateInfo;
	createInfo.layout = g_vulkanCtx->pipeline_layout;
	createInfo.renderPass = g_vulkanCtx->swapchain_renderpass;
	createInfo.subpass = 0;

	this->program = new VkPipeline;
	VK_ASSERT(vkCreateGraphicsPipelines(g_vulkanCtx->device, VK_NULL_HANDLE, 1, &createInfo, nullptr, reinterpret_cast< VkPipeline * >(this->program)), "Failed to create graphics VkPipeline");

	vkDestroyShaderModule(g_vulkanCtx->device, vert_shader, nullptr);
	vkDestroyShaderModule(g_vulkanCtx->device, frag_shader, nullptr);

	return true;
}

void CVulkanShader::Release()
{
	if (this->program == nullptr)
		return;

	vkDestroyPipeline(g_vulkanCtx->device, reinterpret_cast< VkPipeline * >(this->program)[0], nullptr);
	delete reinterpret_cast< VkPipeline * >(this->program);
	this->program = nullptr;
}

void *CVulkanShader::GetProgram()
{
	return this->program;
}
