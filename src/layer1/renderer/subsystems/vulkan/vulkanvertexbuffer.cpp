#include "pch/pch.h"

#include "vulkanvertexbuffer.h"

bool CVulkanVertexBuffer::SetData(const Renderer::Vertex *vertices, unsigned count)
{
	this->buffer = new CVulkanBuffer;

	CVulkanBufferInitInfo initInfo = {};
	initInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	initInfo.reqMemFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	initInfo.prefMemFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
	initInfo.vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY;

	CVulkanBuffer *buf = reinterpret_cast< CVulkanBuffer * >(this->buffer);
	buf->Setup(initInfo, sizeof(Renderer::Vertex)*count);
	buf->Upload(vertices, sizeof(Renderer::Vertex)*count, 0);

	this->count = count;

	return true;
}

void CVulkanVertexBuffer::Release()
{
	CVulkanBuffer *buf = reinterpret_cast< CVulkanBuffer * >(this->buffer);
	buf->Release();
	delete buf;
	this->buffer = nullptr;
}
