#include "pch/pch.h"

#include "vulkanparameterbuffer.h"
#include "vulkanrenderer.h"
#include "vulkanimage.h"

bool CVulkanParameterBuffer::Setup(std::vector< Renderer::ShaderParameterElement > velements)
{
	this->elements = velements;
	this->mem_size = 0;

	for (auto &i : this->elements)
		this->mem_size += PCH_ALIGN(i.dataSize, 16);

	this->buf.Setup(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, this->mem_size);

	VkDescriptorSetAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.descriptorPool = g_vulkanCtx->desc_pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &g_vulkanCtx->desc_layout;

	VK_ASSERT(vkAllocateDescriptorSets(g_vulkanCtx->device, &allocInfo, &this->desc_set), "Failed to allocate descriptor set");

	this->Map();
	this->Flush();
	this->Unmap();

	return true;
}

void CVulkanParameterBuffer::Release()
{
	this->Unmap();
	this->buf.Release();
	for (auto &i : this->elements)
		delete[] i.data;
}

void CVulkanParameterBuffer::Apply()
{
	vkCmdBindDescriptorSets(g_vulkanCtx->swapchain_command_bufs[g_vulkanCtx->current_image], VK_PIPELINE_BIND_POINT_GRAPHICS, g_vulkanCtx->pipeline_layout, 0, 1, &this->desc_set, 0, nullptr);
}

void CVulkanParameterBuffer::Map()
{
	this->map_mem = reinterpret_cast< char * >(this->buf.Map());
}

void CVulkanParameterBuffer::Flush()
{
	size_t stride = 0;

	for (auto &i : this->elements) {
		std::memcpy(this->map_mem + stride, i.data, i.dataSize);
		stride += i.dataSize;
	}
}

void CVulkanParameterBuffer::Unmap()
{
	if (this->map_mem != nullptr) {
		this->buf.Unmap();
		this->map_mem = nullptr;
	}
}

void CVulkanParameterBuffer::SetShaderParameter(Renderer::ShaderParameterUsage param, float value)
{
	size_t stride = 0;
	for (auto &i : this->elements) {
		if (i.usage == param) {
			*(float *)i.data = value;
			std::memcpy(this->map_mem + stride, &value, i.dataSize);
		}
		stride += i.dataSize;
	}
}

void CVulkanParameterBuffer::SetShaderParameter(Renderer::ShaderParameterUsage param, const glm::mat4 &mat)
{
	size_t stride = 0;
	for (auto &i : this->elements) {
		if (i.usage == param) {
			*(glm::mat4 *)i.data = mat;
			std::memcpy(this->map_mem + stride, &mat[0], i.dataSize);
		}
		stride += i.dataSize;
	}
}

void CVulkanParameterBuffer::Update()
{
	Renderer::Texture2D *texture2d = Renderer::g_vulkanRenderer->GetTextureView(0);

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = this->buf.GetBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(float);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	if (texture2d != nullptr) {
		CVulkanImage *image = reinterpret_cast< CVulkanImage * >(texture2d->GetView());
		VkSampler *sampler = reinterpret_cast< VkSampler * >(texture2d->GetSampler());
		imageInfo.imageView = image->GetView();
		imageInfo.sampler = sampler[0];
	} else {
		imageInfo.imageView = VK_NULL_HANDLE;
		imageInfo.sampler = VK_NULL_HANDLE;
	}

	VkWriteDescriptorSet descriptorWrites[2] = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = this->desc_set;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = this->desc_set;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(g_vulkanCtx->device, 2, descriptorWrites, 0, nullptr);
}
