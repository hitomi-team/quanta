#include "level0/pch.h"

#include "api.h"

#include "physfs.h"

VulkanComputePipeline::VulkanComputePipeline(VulkanDevice *device, const RenderPipelineShaderInfo &shader, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderComputePipeline > basePipeline)
{
	this->device = device;

	std::unique_ptr< PHYSFS_File, decltype(&PHYSFS_close) > file(PHYSFS_openRead(shader.filePath.data()), PHYSFS_close);
	if (file == nullptr)
		throw std::runtime_error(fmt::format(FMT_COMPILE("VulkanComputePipeline: Cannot open file \"{}\"!"), shader.filePath));

	std::vector< uint8_t > shaderBinary(static_cast< size_t >(PHYSFS_fileLength(file.get())));
	if (PHYSFS_readBytes(file.get(), shaderBinary.data(), shaderBinary.size()) == -1)
		throw std::runtime_error(fmt::format(FMT_COMPILE("VulkanComputePipeline: Cannot read file \"{}\" into memory!"), shader.filePath));

	VkShaderModuleCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.codeSize = static_cast< uint32_t >(shaderBinary.size());
	createInfo.pCode = reinterpret_cast< const uint32_t * >(shaderBinary.data());

	if (this->device->ftbl.vkCreateShaderModule(this->device->handle, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS)
		throw std::runtime_error(fmt::format(FMT_COMPILE("VulkanComputePipeline: Cannot create shader module from file \"{}\"!"), shader.filePath));

	m_pipelineLayout = std::dynamic_pointer_cast< VulkanPipelineLayout >(pipelineLayout);
	m_basePipeline = std::dynamic_pointer_cast< VulkanComputePipeline >(basePipeline);
}

VulkanComputePipeline::~VulkanComputePipeline()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyPipeline(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}

	if (m_shaderModule != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyShaderModule(this->device->handle, m_shaderModule, nullptr);
		m_shaderModule = VK_NULL_HANDLE;
	}
}

void VulkanComputePipeline::Compile()
{
	if (this->handle != VK_NULL_HANDLE)
		return;

	VkComputePipelineCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = m_basePipeline != nullptr ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0;
	createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage.pNext = nullptr;
	createInfo.stage.flags = 0;
	createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	createInfo.stage.module = m_shaderModule;
	createInfo.stage.pName = "main";
	createInfo.stage.pSpecializationInfo = nullptr;
	createInfo.layout = m_pipelineLayout->handle;
	createInfo.basePipelineHandle = m_basePipeline != nullptr ? m_basePipeline->handle : VK_NULL_HANDLE;
	createInfo.basePipelineIndex = m_basePipeline != nullptr ? -1 : 0;

	if (this->device->ftbl.vkCreateComputePipelines(this->device->handle, VK_NULL_HANDLE, 1, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanComputePipeline: Unable to create compute pipeline!");
}
