#include "level0/pch.h"

#include "api.h"

const std::array< VkCommandBufferLevel, MAX_COMMAND_BUFFER_LEVEL_ENUM > g_VulkanCommandBufferLevels {
	VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	VK_COMMAND_BUFFER_LEVEL_SECONDARY
};

const std::array< VkSubpassContents, MAX_SUBPASS_CONTENTS_ENUM > g_VulkanSubpassContents {
	VK_SUBPASS_CONTENTS_INLINE,
	VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
};

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice *device, VulkanCommandPool *pool, ECommandBufferLevel level)
{
	this->device = device;
	this->pool = pool;

	VkCommandBufferAllocateInfo allocateInfo;
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.commandPool = this->pool->handle;
	allocateInfo.level = g_VulkanCommandBufferLevels[level];
	allocateInfo.commandBufferCount = 1;

	if (this->device->ftbl.vkAllocateCommandBuffers(this->device->handle, &allocateInfo, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanCommandBuffer: Failed to allocate command buffer!");
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
	if (this->handle != VK_NULL_HANDLE) {
		this->device->ftbl.vkFreeCommandBuffers(this->device->handle, this->pool->handle, 1, &this->handle);
		this->handle = VK_NULL_HANDLE;
	}
}

void VulkanCommandBuffer::Begin(ECommandBufferUsage usage)
{
	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.flags = static_cast< VkCommandBufferUsageFlags >(usage);
	beginInfo.pInheritanceInfo = nullptr;

	if (this->device->ftbl.vkBeginCommandBuffer(this->handle, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("VulkanCommandBuffer: Failed to begin command buffer!");
}

void VulkanCommandBuffer::End()
{
	this->device->ftbl.vkEndCommandBuffer(this->handle);
}

void VulkanCommandBuffer::BindGraphicsPipeline(std::shared_ptr< IRenderGraphicsPipeline > pipeline)
{
	this->device->ftbl.vkCmdBindPipeline(this->handle, VK_PIPELINE_BIND_POINT_GRAPHICS, std::dynamic_pointer_cast< VulkanGraphicsPipeline >(pipeline)->handle);
}

void VulkanCommandBuffer::BindGraphicsDescriptorSet(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, std::shared_ptr< IRenderDescriptorSet > descriptorSet, uint32_t dynamicOffset)
{
	this->device->ftbl.vkCmdBindDescriptorSets(this->handle, VK_PIPELINE_BIND_POINT_GRAPHICS, std::dynamic_pointer_cast< VulkanPipelineLayout >(layout)->handle, firstSet, 1, &std::dynamic_pointer_cast< VulkanDescriptorSet >(descriptorSet)->handle, 1, &dynamicOffset);
}

void VulkanCommandBuffer::BindGraphicsDescriptorSets(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, const std::vector< std::shared_ptr< IRenderDescriptorSet > > &_descriptorSets, const std::vector< uint32_t > &dynamicOffsets)
{
	std::vector< VkDescriptorSet > descriptorSets(_descriptorSets.size());
	for (size_t i = 0; i < descriptorSets.size(); i++)
		descriptorSets[i] = std::dynamic_pointer_cast< VulkanDescriptorSet >(_descriptorSets[i])->handle;

	this->device->ftbl.vkCmdBindDescriptorSets(this->handle, VK_PIPELINE_BIND_POINT_GRAPHICS, std::dynamic_pointer_cast< VulkanPipelineLayout >(layout)->handle, firstSet, static_cast< uint32_t >(descriptorSets.size()), descriptorSets.data(), static_cast< uint32_t >(dynamicOffsets.size()), dynamicOffsets.data());
}

void VulkanCommandBuffer::BindVertexBuffer(uint32_t firstBinding, std::shared_ptr< IRenderBuffer > buffer, uint64_t offset)
{
	this->device->ftbl.vkCmdBindVertexBuffers(this->handle, firstBinding, 1, &std::dynamic_pointer_cast< VulkanBuffer >(buffer)->handle, &offset);
}

void VulkanCommandBuffer::BindVertexBuffers(uint32_t firstBinding, const std::vector< std::shared_ptr< IRenderBuffer > > &buffers, const std::vector< uint64_t > &offsets)
{
	std::vector< VkBuffer > _buffers(buffers.size());
	for (size_t i = 0; i < buffers.size(); i++)
		_buffers[i] = std::dynamic_pointer_cast< VulkanBuffer >(buffers[i])->handle;

	this->device->ftbl.vkCmdBindVertexBuffers(this->handle, firstBinding, static_cast< uint32_t >(_buffers.size()), _buffers.data(), offsets.data());
}

void VulkanCommandBuffer::BindIndexBufferU16(std::shared_ptr< IRenderBuffer > buffer, uint64_t offset)
{
	this->device->ftbl.vkCmdBindIndexBuffer(this->handle, std::dynamic_pointer_cast< VulkanBuffer >(buffer)->handle, offset, VK_INDEX_TYPE_UINT16);
}

void VulkanCommandBuffer::BeginRenderPass(std::shared_ptr< IRenderPass > renderPass, std::shared_ptr< IRenderFramebuffer > framebuffer, RenderRectangle renderArea, const std::vector< RenderClearValue > &_clearValues, ESubpassContents subpassContents)
{
	std::vector< VkClearValue > clearValues(_clearValues.size());
	for (size_t i = 0; i < _clearValues.size(); i++)
		std::copy(_clearValues[i].color.u32, _clearValues[i].color.u32 + 4, clearValues[i].color.uint32);

	VkRenderPassBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.renderPass = std::dynamic_pointer_cast< VulkanRenderPass >(renderPass)->handle;
	beginInfo.framebuffer = std::dynamic_pointer_cast< VulkanFramebuffer >(framebuffer)->handle;
	beginInfo.renderArea.offset = { renderArea.offset.x, renderArea.offset.y };
	beginInfo.renderArea.extent = { renderArea.extent.width, renderArea.extent.height };
	beginInfo.clearValueCount = static_cast< uint32_t >(clearValues.size());
	beginInfo.pClearValues = clearValues.data();

	this->device->ftbl.vkCmdBeginRenderPass(this->handle, &beginInfo, g_VulkanSubpassContents[subpassContents]);
}

void VulkanCommandBuffer::ClearAttachment(const RenderClearAttachment &attachment, const RenderClearRectangle &rectangle)
{
	VkClearAttachment clearAttachment;
	VkClearRect clearRect;

	clearAttachment.aspectMask = g_VulkanImageAspectFlags[attachment.aspect];
	clearAttachment.colorAttachment = attachment.colorAttachment;
	std::copy(attachment.clearValue.color.u32, attachment.clearValue.color.u32 + 4, clearAttachment.clearValue.color.uint32);

	clearRect.rect.offset = { rectangle.rectangle.offset.x, rectangle.rectangle.offset.y };
	clearRect.rect.extent = { rectangle.rectangle.extent.width, rectangle.rectangle.extent.height };
	clearRect.baseArrayLayer = rectangle.baseArrayLayer;
	clearRect.layerCount = rectangle.layerCount;

	this->device->ftbl.vkCmdClearAttachments(this->handle, 1, &clearAttachment, 1, &clearRect);
}

void VulkanCommandBuffer::ClearAttachments(const std::vector< RenderClearAttachment > &attachments, const std::vector< RenderClearRectangle > &rectangles)
{
	std::vector< VkClearAttachment > clearAttachments(attachments.size());
	std::vector< VkClearRect > clearRects(rectangles.size());

	for (size_t i = 0; i < attachments.size(); i++) {
		clearAttachments[i].aspectMask = g_VulkanImageAspectFlags[attachments[i].aspect];
		clearAttachments[i].colorAttachment = attachments[i].colorAttachment;
		std::copy(attachments[i].clearValue.color.u32, attachments[i].clearValue.color.u32 + 4, clearAttachments[i].clearValue.color.uint32);
	}

	for (size_t i = 0; i < rectangles.size(); i++) {
		clearRects[i].rect.offset = { rectangles[i].rectangle.offset.x, rectangles[i].rectangle.offset.y };
		clearRects[i].rect.extent = { rectangles[i].rectangle.extent.width, rectangles[i].rectangle.extent.height };
		clearRects[i].baseArrayLayer = rectangles[i].baseArrayLayer;
		clearRects[i].layerCount = rectangles[i].layerCount;
	}

	this->device->ftbl.vkCmdClearAttachments(this->handle, static_cast< uint32_t >(clearAttachments.size()), clearAttachments.data(), static_cast< uint32_t >(clearRects.size()), clearRects.data());
}

void VulkanCommandBuffer::ClearColorImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderColorClearValue &clearValue, const std::vector< RenderImageSubresourceRange > &subresourceRanges)
{
	std::vector< VkImageSubresourceRange > ranges(subresourceRanges.size());

	for (size_t i = 0; i < subresourceRanges.size(); i++) {
		ranges[i].aspectMask = g_VulkanImageAspectFlags[subresourceRanges[i].aspect];
		ranges[i].baseMipLevel = subresourceRanges[i].baseMipLevel;
		ranges[i].levelCount = subresourceRanges[i].levelCount;
		ranges[i].baseArrayLayer = subresourceRanges[i].baseArrayLayer;
		ranges[i].layerCount = subresourceRanges[i].layerCount;
	}

	VkClearColorValue color;
	std::copy(clearValue.u32, clearValue.u32 + 4, color.uint32);

	this->device->ftbl.vkCmdClearColorImage(this->handle, std::dynamic_pointer_cast< VulkanImage >(image)->handle, g_VulkanImageLayouts[imageLayout], &color, static_cast< uint32_t >(ranges.size()), ranges.data());
}

void VulkanCommandBuffer::ClearColorImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderColorClearValue &clearValue, const RenderImageSubresourceRange &subresourceRange)
{
	VkImageSubresourceRange range;
	range.aspectMask = g_VulkanImageAspectFlags[subresourceRange.aspect];
	range.baseMipLevel = subresourceRange.baseMipLevel;
	range.levelCount = subresourceRange.levelCount;
	range.baseArrayLayer = subresourceRange.baseArrayLayer;
	range.layerCount = subresourceRange.layerCount;

	VkClearColorValue color;
	std::copy(clearValue.u32, clearValue.u32 + 4, color.uint32);

	this->device->ftbl.vkCmdClearColorImage(this->handle, std::dynamic_pointer_cast< VulkanImage >(image)->handle, g_VulkanImageLayouts[imageLayout], &color, 1, &range);
}

void VulkanCommandBuffer::ClearDepthStencilImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderDepthStencilClearValue &clearValue, const std::vector< RenderImageSubresourceRange > &subresourceRanges)
{
	std::vector< VkImageSubresourceRange > ranges(subresourceRanges.size());

	for (size_t i = 0; i < subresourceRanges.size(); i++) {
		ranges[i].aspectMask = g_VulkanImageAspectFlags[subresourceRanges[i].aspect];
		ranges[i].baseMipLevel = subresourceRanges[i].baseMipLevel;
		ranges[i].levelCount = subresourceRanges[i].levelCount;
		ranges[i].baseArrayLayer = subresourceRanges[i].baseArrayLayer;
		ranges[i].layerCount = subresourceRanges[i].layerCount;
	}

	VkClearDepthStencilValue depthStencil;
	depthStencil.depth = clearValue.depth;
	depthStencil.stencil = clearValue.stencil;

	this->device->ftbl.vkCmdClearDepthStencilImage(this->handle, std::dynamic_pointer_cast< VulkanImage >(image)->handle, g_VulkanImageLayouts[imageLayout], &depthStencil, static_cast< uint32_t >(ranges.size()), ranges.data());
}

void VulkanCommandBuffer::ClearDepthStencilImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderDepthStencilClearValue &clearValue, const RenderImageSubresourceRange &subresourceRange)
{
	VkImageSubresourceRange range;
	range.aspectMask = g_VulkanImageAspectFlags[subresourceRange.aspect];
	range.baseMipLevel = subresourceRange.baseMipLevel;
	range.levelCount = subresourceRange.levelCount;
	range.baseArrayLayer = subresourceRange.baseArrayLayer;
	range.layerCount = subresourceRange.layerCount;

	VkClearDepthStencilValue depthStencil;
	depthStencil.depth = clearValue.depth;
	depthStencil.stencil = clearValue.stencil;

	this->device->ftbl.vkCmdClearDepthStencilImage(this->handle, std::dynamic_pointer_cast< VulkanImage >(image)->handle, g_VulkanImageLayouts[imageLayout], &depthStencil, 1, &range);
}

