#ifndef LEVEL0_RENDER_API_H
#define LEVEL0_RENDER_API_H

#include "defs.h"

class RenderAPI;
class IRenderPhysicalDevice;
class IRenderDevice;
class IRenderAllocator;
class IRenderBuffer;
class IRenderImage; // this could be 1D, 2D, or 3D
class IRenderPass;
class IRenderFramebuffer;
class IRenderSwapchain;
class IRenderCommandPool;
class IRenderCommandBuffer;
class IRenderFence;
class IRenderSemaphore;
class IRenderSampler;
class IRenderDescriptorSetLayout;
class IRenderDescriptorPool;
class IRenderDescriptorSet;
class IRenderPipelineLayout;
class IRenderShaderModule;
class IRenderComputePipeline;
class IRenderGraphicsPipeline;
// class IRenderRaytracingPipeline;

struct RenderOffset2D {
	int32_t x, y;
};

struct RenderOffset3D {
	int32_t x, y, z;
};

struct RenderExtent2D {
	uint32_t width, height;
};

struct RenderExtent3D {
	uint32_t width, height, depth;
};

struct RenderRectangle {
	RenderOffset2D offset;
	RenderExtent2D extent;
};

struct RenderPhysicalDeviceInfo {
	struct {
		std::string_view name;
		uint32_t vendorID, deviceID;
		EPhysicalDeviceType type;
	} hardware;

	uint64_t maxDeviceMemory;
	uint64_t maxMemory;
};

struct RenderPhysicalDeviceFeatures {
	bool hasSmartAccessMemory;
	bool vulkanAMDAPUHeapWorkaround; // For determining Vulkan memory heap workarounds
};

struct RenderImageSubresourceLayers {
	EImageAspect aspect;
	uint32_t mipLevel;
	uint32_t baseArrayLayer;
	uint32_t layerCount;
};

struct RenderImageSubresourceRange {
	EImageAspect aspect;
	uint32_t baseMipLevel;
	uint32_t levelCount;
	uint32_t baseArrayLayer;
	uint32_t layerCount;
};

struct RenderDescriptorSetLayoutBinding {
	uint32_t binding;
	EDescriptorType descriptorType;
	uint32_t descriptorCount;
	EShaderStage stageFlags;
	std::vector< std::shared_ptr< IRenderSampler > > immutableSamplers;
};

struct RenderDescriptorPoolSize {
	EDescriptorType type;
	uint32_t count;
};

struct RenderPushConstantRange {
	EShaderStage stageFlags;
	uint32_t offset;
	uint32_t size;
};

struct RenderAttachmentDescription {
	EImageFormat format;
	EAttachmentLoadOp loadOp;
	EAttachmentStoreOp storeOp;
	EAttachmentLoadOp stencilLoadOp;
	EAttachmentStoreOp stencilStoreOp;
	EImageLayout initialLayout;
	EImageLayout finalLayout;
};

// this will always be graphics
struct RenderAttachmentReference {
	uint32_t attachment;
	EImageLayout layout;
};

struct RenderSubpassDescription {
	bool depthStencilEnable;
	std::vector< RenderAttachmentReference > inputAttachments;
	std::vector< RenderAttachmentReference > colorAttachments;
	RenderAttachmentReference depthStencilAttachment;
	std::vector< uint32_t > preserveAttachments;
};

struct RenderSubpassDependency {
	uint32_t sourceSubpass;
	uint32_t destSubpass;
	EPipelineStage sourceStageMask;
	EPipelineStage destStageMask;
	EResourceAccess sourceAccessMask;
	EResourceAccess destAccessMask;
	EDependencyFlag dependencyFlags;
};

struct RenderVertexInputBindingDescription {
	uint32_t binding;
	uint32_t stride;
	// 0 for Vertex, 1 for Instance
	uint32_t inputRate;
};

struct RenderVertexInputAttributeDescription {
	uint32_t location;
	uint32_t binding;
	EImageFormat format;
	uint32_t offset;
};

struct RenderSamplerStateDescription {
	ETextureFilterMode magFilter;
	ETextureFilterMode minFilter;
	ETextureFilterMode mipmapMode;
	ETextureAddressMode addressModeU;
	ETextureAddressMode addressModeV;
	ETextureAddressMode addressModeW;
	ETextureComparisonFunction comparisonFunc;
	float mipLODBias;
	float maxLOD;
	float minLOD;
	float maxAniso;
	float borderColor[4];
};

struct RenderViewport {
	float x;
	float y;
	float width;
	float height;
	float minDepth;
	float maxDepth;
};

