#include "level0/pch.h"

#include "api.h"

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice *device, uint32_t maxSets, const std::vector< RenderDescriptorPoolSize > &_poolSizes)
{
	this->device = device;

	std::vector< VkDescriptorPoolSize > poolSizes(_poolSizes.size());

	for (size_t i = 0; i < _poolSizes.size(); i++) {
		poolSizes[i].type = g_VulkanDescriptorTypes[_poolSizes[i].type];
		poolSizes[i].descriptorCount = _poolSizes[i].count;
	}

	VkDescriptorPoolCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	createInfo.maxSets = maxSets;
	createInfo.poolSizeCount = static_cast< uint32_t >(poolSizes.size());
	createInfo.pPoolSizes = poolSizes.data();

	if (this->device->ftbl.vkCreateDescriptorPool(this->device->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanDescriptorPool: Failed to create descriptor pool!");
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyDescriptorPool(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}

std::shared_ptr< IRenderDescriptorSet > VulkanDescriptorPool::AllocateSingle(std::shared_ptr< IRenderDescriptorSetLayout > layout)
{
	return std::dynamic_pointer_cast< IRenderDescriptorSet >(std::make_shared< VulkanDescriptorSet >(this->device, this, layout));
}

void VulkanDescriptorPool::AllocateBulk(std::vector< std::shared_ptr< IRenderDescriptorSet > > &descriptorSets, const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &layouts)
{
	for (size_t i = 0; i < descriptorSets.size(); i++)
		descriptorSets[i] = std::dynamic_pointer_cast< IRenderDescriptorSet >(std::make_shared< VulkanDescriptorSet >(this->device, this, layouts[i]));
}
