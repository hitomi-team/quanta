#ifndef LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANBUFFER_H
#define LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANBUFFER_H

#include "vulkanrequired.h"

struct CVulkanBufferInitInfo {
	VkBufferUsageFlags usage;
	VkMemoryPropertyFlags reqMemFlags;
	VkMemoryPropertyFlags prefMemFlags;
	VmaMemoryUsage vmaUsage;
	VmaAllocationCreateFlags vmaFlags;
};

class CVulkanBuffer {
public:
	inline CVulkanBuffer() { this->ResetCache(); }

	inline void ResetCache()
	{
		this->buf = VK_NULL_HANDLE;
		this->mem_usage = static_cast< VmaMemoryUsage >(-1);
		this->buf_size = 0;
		this->host_mem = nullptr;
	}

	bool Setup(CVulkanBufferInitInfo &initInfo, uint64_t size);
	void Release();

	void Upload(const void *data, uint64_t size, uint64_t offset);
	void *Map();
	void Unmap();

	inline VkBuffer GetBuffer() { return this->buf; }
	inline void *GetMappedMem() { return this->host_mem; }

protected:
	VkBuffer buf;
	VmaMemoryUsage mem_usage;
	VmaAllocation alloc;
	uint64_t buf_size;

	void *host_mem;

	void CopyBuffer(VkBuffer src, VkBuffer dst, uint64_t size, uint64_t offset);
};

#endif