struct RenderStencilOpState {
	EStencilOp failOp;
	EStencilOp passOp;
	EStencilOp depthFailOp;
	ETextureComparisonFunction compareMode;
	uint32_t compareMask;
	uint32_t writeMask;
	uint32_t reference;
};

struct RenderColorBlendAttachmentState {
	bool enable;
	EBlendFactor sourceColorBlendFactor;
	EBlendFactor destColorBlendFactor;
	EBlendOp colorBlendOp;
	EBlendFactor sourceAlphaBlendFactor;
	EBlendFactor destAlphaBlendFactor;
	EBlendOp alphaBlendOp;
	EColorComponent colorWriteMask;
};

union RenderColorClearValue {
	float f32[4];
	int32_t s32[4];
	uint32_t u32[4];
};

struct RenderDepthStencilClearValue {
	float depth;
	uint32_t stencil;
};

union RenderClearValue {
	RenderColorClearValue color;
	RenderDepthStencilClearValue depthStencil;
};

struct RenderClearAttachment {
	EImageAspect aspect;
	uint32_t colorAttachment;
	RenderClearValue clearValue;
};

struct RenderClearRectangle {
	RenderRectangle rectangle;
	uint32_t baseArrayLayer;
	uint32_t layerCount;
};

struct RenderMemoryBarrier {
	EResourceAccess sourceAccess;
	EResourceAccess destAccess;
};

struct RenderBufferMemoryBarrier {
	EResourceAccess sourceAccess;
	EResourceAccess destAccess;
	EDeviceQueue sourceQueue;
	EDeviceQueue destQueue;
	std::shared_ptr< IRenderBuffer > buffer;
	uint64_t offset;
	uint64_t size;
};

struct RenderImageMemoryBarrier {
	EResourceAccess sourceAccess;
	EResourceAccess destAccess;
	EImageLayout oldLayout;
	EImageLayout newLayout;
	EDeviceQueue sourceQueue;
	EDeviceQueue destQueue;
	std::shared_ptr< IRenderImage > image;
	RenderImageSubresourceRange subresourceRange;
};

struct RenderBufferCopy {
	uint64_t sourceOffset;
	uint64_t destOffset;
	uint64_t size;
};

struct RenderBufferImageCopy {
	uint64_t bufferOffset;
	uint32_t bufferRowLength;
	uint32_t bufferImageHeight;
	RenderImageSubresourceLayers imageSubresource;
	RenderOffset3D offset;
	RenderExtent3D extent;
};

struct RenderImageCopy {
	RenderImageSubresourceLayers sourceSubresource;
	RenderOffset3D sourceOffset;
	RenderImageSubresourceLayers destSubresource;
	RenderOffset3D destOffset;
	RenderExtent3D extent;
};

struct RenderPipelineShaderInfo {
	EShaderType type;
	std::string_view filePath;
};

struct RenderDrawIndirectParameter {
	uint32_t vertexCount;
	uint32_t instanceCount;
	uint32_t firstIndex;
	uint32_t firstInstance;
};

struct RenderDrawIndexedIndirectParameter {
	uint32_t indexCount;
	uint32_t instanceCount;
	uint32_t firstIndex;
	int32_t vertexOffset;
	uint32_t firstInstance;
};

class RenderAPI {
public:
	const char *apiName = nullptr;
	std::vector< std::shared_ptr< IRenderPhysicalDevice > > physicalDevices;

	RenderAPI() = delete;
	inline RenderAPI(const char *_apiName) : apiName(_apiName) {}
	virtual ~RenderAPI() = default;
};

std::unique_ptr< RenderAPI > Render_CreateAPI(ERendererType type);

class IRenderPhysicalDevice {
public:
	RenderPhysicalDeviceFeatures apiFeatures = {};

	virtual ~IRenderPhysicalDevice() = default;

	virtual RenderPhysicalDeviceInfo GetInfo() = 0;

	virtual std::shared_ptr< IRenderDevice > CreateLogicalDevice() = 0;
};

class IRenderDevice {
public:
	virtual ~IRenderDevice() = default;

	virtual void WaitIdle() = 0;
	virtual void WaitIdleQueue(EDeviceQueue queue) = 0;

