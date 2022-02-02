#include "level0/pch.h"

#include "api.h"

VulkanPipelineCache::VulkanPipelineCache(VulkanDevice *device, const void *blob, size_t blobSize)
{
	this->device = device;

	VkPipelineCacheCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.initialDataSize = blobSize;
	createInfo.pInitialData = blob;

	VK_CHECK(this->device->ftbl.vkCreatePipelineCache(this->device->handle, &createInfo, nullptr, &this->handle),
		"VulkanPipelineCache: Failed to create pipeline cache object!");
}

VulkanPipelineCache::~VulkanPipelineCache()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkDestroyPipelineCache(this->device->handle, this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}

bool VulkanPipelineCache::RetrieveData(void **blob, size_t *blobSize)
{
	return this->device->ftbl.vkGetPipelineCacheData(this->device->handle, this->handle, blobSize, blob) == VK_SUCCESS;
}
