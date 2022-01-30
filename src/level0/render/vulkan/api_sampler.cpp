#include "level0/pch.h"

#include "api.h"

VulkanSampler::VulkanSampler(VulkanDevice *device, const RenderSamplerStateDescription &state)
{
	this->device = device;

	VkSamplerCustomBorderColorCreateInfoEXT borderColorCreateInfo;
	borderColorCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CUSTOM_BORDER_COLOR_CREATE_INFO_EXT;
	borderColorCreateInfo.pNext = nullptr;
	std::copy(state.borderColor, state.borderColor + 4, borderColorCreateInfo.customBorderColor.float32);
	borderColorCreateInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;

	VkSamplerCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.pNext = &borderColorCreateInfo;
	createInfo.flags = 0;
	createInfo.magFilter = g_VulkanSamplerFilterModes[state.magFilter];
	createInfo.minFilter = g_VulkanSamplerFilterModes[state.minFilter];
	createInfo.mipmapMode = g_VulkanSamplerMipmapModes[state.mipmapMode];
	createInfo.addressModeU = g_VulkanSamplerAddressModes[state.addressModeU];
	createInfo.addressModeV = g_VulkanSamplerAddressModes[state.addressModeV];
	createInfo.addressModeW = g_VulkanSamplerAddressModes[state.addressModeW];
	createInfo.mipLodBias = state.mipLODBias;
	createInfo.anisotropyEnable = state.minFilter == FILTER_ANISOTROPIC || state.magFilter == FILTER_ANISOTROPIC;
	createInfo.maxAnisotropy = state.maxAniso;
	createInfo.compareEnable = VK_TRUE;
	createInfo.compareOp = g_VulkanSamplerCompareOps[state.comparisonFunc];
	createInfo.minLod = state.minLOD;
	createInfo.maxLod = state.maxLOD;
	createInfo.borderColor = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
	createInfo.unnormalizedCoordinates = VK_FALSE;

	if (this->device->ftbl.vkCreateSampler(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanSampler: Cannot create sampler!");
}

VulkanSampler::~VulkanSampler()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroySampler(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}
