#ifndef LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANINDEXBUFFER_H
#define LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANINDEXBUFFER_H

#include "vulkanrequired.h"
#include "vulkanbuffer.h"
#include "layer1/renderer/indexbuffer.h"

class CVulkanIndexBuffer : public Renderer::IndexBuffer {
public:
	inline CVulkanIndexBuffer() {}

	bool SetData(const uint16_t *indices, unsigned count);
	void Release();
};

#endif