void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	this->device->ftbl.vkCmdDraw(this->handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	this->device->ftbl.vkCmdDrawIndexed(this->handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanCommandBuffer::EndRenderPass()
{
	this->device->ftbl.vkCmdEndRenderPass(this->handle);
}

void VulkanCommandBuffer::BindComputePipeline(std::shared_ptr< IRenderComputePipeline > pipeline)
{
	this->device->ftbl.vkCmdBindPipeline(this->handle, VK_PIPELINE_BIND_POINT_COMPUTE, std::dynamic_pointer_cast< VulkanComputePipeline >(pipeline)->handle);
}

void VulkanCommandBuffer::BindComputeDescriptorSet(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, std::shared_ptr< IRenderDescriptorSet > descriptorSet, uint32_t dynamicOffset)
{
	this->device->ftbl.vkCmdBindDescriptorSets(this->handle, VK_PIPELINE_BIND_POINT_COMPUTE, std::dynamic_pointer_cast< VulkanPipelineLayout >(layout)->handle, firstSet, 1, &std::dynamic_pointer_cast< VulkanDescriptorSet >(descriptorSet)->handle, 1, &dynamicOffset);
}

void VulkanCommandBuffer::BindComputeDescriptorSets(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, const std::vector< std::shared_ptr< IRenderDescriptorSet > > &descriptorSets, const std::vector< uint32_t > &dynamicOffsets)
{
	std::vector< VkDescriptorSet > _descriptorSets(descriptorSets.size());
	for (size_t i = 0; i < descriptorSets.size(); i++)
		_descriptorSets[i] = std::dynamic_pointer_cast< VulkanDescriptorSet >(descriptorSets[i])->handle;

	this->device->ftbl.vkCmdBindDescriptorSets(this->handle, VK_PIPELINE_BIND_POINT_COMPUTE, std::dynamic_pointer_cast< VulkanPipelineLayout >(layout)->handle, firstSet, static_cast< uint32_t >(_descriptorSets.size()), _descriptorSets.data(), static_cast< uint32_t >(dynamicOffsets.size()), dynamicOffsets.data());
}

void VulkanCommandBuffer::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	this->device->ftbl.vkCmdDispatch(this->handle, groupCountX, groupCountY, groupCountZ);
}

void VulkanCommandBuffer::DispatchBase(uint32_t baseCountX, uint32_t baseCountY, uint32_t baseCountZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	this->device->ftbl.vkCmdDispatchBase(this->handle, baseCountX, baseCountY, baseCountZ, groupCountX, groupCountY, groupCountZ);
}

void VulkanCommandBuffer::PipelineBarrier(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const std::vector< RenderBufferMemoryBarrier > &_bufferMemoryBarriers, const std::vector< RenderImageMemoryBarrier > &_imageMemoryBarriers)
{
	std::vector< VkBufferMemoryBarrier > bufferMemoryBarriers(_bufferMemoryBarriers.size());
	std::vector< VkImageMemoryBarrier > imageMemoryBarriers(_imageMemoryBarriers.size());

	for (size_t i = 0; i < _bufferMemoryBarriers.size(); i++) {
		bufferMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferMemoryBarriers[i].pNext = nullptr;
		bufferMemoryBarriers[i].srcAccessMask = static_cast< VkAccessFlags >(_bufferMemoryBarriers[i].sourceAccess);
		bufferMemoryBarriers[i].dstAccessMask = static_cast< VkAccessFlags >(_bufferMemoryBarriers[i].destAccess);
		bufferMemoryBarriers[i].srcQueueFamilyIndex = this->device->GetQueueFamilyIndex(_bufferMemoryBarriers[i].sourceQueue);
		bufferMemoryBarriers[i].dstQueueFamilyIndex = this->device->GetQueueFamilyIndex(_bufferMemoryBarriers[i].destQueue);
		bufferMemoryBarriers[i].buffer = std::dynamic_pointer_cast< VulkanBuffer >(_bufferMemoryBarriers[i].buffer)->handle;
		bufferMemoryBarriers[i].offset = _bufferMemoryBarriers[i].offset;
		bufferMemoryBarriers[i].size = _bufferMemoryBarriers[i].size;
	}

	for (size_t i = 0; i < _imageMemoryBarriers.size(); i++) {
		imageMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarriers[i].pNext = nullptr;
		imageMemoryBarriers[i].srcAccessMask = static_cast< VkAccessFlags >(_imageMemoryBarriers[i].sourceAccess);
		imageMemoryBarriers[i].dstAccessMask = static_cast< VkAccessFlags >(_imageMemoryBarriers[i].destAccess);
		imageMemoryBarriers[i].oldLayout = g_VulkanImageLayouts[_imageMemoryBarriers[i].oldLayout];
		imageMemoryBarriers[i].newLayout = g_VulkanImageLayouts[_imageMemoryBarriers[i].newLayout];
		imageMemoryBarriers[i].srcQueueFamilyIndex = this->device->GetQueueFamilyIndex(_imageMemoryBarriers[i].sourceQueue);
		imageMemoryBarriers[i].dstQueueFamilyIndex = this->device->GetQueueFamilyIndex(_imageMemoryBarriers[i].destQueue);
		imageMemoryBarriers[i].image = std::dynamic_pointer_cast< VulkanImage >(_imageMemoryBarriers[i].image)->handle;
		imageMemoryBarriers[i].subresourceRange.aspectMask = g_VulkanImageAspectFlags[_imageMemoryBarriers[i].subresourceRange.aspect];
		imageMemoryBarriers[i].subresourceRange.baseMipLevel = _imageMemoryBarriers[i].subresourceRange.baseMipLevel;
		imageMemoryBarriers[i].subresourceRange.levelCount = _imageMemoryBarriers[i].subresourceRange.levelCount;
		imageMemoryBarriers[i].subresourceRange.baseArrayLayer = _imageMemoryBarriers[i].subresourceRange.baseArrayLayer;
		imageMemoryBarriers[i].subresourceRange.layerCount = _imageMemoryBarriers[i].subresourceRange.layerCount;
	}

	this->device->ftbl.vkCmdPipelineBarrier(this->handle, static_cast< VkPipelineStageFlags >(sourceStage), static_cast< VkPipelineStageFlags >(destStage), static_cast< VkDependencyFlags >(dependency), 0, nullptr, static_cast< uint32_t >(bufferMemoryBarriers.size()), bufferMemoryBarriers.data(), static_cast< uint32_t >(imageMemoryBarriers.size()), imageMemoryBarriers.data());
}

void VulkanCommandBuffer::PipelineBarrier(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const std::vector< RenderMemoryBarrier > &_memoryBarriers, const std::vector< RenderBufferMemoryBarrier > &_bufferMemoryBarriers, const std::vector< RenderImageMemoryBarrier > &_imageMemoryBarriers)
{
	std::vector< VkMemoryBarrier > memoryBarriers(_memoryBarriers.size());
	std::vector< VkBufferMemoryBarrier > bufferMemoryBarriers(_bufferMemoryBarriers.size());
	std::vector< VkImageMemoryBarrier > imageMemoryBarriers(_imageMemoryBarriers.size());

	for (size_t i = 0; i < _memoryBarriers.size(); i++) {
		memoryBarriers[i].sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		memoryBarriers[i].pNext = nullptr;
		memoryBarriers[i].srcAccessMask = static_cast< VkAccessFlags >(_memoryBarriers[i].sourceAccess);
		memoryBarriers[i].dstAccessMask = static_cast< VkAccessFlags >(_memoryBarriers[i].destAccess);
	}

	for (size_t i = 0; i < _bufferMemoryBarriers.size(); i++) {
		bufferMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferMemoryBarriers[i].pNext = nullptr;
		bufferMemoryBarriers[i].srcAccessMask = static_cast< VkAccessFlags >(_bufferMemoryBarriers[i].sourceAccess);
		bufferMemoryBarriers[i].dstAccessMask = static_cast< VkAccessFlags >(_bufferMemoryBarriers[i].destAccess);
		bufferMemoryBarriers[i].srcQueueFamilyIndex = this->device->GetQueueFamilyIndex(_bufferMemoryBarriers[i].sourceQueue);
		bufferMemoryBarriers[i].dstQueueFamilyIndex = this->device->GetQueueFamilyIndex(_bufferMemoryBarriers[i].destQueue);
		bufferMemoryBarriers[i].buffer = std::dynamic_pointer_cast< VulkanBuffer >(_bufferMemoryBarriers[i].buffer)->handle;
		bufferMemoryBarriers[i].offset = _bufferMemoryBarriers[i].offset;
		bufferMemoryBarriers[i].size = _bufferMemoryBarriers[i].size;
	}

	for (size_t i = 0; i < _imageMemoryBarriers.size(); i++) {
		imageMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarriers[i].pNext = nullptr;
		imageMemoryBarriers[i].srcAccessMask = static_cast< VkAccessFlags >(_imageMemoryBarriers[i].sourceAccess);
		imageMemoryBarriers[i].dstAccessMask = static_cast< VkAccessFlags >(_imageMemoryBarriers[i].destAccess);
		imageMemoryBarriers[i].oldLayout = g_VulkanImageLayouts[_imageMemoryBarriers[i].oldLayout];
		imageMemoryBarriers[i].newLayout = g_VulkanImageLayouts[_imageMemoryBarriers[i].newLayout];
		imageMemoryBarriers[i].srcQueueFamilyIndex = this->device->GetQueueFamilyIndex(_imageMemoryBarriers[i].sourceQueue);
		imageMemoryBarriers[i].dstQueueFamilyIndex = this->device->GetQueueFamilyIndex(_imageMemoryBarriers[i].destQueue);
		imageMemoryBarriers[i].image = std::dynamic_pointer_cast< VulkanImage >(_imageMemoryBarriers[i].image)->handle;
		imageMemoryBarriers[i].subresourceRange.aspectMask = g_VulkanImageAspectFlags[_imageMemoryBarriers[i].subresourceRange.aspect];
		imageMemoryBarriers[i].subresourceRange.baseMipLevel = _imageMemoryBarriers[i].subresourceRange.baseMipLevel;
		imageMemoryBarriers[i].subresourceRange.levelCount = _imageMemoryBarriers[i].subresourceRange.levelCount;
		imageMemoryBarriers[i].subresourceRange.baseArrayLayer = _imageMemoryBarriers[i].subresourceRange.baseArrayLayer;
		imageMemoryBarriers[i].subresourceRange.layerCount = _imageMemoryBarriers[i].subresourceRange.layerCount;
	}

	this->device->ftbl.vkCmdPipelineBarrier(this->handle, static_cast< VkPipelineStageFlags >(sourceStage), static_cast< VkPipelineStageFlags >(destStage), static_cast< VkDependencyFlags >(dependency), static_cast< uint32_t >(memoryBarriers.size()), memoryBarriers.data(), static_cast< uint32_t >(bufferMemoryBarriers.size()), bufferMemoryBarriers.data(), static_cast< uint32_t >(imageMemoryBarriers.size()), imageMemoryBarriers.data());
}

void VulkanCommandBuffer::PipelineBarrierMemory(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const RenderMemoryBarrier &_memoryBarrier)
{
	VkMemoryBarrier memoryBarrier;
	memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	memoryBarrier.pNext = nullptr;
	memoryBarrier.srcAccessMask = static_cast< VkAccessFlags >(_memoryBarrier.sourceAccess);
	memoryBarrier.dstAccessMask = static_cast< VkAccessFlags >(_memoryBarrier.destAccess);

	this->device->ftbl.vkCmdPipelineBarrier(this->handle, static_cast< VkPipelineStageFlags >(sourceStage), static_cast< VkPipelineStageFlags >(destStage), static_cast< VkDependencyFlags >(dependency), 1, &memoryBarrier, 0, nullptr, 0, nullptr);
}

void VulkanCommandBuffer::PipelineBarrierBuffer(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const RenderBufferMemoryBarrier &_bufferMemoryBarrier)
{
	VkBufferMemoryBarrier bufferMemoryBarrier;
	bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	bufferMemoryBarrier.pNext = nullptr;
	bufferMemoryBarrier.srcAccessMask = static_cast< VkAccessFlags >(_bufferMemoryBarrier.sourceAccess);
	bufferMemoryBarrier.dstAccessMask = static_cast< VkAccessFlags >(_bufferMemoryBarrier.destAccess);
	bufferMemoryBarrier.srcQueueFamilyIndex = this->device->GetQueueFamilyIndex(_bufferMemoryBarrier.sourceQueue);
	bufferMemoryBarrier.dstQueueFamilyIndex = this->device->GetQueueFamilyIndex(_bufferMemoryBarrier.destQueue);
	bufferMemoryBarrier.buffer = std::dynamic_pointer_cast< VulkanBuffer >(_bufferMemoryBarrier.buffer)->handle;
	bufferMemoryBarrier.offset = _bufferMemoryBarrier.offset;
	bufferMemoryBarrier.size = _bufferMemoryBarrier.size;

	this->device->ftbl.vkCmdPipelineBarrier(this->handle, static_cast< VkPipelineStageFlags >(sourceStage), static_cast< VkPipelineStageFlags >(destStage), static_cast< VkDependencyFlags >(dependency), 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
}

void VulkanCommandBuffer::PipelineBarrierImage(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const RenderImageMemoryBarrier &_imageMemoryBarrier)
{
	VkImageMemoryBarrier imageMemoryBarrier;
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.srcAccessMask = static_cast< VkAccessFlags >(_imageMemoryBarrier.sourceAccess);
	imageMemoryBarrier.dstAccessMask = static_cast< VkAccessFlags >(_imageMemoryBarrier.destAccess);
	imageMemoryBarrier.oldLayout = g_VulkanImageLayouts[_imageMemoryBarrier.oldLayout];
	imageMemoryBarrier.newLayout = g_VulkanImageLayouts[_imageMemoryBarrier.newLayout];
	imageMemoryBarrier.srcQueueFamilyIndex = this->device->GetQueueFamilyIndex(_imageMemoryBarrier.sourceQueue);
	imageMemoryBarrier.dstQueueFamilyIndex = this->device->GetQueueFamilyIndex(_imageMemoryBarrier.destQueue);
	imageMemoryBarrier.image = std::dynamic_pointer_cast< VulkanImage >(_imageMemoryBarrier.image)->handle;
	imageMemoryBarrier.subresourceRange.aspectMask = g_VulkanImageAspectFlags[_imageMemoryBarrier.subresourceRange.aspect];
	imageMemoryBarrier.subresourceRange.baseMipLevel = _imageMemoryBarrier.subresourceRange.baseMipLevel;
	imageMemoryBarrier.subresourceRange.levelCount = _imageMemoryBarrier.subresourceRange.levelCount;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = _imageMemoryBarrier.subresourceRange.baseArrayLayer;
	imageMemoryBarrier.subresourceRange.layerCount = _imageMemoryBarrier.subresourceRange.layerCount;

	this->device->ftbl.vkCmdPipelineBarrier(this->handle, static_cast< VkPipelineStageFlags >(sourceStage), static_cast< VkPipelineStageFlags >(destStage), static_cast< VkDependencyFlags >(dependency), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void VulkanCommandBuffer::CopyBuffer(std::shared_ptr< IRenderBuffer > source, std::shared_ptr< IRenderBuffer > dest, const RenderBufferCopy &_region)
{
	VkBufferCopy region;
	region.srcOffset = _region.sourceOffset;
	region.dstOffset = _region.destOffset;
	region.size = _region.size;

	this->device->ftbl.vkCmdCopyBuffer(this->handle, std::dynamic_pointer_cast< VulkanBuffer >(source)->handle, std::dynamic_pointer_cast< VulkanBuffer >(dest)->handle, 1, &region);
}

void VulkanCommandBuffer::CopyBuffer(std::shared_ptr< IRenderBuffer > source, std::shared_ptr< IRenderBuffer > dest, const std::vector< RenderBufferCopy > &_regions)
{
	std::vector< VkBufferCopy > regions(_regions.size());

	for (size_t i = 0; i < _regions.size(); i++) {
		regions[i].srcOffset = _regions[i].sourceOffset;
		regions[i].dstOffset = _regions[i].destOffset;
		regions[i].size = _regions[i].size;
	}

	this->device->ftbl.vkCmdCopyBuffer(this->handle, std::dynamic_pointer_cast< VulkanBuffer >(source)->handle, std::dynamic_pointer_cast< VulkanBuffer >(dest)->handle, static_cast< uint32_t >(regions.size()), regions.data());
}

void VulkanCommandBuffer::CopyBufferToImage(std::shared_ptr< IRenderBuffer > sourceBuffer, std::shared_ptr< IRenderImage > destImage, EImageLayout destImageLayout, const RenderBufferImageCopy &_region)
{
	VkBufferImageCopy region;
	region.bufferOffset = _region.bufferOffset;
	region.bufferRowLength = _region.bufferRowLength;
	region.bufferImageHeight = _region.bufferImageHeight;
	region.imageSubresource.aspectMask = g_VulkanImageAspectFlags[_region.imageSubresource.aspect];
	region.imageSubresource.mipLevel = _region.imageSubresource.mipLevel;
	region.imageSubresource.baseArrayLayer = _region.imageSubresource.baseArrayLayer;
	region.imageSubresource.layerCount = _region.imageSubresource.layerCount;
	region.imageOffset = { _region.offset.x, _region.offset.y, _region.offset.z };
	region.imageExtent = { _region.extent.width, _region.extent.height, _region.extent.depth };

	this->device->ftbl.vkCmdCopyBufferToImage(this->handle, std::dynamic_pointer_cast< VulkanBuffer >(sourceBuffer)->handle, std::dynamic_pointer_cast< VulkanImage >(destImage)->handle, g_VulkanImageLayouts[destImageLayout], 1, &region);
}

void VulkanCommandBuffer::CopyBufferToImage(std::shared_ptr< IRenderBuffer > sourceBuffer, std::shared_ptr< IRenderImage > destImage, EImageLayout destImageLayout, const std::vector< RenderBufferImageCopy > &_regions)
{
	std::vector< VkBufferImageCopy > regions(_regions.size());

	for (size_t i = 0; i < _regions.size(); i++) {
		regions[i].bufferOffset = _regions[i].bufferOffset;
		regions[i].bufferRowLength = _regions[i].bufferRowLength;
		regions[i].bufferImageHeight = _regions[i].bufferImageHeight;
		regions[i].imageSubresource.aspectMask = g_VulkanImageAspectFlags[_regions[i].imageSubresource.aspect];
		regions[i].imageSubresource.mipLevel = _regions[i].imageSubresource.mipLevel;
		regions[i].imageSubresource.baseArrayLayer = _regions[i].imageSubresource.baseArrayLayer;
		regions[i].imageSubresource.layerCount = _regions[i].imageSubresource.layerCount;
		regions[i].imageOffset = { _regions[i].offset.x, _regions[i].offset.y, _regions[i].offset.z };
		regions[i].imageExtent = { _regions[i].extent.width, _regions[i].extent.height, _regions[i].extent.depth };
	}

	this->device->ftbl.vkCmdCopyBufferToImage(this->handle, std::dynamic_pointer_cast< VulkanBuffer >(sourceBuffer)->handle, std::dynamic_pointer_cast< VulkanImage >(destImage)->handle, g_VulkanImageLayouts[destImageLayout], static_cast< uint32_t >(regions.size()), regions.data());
}

void VulkanCommandBuffer::CopyImage(std::shared_ptr< IRenderImage > source, EImageLayout sourceImageLayout, std::shared_ptr< IRenderImage > dest, EImageLayout destImageLayout, const RenderImageCopy &_region)
{
	VkImageCopy region;
	region.srcSubresource.aspectMask = g_VulkanImageAspectFlags[_region.sourceSubresource.aspect];
	region.srcSubresource.mipLevel = _region.sourceSubresource.mipLevel;
	region.srcSubresource.baseArrayLayer = _region.sourceSubresource.baseArrayLayer;
	region.srcSubresource.layerCount = _region.sourceSubresource.layerCount;
	region.srcOffset = { _region.sourceOffset.x, _region.sourceOffset.y, _region.sourceOffset.z };
	region.dstSubresource.aspectMask = g_VulkanImageAspectFlags[_region.destSubresource.aspect];
	region.dstSubresource.mipLevel = _region.destSubresource.mipLevel;
	region.dstSubresource.baseArrayLayer = _region.destSubresource.baseArrayLayer;
	region.dstSubresource.layerCount = _region.destSubresource.layerCount;
	region.dstOffset = { _region.destOffset.x, _region.destOffset.y, _region.destOffset.z };
	region.extent = { _region.extent.width, _region.extent.height, _region.extent.depth };

	this->device->ftbl.vkCmdCopyImage(this->handle, std::dynamic_pointer_cast< VulkanImage >(source)->handle, g_VulkanImageLayouts[sourceImageLayout], std::dynamic_pointer_cast< VulkanImage >(dest)->handle, g_VulkanImageLayouts[destImageLayout], 1, &region);
}

void VulkanCommandBuffer::CopyImage(std::shared_ptr< IRenderImage > source, EImageLayout sourceImageLayout, std::shared_ptr< IRenderImage > dest, EImageLayout destImageLayout, const std::vector< RenderImageCopy > &_regions)
{
	std::vector< VkImageCopy > regions(_regions.size());

	for (size_t i = 0; i < _regions.size(); i++) {
		regions[i].srcSubresource.aspectMask = g_VulkanImageAspectFlags[_regions[i].sourceSubresource.aspect];
		regions[i].srcSubresource.mipLevel = _regions[i].sourceSubresource.mipLevel;
		regions[i].srcSubresource.baseArrayLayer = _regions[i].sourceSubresource.baseArrayLayer;
		regions[i].srcSubresource.layerCount = _regions[i].sourceSubresource.layerCount;
		regions[i].srcOffset = { _regions[i].sourceOffset.x, _regions[i].sourceOffset.y, _regions[i].sourceOffset.z };
		regions[i].dstSubresource.aspectMask = g_VulkanImageAspectFlags[_regions[i].destSubresource.aspect];
		regions[i].dstSubresource.mipLevel = _regions[i].destSubresource.mipLevel;
		regions[i].dstSubresource.baseArrayLayer = _regions[i].destSubresource.baseArrayLayer;
		regions[i].dstSubresource.layerCount = _regions[i].destSubresource.layerCount;
		regions[i].dstOffset = { _regions[i].destOffset.x, _regions[i].destOffset.y, _regions[i].destOffset.z };
		regions[i].extent = { _regions[i].extent.width, _regions[i].extent.height, _regions[i].extent.depth };
	}

	this->device->ftbl.vkCmdCopyImage(this->handle, std::dynamic_pointer_cast< VulkanImage >(source)->handle, g_VulkanImageLayouts[sourceImageLayout], std::dynamic_pointer_cast< VulkanImage >(dest)->handle, g_VulkanImageLayouts[destImageLayout], static_cast< uint32_t >(regions.size()), regions.data());
}

void VulkanCommandBuffer::CopyImageToBuffer(std::shared_ptr< IRenderImage > sourceImage, EImageLayout sourceImageLayout, std::shared_ptr< IRenderBuffer > destBuffer, const RenderBufferImageCopy &_region)
{
	VkBufferImageCopy region;
	region.bufferOffset = _region.bufferOffset;
	region.bufferRowLength = _region.bufferRowLength;
	region.bufferImageHeight = _region.bufferImageHeight;
	region.imageSubresource.aspectMask = g_VulkanImageAspectFlags[_region.imageSubresource.aspect];
	region.imageSubresource.mipLevel = _region.imageSubresource.mipLevel;
	region.imageSubresource.baseArrayLayer = _region.imageSubresource.baseArrayLayer;
	region.imageSubresource.layerCount = _region.imageSubresource.layerCount;
	region.imageOffset = { _region.offset.x, _region.offset.y, _region.offset.z };
	region.imageExtent = { _region.extent.width, _region.extent.height, _region.extent.depth };

	this->device->ftbl.vkCmdCopyImageToBuffer(this->handle, std::dynamic_pointer_cast< VulkanImage >(sourceImage)->handle, g_VulkanImageLayouts[sourceImageLayout], std::dynamic_pointer_cast< VulkanBuffer >(destBuffer)->handle, 1, &region);
}

void VulkanCommandBuffer::CopyImageToBuffer(std::shared_ptr< IRenderImage > sourceImage, EImageLayout sourceImageLayout, std::shared_ptr< IRenderBuffer > destBuffer, const std::vector< RenderBufferImageCopy > &_regions)
{
	std::vector< VkBufferImageCopy > regions(_regions.size());

	for (size_t i = 0; i < _regions.size(); i++) {
		regions[i].bufferOffset = _regions[i].bufferOffset;
		regions[i].bufferRowLength = _regions[i].bufferRowLength;
		regions[i].bufferImageHeight = _regions[i].bufferImageHeight;
		regions[i].imageSubresource.aspectMask = g_VulkanImageAspectFlags[_regions[i].imageSubresource.aspect];
		regions[i].imageSubresource.mipLevel = _regions[i].imageSubresource.mipLevel;
		regions[i].imageSubresource.baseArrayLayer = _regions[i].imageSubresource.baseArrayLayer;
		regions[i].imageSubresource.layerCount = _regions[i].imageSubresource.layerCount;
		regions[i].imageOffset = { _regions[i].offset.x, _regions[i].offset.y, _regions[i].offset.z };
		regions[i].imageExtent = { _regions[i].extent.width, _regions[i].extent.height, _regions[i].extent.depth };
	}

	this->device->ftbl.vkCmdCopyImageToBuffer(this->handle, std::dynamic_pointer_cast< VulkanImage >(sourceImage)->handle, g_VulkanImageLayouts[sourceImageLayout], std::dynamic_pointer_cast< VulkanBuffer >(destBuffer)->handle, static_cast< uint32_t >(regions.size()), regions.data());
}
