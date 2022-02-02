#include "level0/pch.h"

#include "api.h"

VulkanComputePipeline::VulkanComputePipeline(VulkanDevice *device, std::shared_ptr< IRenderShaderModule > shaderModule, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderPipelineCache > pipelineCache)
{
	this->device = device;

	VkComputePipelineCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage.pNext = nullptr;
	createInfo.stage.flags = 0;
	createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	createInfo.stage.module = std::dynamic_pointer_cast< VulkanShaderModule >(shaderModule)->handle;
	createInfo.stage.pName = "main";
	createInfo.stage.pSpecializationInfo = nullptr;
	createInfo.layout = std::dynamic_pointer_cast< VulkanPipelineLayout >(pipelineLayout)->handle;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = 0;

	VkPipelineCache cache = pipelineCache != nullptr ? std::dynamic_pointer_cast< VulkanPipelineCache >(pipelineCache)->handle : VK_NULL_HANDLE;
	if (this->device->ftbl.vkCreateComputePipelines(this->device->handle, cache, 1, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanComputePipeline: Unable to create compute pipeline!");
}

VulkanComputePipeline::~VulkanComputePipeline()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyPipeline(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}
