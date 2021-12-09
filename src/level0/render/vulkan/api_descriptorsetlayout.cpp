#include "level0/pch.h"

#include "api.h"

const std::array< VkDescriptorType, MAX_DESCRIPTOR_TYPE_ENUM > g_VulkanDescriptorTypes {
	VK_DESCRIPTOR_TYPE_SAMPLER,
	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
	VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
	VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
	VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
	VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
	VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT,
	VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR
};

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice *device, const std::vector< RenderDescriptorSetLayoutBinding > &_bindings)
{
	this->device = device;

	std::vector< std::vector< VkSampler > > immutableSamplers(_bindings.size());
	std::vector< VkDescriptorSetLayoutBinding > bindings(_bindings.size());

	for (size_t i = 0; i < _bindings.size(); i++) {
		bindings[i].binding = _bindings[i].binding;
		bindings[i].descriptorType = g_VulkanDescriptorTypes[_bindings[i].descriptorType];
		bindings[i].descriptorCount = _bindings[i].descriptorCount;
		bindings[i].stageFlags = static_cast< VkShaderStageFlags >(_bindings[i].stageFlags);
		bindings[i].pImmutableSamplers = [&]() mutable -> VkSampler * {
			std::vector< VkSampler > samplers(_bindings[i].immutableSamplers.size());

			for (size_t j = 0; j < _bindings[i].immutableSamplers.size(); j++)
				samplers[j] = std::dynamic_pointer_cast< VulkanSampler >(_bindings[i].immutableSamplers[j])->handle;

			VkSampler *samplers_ptr = samplers.data();
			immutableSamplers[i] = std::move(samplers);

			return samplers_ptr;
		}();
	}

	VkDescriptorSetLayoutCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.bindingCount = static_cast< uint32_t >(bindings.size());
	createInfo.pBindings = bindings.data();

	if (this->device->ftbl.vkCreateDescriptorSetLayout(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanDescriptorSetLayout: Failed to create descriptor set layout!");
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyDescriptorSetLayout(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}
