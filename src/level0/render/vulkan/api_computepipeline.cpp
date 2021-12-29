#include "level0/pch.h"

#include "api.h"

VulkanComputePipeline::VulkanComputePipeline(VulkanDevice *device, std::shared_ptr< IRenderShaderModule > _shaderModule, std::shared_ptr< IRenderPipelineLayout > _pipelineLayout, std::shared_ptr< IRenderComputePipeline > _basePipeline)
{
	this->device = device;

	auto shaderModule = std::dynamic_pointer_cast< VulkanShaderModule >(_shaderModule);
	auto pipelineLayout = std::dynamic_pointer_cast< VulkanPipelineLayout >(_pipelineLayout);
	auto basePipeline = std::dynamic_pointer_cast< VulkanComputePipeline >(_basePipeline);

	VkComputePipelineCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = basePipeline != nullptr ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0;
	createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage.pNext = nullptr;
	createInfo.stage.flags = 0;
	createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	createInfo.stage.module = shaderModule->handle;
	createInfo.stage.pName = "main";
	createInfo.stage.pSpecializationInfo = nullptr;
	createInfo.layout = pipelineLayout->handle;
	createInfo.basePipelineHandle = basePipeline != nullptr ? basePipeline->handle : VK_NULL_HANDLE;
	createInfo.basePipelineIndex = basePipeline != nullptr ? -1 : 0;

	if (this->device->ftbl.vkCreateComputePipelines(this->device->handle, VK_NULL_HANDLE, 1, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanComputePipeline: Unable to create compute pipeline!");
}

VulkanComputePipeline::~VulkanComputePipeline()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyPipeline(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}
