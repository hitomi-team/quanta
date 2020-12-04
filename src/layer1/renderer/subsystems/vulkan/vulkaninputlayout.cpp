#include "pch/pch.h"

#include "vulkaninputlayout.h"

bool CVulkanInputLayout::Setup(unsigned char *vs_bytecode, unsigned vs_size)
{
	(void)vs_bytecode;
	(void)vs_size;

	static const VkVertexInputAttributeDescription attrib_desc[] = {
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
		{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3) },
		{ 2, 0, VK_FORMAT_R32G32_SFLOAT, 2*sizeof(glm::vec3) },
	};

	static const VkVertexInputBindingDescription binding_desc = { 0, sizeof(Renderer::Vertex), VK_VERTEX_INPUT_RATE_VERTEX };

	auto createInfo = new VkPipelineVertexInputStateCreateInfo();
	createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	createInfo->vertexBindingDescriptionCount = 1;
	createInfo->pVertexBindingDescriptions = &binding_desc;
	createInfo->vertexAttributeDescriptionCount = 3;
	createInfo->pVertexAttributeDescriptions = attrib_desc;

	this->inputlayout = createInfo;
	return true;
}

void CVulkanInputLayout::Release()
{
	auto createInfo = reinterpret_cast< VkPipelineVertexInputStateCreateInfo * >(this->inputlayout);
	delete createInfo;
	this->inputlayout = nullptr;
}
