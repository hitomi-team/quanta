#ifndef LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANVERTEXBUFFER_H
#define LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANVERTEXBUFFER_H

#include "vulkanrequired.h"
#include "vulkanbuffer.h"
#include "layer1/renderer/vertexbuffer.h"

class CVulkanVertexBuffer : public Renderer::VertexBuffer {
public:
	inline CVulkanVertexBuffer() {}

	bool SetData(const Renderer::Vertex *vertices, unsigned count);
	void Release();
};

#endif
