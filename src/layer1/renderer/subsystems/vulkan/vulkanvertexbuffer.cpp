#include "pch/pch.h"

#include "vulkanvertexbuffer.h"

bool CVulkanVertexBuffer::SetData(const Renderer::Vertex *vertices, unsigned count)
{
	this->buffer = new CVulkanBuffer;

	CVulkanBuffer *buf = reinterpret_cast< CVulkanBuffer * >(this->buffer);
	buf->Setup(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, sizeof(Renderer::Vertex)*count);
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
