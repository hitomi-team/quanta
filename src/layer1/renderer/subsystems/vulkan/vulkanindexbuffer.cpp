#include "pch/pch.h"

#include "vulkanindexbuffer.h"

bool CVulkanIndexBuffer::SetData(const uint16_t *indices, unsigned count)
{
	this->buffer = new CVulkanBuffer;

	CVulkanBufferInitInfo initInfo = {};
	initInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	initInfo.reqMemFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	initInfo.prefMemFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
	initInfo.vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY;

	CVulkanBuffer *buf = reinterpret_cast< CVulkanBuffer * >(this->buffer);
	buf->Setup(initInfo, sizeof(uint16_t)*count);
	buf->Upload(indices, sizeof(uint16_t)*count, 0);

	this->count = count;

	return true;
}

void CVulkanIndexBuffer::Release()
{
	CVulkanBuffer *buf = reinterpret_cast< CVulkanBuffer * >(this->buffer);
	buf->Release();
	delete buf;
	this->buffer = nullptr;
}
