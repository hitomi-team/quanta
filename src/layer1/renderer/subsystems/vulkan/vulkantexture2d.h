#ifndef LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANTEXTURE2D
#define LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANTEXTURE2D

#include "vulkanrequired.h"

class CVulkanTexture2D : public Renderer::Texture2D {
public:
	bool SetData(unsigned char *data, unsigned width, unsigned height, Renderer::SamplerStateDesc samplerstatedesc);
	void Release();

};

#endif
