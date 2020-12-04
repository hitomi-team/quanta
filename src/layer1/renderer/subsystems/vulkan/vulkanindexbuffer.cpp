#include "pch/pch.h"

#include "vulkanindexbuffer.h"

bool CVulkanIndexBuffer::SetData(const uint16_t *indices, unsigned count)
{
	this->buffer = new CVulkanBuffer;

	CVulkanBuffer *buf = reinterpret_cast< CVulkanBuffer * >(this->buffer);
	buf->Setup(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, sizeof(uint16_t)*count);
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
