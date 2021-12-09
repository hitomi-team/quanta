#include "level0/pch.h"

#include "api.h"

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice *device, VulkanDescriptorPool *pool, std::shared_ptr< IRenderDescriptorSetLayout > layout)
{
	this->device = device;
	this->pool = pool;

	VkDescriptorSetAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.descriptorPool = this->pool->handle;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &std::dynamic_pointer_cast< VulkanDescriptorSetLayout >(layout)->handle;

	if (this->device->ftbl.vkAllocateDescriptorSets(this->device->handle, &allocInfo, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanDescriptorSet: Failed to allocate descriptor set!");
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkFreeDescriptorSets(this->device->handle, this->pool->handle, 1, &this->handle);
		this->handle = VK_NULL_HANDLE;
	}
}

void VulkanDescriptorSet::Copy(uint64_t sourceBinding, uint64_t sourceArrayElement, std::shared_ptr< IRenderDescriptorSet > destSet, uint64_t destBinding, uint64_t destArrayElement)
{
	VkCopyDescriptorSet copyInfo;
	copyInfo.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
	copyInfo.pNext = nullptr;
	copyInfo.srcSet = this->handle;
	copyInfo.srcBinding = sourceBinding;
	copyInfo.srcArrayElement = sourceArrayElement;
	copyInfo.dstSet = std::dynamic_pointer_cast< VulkanDescriptorSet >(destSet)->handle;
	copyInfo.dstBinding = destBinding;
	copyInfo.dstArrayElement = destArrayElement;
	copyInfo.descriptorCount = 1;

	this->device->ftbl.vkUpdateDescriptorSets(this->device->handle, 0, nullptr, 1, &copyInfo);
}

void VulkanDescriptorSet::Update(uint32_t binding, uint32_t arrayElement, EDescriptorType type, std::shared_ptr< IRenderBuffer > buffer, uint64_t offset, uint64_t range)
{
	VkDescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = std::dynamic_pointer_cast< VulkanBuffer >(buffer)->handle;
	bufferInfo.offset = offset;
	bufferInfo.range = range;

	VkWriteDescriptorSet writeInfo;
	writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeInfo.pNext = nullptr;
	writeInfo.dstSet = this->handle;
	writeInfo.dstBinding = binding;
	writeInfo.dstArrayElement = arrayElement;
	writeInfo.descriptorCount = 1;
	writeInfo.descriptorType = g_VulkanDescriptorTypes[type];
	writeInfo.pImageInfo = nullptr;
	writeInfo.pBufferInfo = &bufferInfo;
	writeInfo.pTexelBufferView = nullptr;

	this->device->ftbl.vkUpdateDescriptorSets(this->device->handle, 1, &writeInfo, 0, nullptr);
}
