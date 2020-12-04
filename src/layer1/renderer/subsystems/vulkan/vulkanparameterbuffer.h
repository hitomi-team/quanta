#ifndef LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANPARAMETERBUFFER_H
#define LAYER1_RENDERER_SUBSYSTEM_VULKAN_VULKANPARAMETERBUFFER_H

#include "vulkanbuffer.h"

class CVulkanParameterBuffer : public Renderer::ShaderParameterBuffer {
public:
	inline CVulkanParameterBuffer()
	{
		this->desc_set = VK_NULL_HANDLE;
		this->map_mem = nullptr;
	}

	bool Setup(std::vector< Renderer::ShaderParameterElement > elements);
	void Release();

	void Apply();
	void Map();
	void Flush();
	void Unmap();

	void SetShaderParameter(Renderer::ShaderParameterUsage param, float value);
	void SetShaderParameter(Renderer::ShaderParameterUsage param, const glm::mat4 &mat);

	// used by VulkanRenderer 
	void Update();

protected:
	CVulkanBuffer buf;
	VkDescriptorSet desc_set;

	char *map_mem;
	size_t mem_size;
};

#endif
