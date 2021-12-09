#include "level0/pch.h"

#include "api.h"

VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice *device, const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &_layouts)
{
	this->device = device;

	std::vector< VkDescriptorSetLayout > layouts(_layouts.size());
	for (size_t i = 0; i < _layouts.size(); i++)
		layouts[i] = std::dynamic_pointer_cast< VulkanDescriptorSetLayout >(_layouts[i])->handle;

	VkPipelineLayoutCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.setLayoutCount = static_cast< uint32_t >(layouts.size());
	createInfo.pSetLayouts = layouts.data();
	createInfo.pushConstantRangeCount = 0;
	createInfo.pPushConstantRanges = nullptr;

	if (this->device->ftbl.vkCreatePipelineLayout(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanPipelineLayout: Failed to create pipeline layout!");
}

VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice *device, const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &_layouts, const std::vector< RenderPushConstantRange > &_ranges)
{
	this->device = device;

	std::vector< VkDescriptorSetLayout > layouts(_layouts.size());
	for (size_t i = 0; i < _layouts.size(); i++)
		layouts[i] = std::dynamic_pointer_cast< VulkanDescriptorSetLayout >(_layouts[i])->handle;

	std::vector< VkPushConstantRange > ranges(_ranges.size());
	for (size_t i = 0; i < _ranges.size(); i++) {
		ranges[i].stageFlags = static_cast< VkShaderStageFlags >(_ranges[i].stageFlags);
		ranges[i].offset = _ranges[i].offset;
		ranges[i].size = _ranges[i].size;
	}

	VkPipelineLayoutCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.setLayoutCount = static_cast< uint32_t >(layouts.size());
	createInfo.pSetLayouts = layouts.data();
	createInfo.pushConstantRangeCount = static_cast< uint32_t >(ranges.size());
	createInfo.pPushConstantRanges = ranges.data();

	if (this->device->ftbl.vkCreatePipelineLayout(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanPipelineLayout: Failed to create pipeline layout!");
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyPipelineLayout(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}