	// General
	virtual std::shared_ptr< IRenderAllocator > CreateAllocator(const std::string &name, EResourceMemoryUsage memoryUsage, uint64_t minAlign, uint64_t blockSize, size_t minBlockCount, size_t maxBlockCount) = 0;
	virtual std::shared_ptr< IRenderCommandPool > CreateCommandPool(EDeviceQueue queue, ECommandPoolUsage usage) = 0;
	virtual std::shared_ptr< IRenderDescriptorPool > CreateDescriptorPool(uint32_t maxSets, const std::vector< RenderDescriptorPoolSize > &poolSizes) = 0;
	virtual std::shared_ptr< IRenderFence > CreateFence(bool signaled) = 0;
	virtual std::shared_ptr< IRenderSemaphore > CreateSemaphore() = 0;

	// Pipeline Layouts and Descriptor Set Layouts
	virtual std::shared_ptr< IRenderDescriptorSetLayout > CreateDescriptorSetLayout(const std::vector< RenderDescriptorSetLayoutBinding > &bindings) = 0;
	virtual std::shared_ptr< IRenderPipelineLayout > CreatePipelineLayout(const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &layouts) = 0;
	virtual std::shared_ptr< IRenderPipelineLayout > CreatePipelineLayout(const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &layouts, const std::vector< RenderPushConstantRange > &ranges) = 0;

	// Render
	virtual std::shared_ptr< IRenderPass > CreateRenderPass(const std::vector< RenderAttachmentDescription > &attachments, const std::vector< RenderSubpassDescription > &subpasses, const std::vector< RenderSubpassDependency > &subpassDependencies) = 0;
	virtual std::shared_ptr< IRenderFramebuffer > CreateFramebuffer(std::shared_ptr< IRenderPass > renderPass, const std::vector< std::shared_ptr< IRenderImage > > &images, const RenderExtent2D &extent) = 0;
	virtual std::shared_ptr< IRenderFramebuffer > CreateFramebuffer(std::shared_ptr< IRenderPass > renderPass, std::shared_ptr< IRenderImage > image, const RenderExtent2D &extent) = 0;
	virtual std::shared_ptr< IRenderSwapchain > CreateSwapchain(ESwapchainPresentMode presentMode, EDeviceQueue preferPresentQueue) = 0; // will handle image counts, etc...

	// Samplers
	virtual std::shared_ptr< IRenderSampler > CreateSampler(const RenderSamplerStateDescription &state) = 0;

	// Pipelines
	virtual std::shared_ptr< IRenderShaderModule > CreateShaderModule(EShaderType type, const void *blob, size_t blobSize) = 0;
	virtual std::shared_ptr< IRenderComputePipeline > CreateComputePipeline(std::shared_ptr< IRenderShaderModule > shaderModule, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderComputePipeline > basePipeline) = 0;
	virtual std::shared_ptr< IRenderGraphicsPipeline > CreateGraphicsPipeline(const std::vector< std::shared_ptr< IRenderShaderModule > > &shaderModules, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderGraphicsPipeline > basePipeline, std::shared_ptr< IRenderPass > renderPass, uint32_t subpass) = 0;

	// Command Submission
	virtual void Submit(EDeviceQueue queue, std::shared_ptr< IRenderCommandBuffer > commandBuffer, std::shared_ptr< IRenderSemaphore > waitSemaphore, EPipelineStage waitPipelineStage, std::shared_ptr< IRenderSemaphore > signalSemaphore, std::shared_ptr< IRenderFence > fence) = 0;
	virtual void Submit(EDeviceQueue queue, std::shared_ptr< IRenderCommandBuffer > commandBuffer, const std::vector< std::shared_ptr< IRenderSemaphore > > &waitSemaphores, const std::vector< EPipelineStage > &waitPipelineStages, std::shared_ptr< IRenderSemaphore > signalSemaphore, std::shared_ptr< IRenderFence > fence) = 0;
	virtual void Submit(EDeviceQueue queue, std::shared_ptr< IRenderCommandBuffer > commandBuffer, const std::vector< std::shared_ptr< IRenderSemaphore > > &waitSemaphores, const std::vector< EPipelineStage > &waitPipelineStages, const std::vector< std::shared_ptr< IRenderSemaphore > > &signalSemaphores, std::shared_ptr< IRenderFence > fence) = 0;
	virtual void Submit(EDeviceQueue queue, const std::vector< std::shared_ptr< IRenderCommandBuffer > > &commandBuffers, std::shared_ptr< IRenderSemaphore > waitSemaphore, EPipelineStage waitPipelineStage, std::shared_ptr< IRenderSemaphore > signalSemaphore, std::shared_ptr< IRenderFence > fence) = 0;
	virtual void Submit(EDeviceQueue queue, const std::vector< std::shared_ptr< IRenderCommandBuffer > > &commandBuffers, const std::vector< std::shared_ptr< IRenderSemaphore > > &waitSemaphores, const std::vector< EPipelineStage > &waitPipelineStages, std::shared_ptr< IRenderSemaphore > signalSemaphore, std::shared_ptr< IRenderFence > fence) = 0;
	virtual void Submit(EDeviceQueue queue, const std::vector< std::shared_ptr< IRenderCommandBuffer > > &commandBuffers, const std::vector< std::shared_ptr< IRenderSemaphore > > &waitSemaphores, const std::vector< EPipelineStage > &waitPipelineStages, const std::vector< std::shared_ptr< IRenderSemaphore > > &signalSemaphores, std::shared_ptr< IRenderFence > fence) = 0;
};

