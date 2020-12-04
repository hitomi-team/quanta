#ifndef LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANRENDERTARGET_H
#define LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANRENDERTARGET_H

#include "vulkanrequired.h"

class CVulkanRenderTarget : public Renderer::RenderTarget {
public:
	inline CVulkanRenderTarget() {}

	bool Setup(unsigned width, unsigned height, Renderer::TextureUsage usage);
	void Release();

};

#endif
