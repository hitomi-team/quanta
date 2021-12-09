#include "level0/pch.h"

#include "api.h"

const std::array< VkSamplerAddressMode, MAX_ADDRESSMODES > g_VulkanSamplerAddressModes {
	VK_SAMPLER_ADDRESS_MODE_REPEAT,
	VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
	VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
	VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
	VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
};

const std::array< VkCompareOp, MAX_TCF > g_VulkanSamplerCompareOps {
	VK_COMPARE_OP_NEVER,
	VK_COMPARE_OP_EQUAL,
	VK_COMPARE_OP_NOT_EQUAL,
	VK_COMPARE_OP_LESS,
	VK_COMPARE_OP_LESS_OR_EQUAL,
	VK_COMPARE_OP_GREATER,
	VK_COMPARE_OP_GREATER_OR_EQUAL,
	VK_COMPARE_OP_ALWAYS
};

const std::array< VkFilter, MAX_FILTERMODES > g_VulkanSamplerFilterModes {
	VK_FILTER_NEAREST,
	VK_FILTER_LINEAR,
	VK_FILTER_LINEAR,
	VK_FILTER_LINEAR,
	VK_FILTER_LINEAR
};

const std::array< VkSamplerMipmapMode, MAX_FILTERMODES > g_VulkanSamplerMipmapModes {
	VK_SAMPLER_MIPMAP_MODE_NEAREST,
	VK_SAMPLER_MIPMAP_MODE_LINEAR,
	VK_SAMPLER_MIPMAP_MODE_LINEAR,
	VK_SAMPLER_MIPMAP_MODE_LINEAR,
	VK_SAMPLER_MIPMAP_MODE_LINEAR
};

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
