#ifndef LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANINPUTLAYOUT_H
#define LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANINPUTLAYOUT_H

#include "vulkanrequired.h"

class CVulkanInputLayout : public Renderer::InputLayout {
public:
	bool Setup(unsigned char *vs_bytecode, unsigned vs_size);
	void Release();

};

#endif