class IRenderAllocator {
public:
	std::string name;

	virtual ~IRenderAllocator() = default;

	virtual std::shared_ptr< IRenderBuffer > AllocateBuffer(EBufferUsage usage, uint64_t size) = 0;
	virtual std::shared_ptr< IRenderImage > AllocateImage(EImageType type, EImageFormat format, EImageUsage usage, const RenderExtent3D &extent3d, const RenderImageSubresourceRange &subresourceRange) = 0;

	virtual EResourceMemoryUsage GetResourceMemoryUsage() = 0;

	// Dangerous call
	virtual void Compactify() = 0;

#ifndef NDEBUG
	virtual void DumpInfo(const std::string &filename) = 0;
#endif
};

class IRenderBuffer {
public:
	virtual ~IRenderBuffer() = default;

	virtual void *Map() = 0;
	virtual void Unmap() = 0;

	virtual EResourceMemoryUsage GetResourceMemoryUsage() = 0;
	virtual EBufferUsage GetUsage() = 0;
	virtual uint64_t GetSize() = 0;
};

class IRenderImage {
public:
	virtual ~IRenderImage() = default;

	virtual void *Map() = 0;
	virtual void Unmap() = 0;

	virtual EResourceMemoryUsage GetResourceMemoryUsage() = 0;
	virtual EImageUsage GetUsage() = 0;
	virtual EImageFormat GetFormat() = 0;
	virtual RenderExtent3D GetExtent() = 0;
	virtual RenderImageSubresourceRange GetSubresourceRange() = 0;
};

class IRenderPass {
public:
	virtual ~IRenderPass() = default;
};

class IRenderFramebuffer {
public:
	virtual ~IRenderFramebuffer() = default;

	virtual std::vector< std::shared_ptr< IRenderImage > > GetImages() = 0;
	virtual std::shared_ptr< IRenderPass > GetRenderPass() = 0;
	virtual RenderExtent2D GetExtent() = 0;
	virtual bool IsMultiView() = 0;
};

class IRenderSwapchain {
public:
	virtual ~IRenderSwapchain() = default;

	virtual ESwapchainResult GetAvailableImage(std::shared_ptr< IRenderSemaphore > semaphore, std::shared_ptr< IRenderFence > fence, uint64_t timeout, uint32_t &index) = 0;
	virtual RenderExtent2D GetExtent() = 0;
	virtual std::shared_ptr< IRenderImage > GetImage(uint32_t index) = 0;
	virtual std::vector< std::shared_ptr< IRenderImage > > GetImages() = 0;
	virtual uint32_t GetMaxImages() = 0;
	virtual EDeviceQueue GetPresentingQueue() = 0;

	virtual ESwapchainResult PresentImage(std::shared_ptr< IRenderSemaphore > waitSemaphore, uint32_t index) = 0;
	virtual void Recreate(ESwapchainPresentMode presentMode, EDeviceQueue preferPresentQueue) = 0;
};

class IRenderCommandPool {
public:
	virtual ~IRenderCommandPool() = default;

	virtual std::shared_ptr< IRenderCommandBuffer > AllocateSingle(ECommandBufferLevel level) = 0;
	virtual void AllocateBulk(std::vector< std::shared_ptr< IRenderCommandBuffer > > &commandBuffers, ECommandBufferLevel level) = 0;
};

class IRenderCommandBuffer {
public:
	virtual ~IRenderCommandBuffer() = default;

	virtual void Begin(ECommandBufferUsage usage) = 0;
	virtual void End() = 0;

