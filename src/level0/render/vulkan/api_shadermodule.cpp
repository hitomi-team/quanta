#include "level0/pch.h"

#include "api.h"

VulkanShaderModule::VulkanShaderModule(VulkanDevice *device, EShaderType type, const void *blob, size_t blobSize)
{
	this->device = device;
	m_type = type;

	VkShaderModuleCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.codeSize = static_cast< uint32_t >(blobSize);
	createInfo.pCode = reinterpret_cast< const uint32_t * >(blob);

	if (this->device->ftbl.vkCreateShaderModule(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanShaderModule: Cannot create shader module!");
}

VulkanShaderModule::~VulkanShaderModule()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyShaderModule(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}

EShaderType VulkanShaderModule::GetType()
{
	return m_type;
}
