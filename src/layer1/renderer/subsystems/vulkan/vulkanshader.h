#ifndef LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANSHADER_H
#define LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANSHADER_H

#include "vulkanrequired.h"

class CVulkanShader : public Renderer::Shader {
public:
	inline CVulkanShader() {}

	bool Build(unsigned char *vs_bytecode, unsigned vs_size, unsigned char *fs_bytecode, unsigned fs_size);
	void Release();

	// get pipeline through this function
	void *GetProgram();

};

#endif