	// graphics
	virtual void BindGraphicsPipeline(std::shared_ptr< IRenderGraphicsPipeline > pipeline) = 0;
	virtual void BindGraphicsDescriptorSet(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, std::shared_ptr< IRenderDescriptorSet > descriptorSet, uint32_t dynamicOffset) = 0;
	virtual void BindGraphicsDescriptorSets(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, const std::vector< std::shared_ptr< IRenderDescriptorSet > > &descriptorSets, const std::vector< uint32_t > &dynamicOffsets) = 0;
	virtual void BindVertexBuffer(uint32_t firstBinding, std::shared_ptr< IRenderBuffer > buffer, uint64_t offset) = 0;
	virtual void BindVertexBuffers(uint32_t firstBinding, const std::vector< std::shared_ptr< IRenderBuffer > > &buffers, const std::vector< uint64_t > &offsets) = 0;
	virtual void BindIndexBufferU16(std::shared_ptr< IRenderBuffer > buffer, uint64_t offset) = 0;
	virtual void BeginRenderPass(std::shared_ptr< IRenderPass > renderPass, std::shared_ptr< IRenderFramebuffer > framebuffer, RenderRectangle renderArea, const std::vector< RenderClearValue > &clearValues, ESubpassContents subpassContents) = 0;
	virtual void ClearAttachment(const RenderClearAttachment &attachment, const RenderClearRectangle &rectangle) = 0;
	virtual void ClearAttachments(const std::vector< RenderClearAttachment > &attachments, const std::vector< RenderClearRectangle > &rectangles) = 0;
	virtual void ClearColorImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderColorClearValue &clearValue, const std::vector< RenderImageSubresourceRange > &subresourceRanges) = 0;
	virtual void ClearColorImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderColorClearValue &clearValue, const RenderImageSubresourceRange &subresourceRange) = 0;
	virtual void ClearDepthStencilImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderDepthStencilClearValue &clearValue, const std::vector< RenderImageSubresourceRange > &subresourceRanges) = 0;
	virtual void ClearDepthStencilImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderDepthStencilClearValue &clearValue, const RenderImageSubresourceRange &subresourceRange) = 0;
	virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
	virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;
	virtual void DrawIndexedIndirect(std::shared_ptr< IRenderBuffer > buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) = 0;
	virtual void DrawIndirect(std::shared_ptr< IRenderBuffer > buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) = 0;
	virtual void EndRenderPass() = 0;

	// compute
	virtual void BindComputePipeline(std::shared_ptr< IRenderComputePipeline > pipeline) = 0;
	virtual void BindComputeDescriptorSet(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, std::shared_ptr< IRenderDescriptorSet > descriptorSet, uint32_t dynamicOffset) = 0;
	virtual void BindComputeDescriptorSets(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, const std::vector< std::shared_ptr< IRenderDescriptorSet > > &descriptorSets, const std::vector< uint32_t > &dynamicOffsets) = 0;
	virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;
	virtual void DispatchBase(uint32_t baseCountX, uint32_t baseCountY, uint32_t baseCountZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

	// barriers
	virtual void PipelineBarrier(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const std::vector< RenderBufferMemoryBarrier > &bufferMemoryBarriers, const std::vector< RenderImageMemoryBarrier > &imageMemoryBarriers) = 0;
	virtual void PipelineBarrier(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const std::vector< RenderMemoryBarrier > &memoryBarriers, const std::vector< RenderBufferMemoryBarrier > &bufferMemoryBarriers, const std::vector< RenderImageMemoryBarrier > &imageMemoryBarriers) = 0;
	virtual void PipelineBarrierMemory(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const RenderMemoryBarrier &memoryBarrier) = 0;
	virtual void PipelineBarrierBuffer(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const RenderBufferMemoryBarrier &bufferMemoryBarrier) = 0;
	virtual void PipelineBarrierImage(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const RenderImageMemoryBarrier &imageMemoryBarrier) = 0;

	// transfer/copy
	virtual void CopyBuffer(std::shared_ptr< IRenderBuffer > source, std::shared_ptr< IRenderBuffer > dest, const RenderBufferCopy &region) = 0;
	virtual void CopyBuffer(std::shared_ptr< IRenderBuffer > source, std::shared_ptr< IRenderBuffer > dest, const std::vector< RenderBufferCopy > &regions) = 0;
	virtual void CopyBufferToImage(std::shared_ptr< IRenderBuffer > sourceBuffer, std::shared_ptr< IRenderImage > destImage, EImageLayout destImageLayout, const RenderBufferImageCopy &region) = 0;
	virtual void CopyBufferToImage(std::shared_ptr< IRenderBuffer > sourceBuffer, std::shared_ptr< IRenderImage > destImage, EImageLayout destImageLayout, const std::vector< RenderBufferImageCopy > &regions) = 0;
	virtual void CopyImage(std::shared_ptr< IRenderImage > source, EImageLayout sourceImageLayout, std::shared_ptr< IRenderImage > dest, EImageLayout destImageLayout, const RenderImageCopy &region) = 0;
	virtual void CopyImage(std::shared_ptr< IRenderImage > source, EImageLayout sourceImageLayout, std::shared_ptr< IRenderImage > dest, EImageLayout destImageLayout, const std::vector< RenderImageCopy > &regions) = 0;
	virtual void CopyImageToBuffer(std::shared_ptr< IRenderImage > sourceImage, EImageLayout sourceImageLayout, std::shared_ptr< IRenderBuffer > destBuffer, const RenderBufferImageCopy &region) = 0;
	virtual void CopyImageToBuffer(std::shared_ptr< IRenderImage > sourceImage, EImageLayout sourceImageLayout, std::shared_ptr< IRenderBuffer > destBuffer, const std::vector< RenderBufferImageCopy > &regions) = 0;
};

class IRenderFence {
public:
	virtual ~IRenderFence() = default;

	virtual bool IsSignaled() = 0;

	virtual void Reset() = 0;
	virtual void Wait(uint64_t timeout) = 0;
};

class IRenderSemaphore {
public:
	virtual ~IRenderSemaphore() = default;
};

class IRenderSampler {
public:
	virtual ~IRenderSampler() = default;
};

class IRenderDescriptorSetLayout {
public:
	virtual ~IRenderDescriptorSetLayout() = default;
};

class IRenderDescriptorPool {
public:
	virtual ~IRenderDescriptorPool() = default;

	virtual std::shared_ptr< IRenderDescriptorSet > AllocateSingle(std::shared_ptr< IRenderDescriptorSetLayout > layout) = 0;
	virtual void AllocateBulk(std::vector< std::shared_ptr< IRenderDescriptorSet > > &descriptorSets, const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &layouts) = 0;
};

class IRenderDescriptorSet {
public:
	virtual ~IRenderDescriptorSet() = default;

	virtual void Copy(uint64_t sourceBinding, uint64_t sourceArrayElement, std::shared_ptr< IRenderDescriptorSet > destSet, uint64_t destBinding, uint64_t destArrayElement) = 0;
	virtual void Update(uint32_t binding, uint32_t arrayElement, EDescriptorType type, std::shared_ptr< IRenderBuffer > buffer, uint64_t offset, uint64_t range) = 0;
};

class IRenderPipelineLayout {
public:
	virtual ~IRenderPipelineLayout() = default;
};

class IRenderShaderModule {
public:
	virtual ~IRenderShaderModule() = default;

	virtual EShaderType GetType() = 0;
};

class IRenderComputePipeline {
public:
	virtual ~IRenderComputePipeline() = default;
};

class IRenderGraphicsPipeline {
public:
	struct {
		std::vector< RenderVertexInputBindingDescription > bindings;
		std::vector< RenderVertexInputAttributeDescription > attributes;
	} vertexInputState;

	struct {
		EPrimitiveType primitiveType;
		bool primitiveRestartEnable;
	} inputAssemblyState = {};

	struct {
		uint32_t patchControlPoints;
	} tessellationState = {};

	struct {
		std::vector< RenderViewport > viewports;
		std::vector< RenderRectangle > scissors;
	} viewportState;

	struct {
		bool depthClampEnable;
		bool rasterizerDiscardEnable;
		EFillMode fillMode;
		ECullMode cullMode;
		EFrontFace frontFace;
		bool depthBiasEnable;
		float depthBiasConstantFactor;
		float depthBiasClamp;
		float depthBiasSlopeFactor;
		float lineWidth;
	} rasterizationState = {};

	struct {
		bool depthTestEnable;
		bool depthWriteEnable;
		ETextureComparisonFunction compareMode;
		bool depthBoundsTestEnable;
		bool stencilTestEnable;
		RenderStencilOpState front;
		RenderStencilOpState back;
		float minDepthBounds;
		float maxDepthBounds;
	} depthStencilState = {};

	struct {
		bool logicOpEnable;
		ELogicOp logicOp;
		float blendConstants[4];
	} colorBlendState = {};

	std::vector< RenderColorBlendAttachmentState > colorBlendAttachments;

	virtual ~IRenderGraphicsPipeline() = default;

	virtual void Compile() = 0;
};

#endif
