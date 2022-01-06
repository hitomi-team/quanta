#ifndef LEVEL0_RENDERER_VULKAN_API_H
#define LEVEL0_RENDERER_VULKAN_API_H

#include "volk.h"
#include "vk_mem_alloc.h"
#include <SDL2/SDL_vulkan.h>

#include "level0/dependencies/imgui/imgui_impl_sdl.h"
#include "level0/dependencies/imgui/imgui_impl_vulkan.h"

#include "level0/render/api.h"

struct VulkanFormatAlphaProperties {
	VkFormat format;
	bool ignoreAlpha;
};

#ifndef NDEBUG
extern const std::array< const char *, 1 > g_VulkanLayerNames;
#endif
extern const std::array< VkAttachmentLoadOp, MAX_ATTACHMENT_LOAD_OP_ENUM > g_VulkanAttachmentLoadOps;
extern const std::array< VkAttachmentStoreOp, MAX_ATTACHMENT_STORE_OP_ENUM > g_VulkanAttachmentStoreOps;
extern const std::array< VkBlendFactor, MAX_BLEND_FACTORS > g_VulkanBlendFactors;
extern const std::array< VkBlendOp, MAX_BLEND_OPS > g_VulkanBlendOps;
extern const std::array< VkCommandBufferLevel, MAX_COMMAND_BUFFER_LEVEL_ENUM > g_VulkanCommandBufferLevels;
extern const std::array< VkCullModeFlags, MAX_CULLMODES > g_VulkanCullModes;
extern const std::array< VkDescriptorType, MAX_DESCRIPTOR_TYPE_ENUM > g_VulkanDescriptorTypes;
extern const std::array< VkFrontFace, MAX_FRONT_FACE_ENUM > g_VulkanFrontFaces;
extern const std::array< VkImageAspectFlags, MAX_IMAGE_ASPECT_ENUM > g_VulkanImageAspectFlags;
extern const std::array< VulkanFormatAlphaProperties, MAX_IMAGE_FORMAT_ENUM > g_VulkanImageFormats;
extern const std::array< VkImageLayout, MAX_IMAGE_LAYOUT_ENUM > g_VulkanImageLayouts;
extern const std::array< VkImageType, MAX_IMAGE_TYPE_ENUM > g_VulkanImageTypes;
extern const std::array< VkImageViewType, MAX_IMAGE_TYPE_ENUM > g_VulkanImageViewTypes;
extern const std::array< VkLogicOp, MAX_LOGIC_OP_ENUM > g_VulkanLogicOps;
extern const std::array< VkPolygonMode, MAX_FILL_MODE_ENUM > g_VulkanPolygonModes;
extern const std::array< VkPresentModeKHR, MAX_PRESENT_MODE_ENUM > g_VulkanPresentModes;
extern const std::array< VkPrimitiveTopology, MAX_PRIMITIVE_TYPE_ENUM > g_VulkanPrimitiveTypes;
extern const std::array< VkSubpassContents, MAX_SUBPASS_CONTENTS_ENUM > g_VulkanSubpassContents;
extern const std::array< VkSamplerAddressMode, MAX_ADDRESSMODES > g_VulkanSamplerAddressModes;
extern const std::array< VkCompareOp, MAX_TCF > g_VulkanSamplerCompareOps;
extern const std::array< VkFilter, MAX_FILTERMODES > g_VulkanSamplerFilterModes;
extern const std::array< VkSamplerMipmapMode, MAX_FILTERMODES > g_VulkanSamplerMipmapModes;
extern const std::array< VkShaderStageFlagBits, MAX_SHADER_TYPES > g_VulkanShaderTypes;
extern const std::array< VkStencilOp, MAX_STENCIL_OP_ENUM > g_VulkanStencilOps;
extern const std::array< VkVertexInputRate, MAX_VERTEX_INPUT_RATE_ENUM > g_VulkanVertexInputRates;

class VulkanAPI : public RenderAPI {
public:
	VkInstance inst = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	std::vector< const char * > requiredExtensions;

	VulkanAPI(const char *titleName);
	~VulkanAPI();

#ifndef NDEBUG
	void DebugUtilsMessenger(
		VkDebugUtilsMessageSeverityFlagBitsEXT,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT *
	);

private:
	VkDebugUtilsMessengerEXT m_debugUtilsMessenger = VK_NULL_HANDLE;
#endif
	SDL_Window *m_window = nullptr;
	friend class VulkanSwapchain;
	friend class VulkanImGui;
};

extern VulkanAPI *g_VulkanAPI;

class VulkanPhysicalDevice : public IRenderPhysicalDevice {
public:
	VkPhysicalDevice handle;
	VkPhysicalDeviceFeatures2 features;
	VkPhysicalDeviceMemoryProperties memProperties;
	RenderPhysicalDeviceInfo cachedInfo;

	std::string name;

	VulkanPhysicalDevice() = delete;
	VulkanPhysicalDevice(VkPhysicalDevice _handle);

	RenderPhysicalDeviceInfo GetInfo();

	std::shared_ptr< IRenderDevice > CreateLogicalDevice();
};

class VulkanDevice : public IRenderDevice {
public:
	VkDevice handle = VK_NULL_HANDLE;
	VulkanPhysicalDevice *phy = nullptr;
	VmaAllocator allocator = VK_NULL_HANDLE;
	VolkDeviceTable ftbl = {};

	VulkanDevice() = delete;
	VulkanDevice(VulkanPhysicalDevice *physicalDevice);

	~VulkanDevice();

	void WaitIdle();
	void WaitIdleQueue(EDeviceQueue queue);

	std::shared_ptr< IRenderAllocator > CreateAllocator(const std::string &name, EResourceMemoryUsage usage, uint64_t minAlign, uint64_t blockSize, size_t minBlockCount, size_t maxBlockCount);
	std::shared_ptr< IRenderCommandPool > CreateCommandPool(EDeviceQueue queue, ECommandPoolUsage usage);
	std::shared_ptr< IRenderDescriptorPool > CreateDescriptorPool(uint32_t maxSets, const std::vector< RenderDescriptorPoolSize > &poolSizes);
	std::shared_ptr< IRenderFence > CreateFence(bool signaled);
	std::shared_ptr< IRenderSemaphore > CreateSemaphore();

	std::shared_ptr< IRenderDescriptorSetLayout > CreateDescriptorSetLayout(const std::vector< RenderDescriptorSetLayoutBinding > &bindings);
	std::shared_ptr< IRenderPipelineLayout > CreatePipelineLayout(const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &layouts);
	std::shared_ptr< IRenderPipelineLayout > CreatePipelineLayout(const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &layouts, const std::vector< RenderPushConstantRange > &ranges);

	std::shared_ptr< IRenderPass > CreateRenderPass(const std::vector< RenderAttachmentDescription > &attachments, const std::vector< RenderSubpassDescription > &subpasses, const std::vector< RenderSubpassDependency > &subpassDependencies);
	std::shared_ptr< IRenderFramebuffer > CreateFramebuffer(std::shared_ptr< IRenderPass > renderPass, const std::vector< std::shared_ptr< IRenderImage > > &images, const RenderExtent2D &extent);
	std::shared_ptr< IRenderFramebuffer > CreateFramebuffer(std::shared_ptr< IRenderPass > renderPass, std::shared_ptr< IRenderImage > image, const RenderExtent2D &extent);
	std::shared_ptr< IRenderSwapchain > CreateSwapchain(ESwapchainPresentMode presentMode, EDeviceQueue preferPresentQueue);
	std::shared_ptr< IRenderImGui > CreateImGui(std::shared_ptr< IRenderPass > renderPass, uint32_t imageCount);

	std::shared_ptr< IRenderSampler > CreateSampler(const RenderSamplerStateDescription &state);

	std::shared_ptr< IRenderShaderModule > CreateShaderModule(EShaderType type, const void *blob, size_t blobSize);
	std::shared_ptr< IRenderComputePipeline > CreateComputePipeline(std::shared_ptr< IRenderShaderModule > shaderModule, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderComputePipeline > basePipeline);
	std::shared_ptr< IRenderGraphicsPipeline > CreateGraphicsPipeline(const std::vector< std::shared_ptr< IRenderShaderModule > > &shaderModules, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderGraphicsPipeline > basePipeline, std::shared_ptr< IRenderPass > renderPass, uint32_t subpass);

	void Submit(EDeviceQueue queue, std::shared_ptr< IRenderCommandBuffer > commandBuffer, std::shared_ptr< IRenderSemaphore > waitSemaphore, EPipelineStage waitPipelineStage, std::shared_ptr< IRenderSemaphore > signalSemaphore, std::shared_ptr< IRenderFence > fence);
	void Submit(EDeviceQueue queue, std::shared_ptr< IRenderCommandBuffer > commandBuffer, const std::vector< std::shared_ptr< IRenderSemaphore > > &waitSemaphores, const std::vector< EPipelineStage > &waitPipelineStages, std::shared_ptr< IRenderSemaphore > signalSemaphore, std::shared_ptr< IRenderFence > fence);
	void Submit(EDeviceQueue queue, std::shared_ptr< IRenderCommandBuffer > commandBuffer, const std::vector< std::shared_ptr< IRenderSemaphore > > &waitSemaphores, const std::vector< EPipelineStage > &waitPipelineStages, const std::vector< std::shared_ptr< IRenderSemaphore > > &signalSemaphores, std::shared_ptr< IRenderFence > fence);
	void Submit(EDeviceQueue queue, const std::vector< std::shared_ptr< IRenderCommandBuffer > > &commandBuffers, std::shared_ptr< IRenderSemaphore > waitSemaphore, EPipelineStage waitPipelineStage, std::shared_ptr< IRenderSemaphore > signalSemaphore, std::shared_ptr< IRenderFence > fence);
	void Submit(EDeviceQueue queue, const std::vector< std::shared_ptr< IRenderCommandBuffer > > &commandBuffers, const std::vector< std::shared_ptr< IRenderSemaphore > > &waitSemaphores, const std::vector< EPipelineStage > &waitPipelineStages, std::shared_ptr< IRenderSemaphore > signalSemaphore, std::shared_ptr< IRenderFence > fence);
	void Submit(EDeviceQueue queue, const std::vector< std::shared_ptr< IRenderCommandBuffer > > &commandBuffers, const std::vector< std::shared_ptr< IRenderSemaphore > > &waitSemaphores, const std::vector< EPipelineStage > &waitPipelineStages, const std::vector< std::shared_ptr< IRenderSemaphore > > &signalSemaphores, std::shared_ptr< IRenderFence > fence);
public:
	inline VkQueue GetQueue(EDeviceQueue queue) { return m_queues[queue]; }
	inline uint32_t GetQueueFamilyIndex(EDeviceQueue queue) { return m_queueFamilyIndices[queue]; }
	inline bool SupportsQueue(EDeviceQueue queue) { return m_supportsQueue[queue]; }
private:
	std::array< VkQueue, MAX_DEVICE_QUEUE_ENUM > m_queues {};
	std::array< uint32_t, MAX_DEVICE_QUEUE_ENUM > m_queueFamilyIndices {};
	std::array< bool, MAX_DEVICE_QUEUE_ENUM > m_supportsQueue {};
};

class VulkanAllocator : public IRenderAllocator {
public:
	VulkanDevice *device;
	VmaPool handle = VK_NULL_HANDLE;

	VulkanAllocator() = delete;
	VulkanAllocator(VulkanDevice *device, const std::string &name, EResourceMemoryUsage usage, uint64_t minAlign, uint64_t blockSize, size_t minBlockCount, size_t maxBlockCount);

	~VulkanAllocator();

	EResourceMemoryUsage GetResourceMemoryUsage();

	std::shared_ptr< IRenderBuffer > AllocateBuffer(EBufferUsage usage, uint64_t size);
	std::shared_ptr< IRenderImage > AllocateImage(EImageType type, EImageFormat format, EImageUsage usage, const RenderExtent3D &extent3d, const RenderImageSubresourceRange &subresourceRange);

	void Compactify();

#ifndef NDEBUG
	void DumpInfo(const std::string &filename);
#endif
private:
	EResourceMemoryUsage m_memoryUsage;
};

class VulkanBuffer : public IRenderBuffer {
public:
	VulkanDevice *device;
	VmaAllocation allocation;
	VkBuffer handle = VK_NULL_HANDLE;
	void *mem_ptr = nullptr;

	VulkanBuffer() = delete;
	VulkanBuffer(VulkanDevice *device, VmaPool pool, EResourceMemoryUsage memoryUsage, EBufferUsage usage, uint64_t size);

	~VulkanBuffer();

	void *Map();
	void Unmap();

	EResourceMemoryUsage GetResourceMemoryUsage();
	EBufferUsage GetUsage();
	uint64_t GetSize();
private:
	EResourceMemoryUsage m_memoryUsage;
	EBufferUsage m_usage;
	uint64_t m_size;
};

class VulkanImage : public IRenderImage {
public:
	VulkanDevice *device;
	VmaAllocation allocation;
	VkImage handle = VK_NULL_HANDLE;
	VkImageView imageView = VK_NULL_HANDLE;
	void *mem_ptr = nullptr;
	bool swapchainImage = false;

	VulkanImage() = delete;
	VulkanImage(VulkanDevice *device, VmaPool pool, EResourceMemoryUsage memoryUsage, EImageType type, EImageFormat format, EImageUsage usage, const RenderExtent3D &extent, const RenderImageSubresourceRange &subresourceRange);

	// for swapchain
	VulkanImage(VulkanDevice *device, VkImage image, EImageFormat format, const RenderExtent2D &extent, const RenderImageSubresourceRange &subresourceRange);

	~VulkanImage();

	void *Map();
	void Unmap();

	EResourceMemoryUsage GetResourceMemoryUsage();
	EImageUsage GetUsage();
	EImageFormat GetFormat();
	RenderExtent3D GetExtent();
	RenderImageSubresourceRange GetSubresourceRange();
private:
	EResourceMemoryUsage m_memoryUsage;
	EImageType m_type;
	EImageUsage m_usage;
	EImageFormat m_format;
	RenderExtent3D m_extent;
	RenderImageSubresourceRange m_subresourceRange;
};

class VulkanRenderPass : public IRenderPass {
public:
	VulkanDevice *device;
	VkRenderPass handle = VK_NULL_HANDLE;

	VulkanRenderPass() = delete;
	VulkanRenderPass(VulkanDevice *device, const std::vector< RenderAttachmentDescription > &attachments, const std::vector< RenderSubpassDescription > &subpasses, const std::vector< RenderSubpassDependency > &subpassDependencies);

	~VulkanRenderPass();
};

class VulkanFramebuffer : public IRenderFramebuffer {
public:
	VulkanDevice *device;
	std::vector< std::shared_ptr< VulkanImage > > images;
	std::shared_ptr< VulkanRenderPass > renderPass;
	VkFramebuffer handle = VK_NULL_HANDLE;

	VulkanFramebuffer() = delete;
	VulkanFramebuffer(VulkanDevice *device, std::shared_ptr< IRenderPass > renderPass, const std::vector< std::shared_ptr< IRenderImage > > &images, const RenderExtent2D &extent);
	VulkanFramebuffer(VulkanDevice *device, std::shared_ptr< IRenderPass > renderPass, std::shared_ptr< IRenderImage > image, const RenderExtent2D &extent);

	~VulkanFramebuffer();

	std::vector< std::shared_ptr< IRenderImage > > GetImages();
	std::shared_ptr< IRenderPass > GetRenderPass();
	RenderExtent2D GetExtent();
	bool IsMultiView();
private:
	RenderExtent2D m_extent;
};

class VulkanSwapchain : public IRenderSwapchain {
public:
	VulkanDevice *device;
	VkSwapchainKHR handle = VK_NULL_HANDLE;
	std::vector< std::shared_ptr< IRenderImage > > images;

	uint32_t numImages;

	VulkanSwapchain() = delete;
	VulkanSwapchain(VulkanDevice *device, ESwapchainPresentMode presentMode, EDeviceQueue preferPresentQueue);

	~VulkanSwapchain();

	ESwapchainResult GetAvailableImage(std::shared_ptr< IRenderSemaphore > semaphore, std::shared_ptr< IRenderFence > fence, uint64_t timeout, uint32_t &index);
	RenderExtent2D GetExtent();
	std::shared_ptr< IRenderImage > GetImage(uint32_t index);
	std::vector< std::shared_ptr< IRenderImage > > GetImages();
	uint32_t GetMaxImages();
	EDeviceQueue GetPresentingQueue();

	ESwapchainResult PresentImage(std::shared_ptr< IRenderSemaphore > waitSemaphore, uint32_t index);
	void Recreate(ESwapchainPresentMode presentMode, EDeviceQueue preferPresentQueue);
private:
	void Init(ESwapchainPresentMode presentMode, bool useOldSwapchain, EDeviceQueue preferPresentQueue);
private:
	RenderExtent2D m_extent;
	EDeviceQueue m_presentingQueue;
};

class VulkanCommandPool : public IRenderCommandPool {
public:
	VulkanDevice *device;
	VkCommandPool handle = VK_NULL_HANDLE;

	VulkanCommandPool() = delete;
	VulkanCommandPool(VulkanDevice *device, EDeviceQueue queue, ECommandPoolUsage usage);

	~VulkanCommandPool();

	std::shared_ptr< IRenderCommandBuffer > AllocateSingle(ECommandBufferLevel level);
	void AllocateBulk(std::vector< std::shared_ptr< IRenderCommandBuffer > > &commandBuffers, ECommandBufferLevel level);
};

class VulkanCommandBuffer : public IRenderCommandBuffer {
public:
	VulkanDevice *device;
	VulkanCommandPool *pool;
	VkCommandBuffer handle = VK_NULL_HANDLE;

	VulkanCommandBuffer(VulkanDevice *device, VulkanCommandPool *pool, ECommandBufferLevel level);
	~VulkanCommandBuffer();

	void Begin(ECommandBufferUsage usage);
	void End();

	void BindGraphicsPipeline(std::shared_ptr< IRenderGraphicsPipeline > pipeline);
	void BindGraphicsDescriptorSet(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, std::shared_ptr< IRenderDescriptorSet > descriptorSet, uint32_t dynamicOffset);
	void BindGraphicsDescriptorSets(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, const std::vector< std::shared_ptr< IRenderDescriptorSet > > &descriptorSets, const std::vector< uint32_t > &dynamicOffsets);
	void BindVertexBuffer(uint32_t firstBinding, std::shared_ptr< IRenderBuffer > buffer, uint64_t offset);
	void BindVertexBuffers(uint32_t firstBinding, const std::vector< std::shared_ptr< IRenderBuffer > > &buffers, const std::vector< uint64_t > &offsets);
	void BindIndexBufferU16(std::shared_ptr< IRenderBuffer > buffer, uint64_t offset);
	void BeginRenderPass(std::shared_ptr< IRenderPass > renderPass, std::shared_ptr< IRenderFramebuffer > framebuffer, RenderRectangle renderArea, const std::vector< RenderClearValue > &clearValues, ESubpassContents subpassContents);
	void ClearAttachment(const RenderClearAttachment &attachment, const RenderClearRectangle &rectangle);
	void ClearAttachments(const std::vector< RenderClearAttachment > &attachments, const std::vector< RenderClearRectangle > &rectangles);
	void ClearColorImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderColorClearValue &clearValue, const std::vector< RenderImageSubresourceRange > &subresourceRanges);
	void ClearColorImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderColorClearValue &clearValue, const RenderImageSubresourceRange &subresourceRange);
	void ClearDepthStencilImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderDepthStencilClearValue &clearValue, const std::vector< RenderImageSubresourceRange > &subresourceRanges);
	void ClearDepthStencilImage(std::shared_ptr< IRenderImage > image, EImageLayout imageLayout, const RenderDepthStencilClearValue &clearValue, const RenderImageSubresourceRange &subresourceRange);
	void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
	void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
	void DrawIndexedIndirect(std::shared_ptr< IRenderBuffer > buffer, uint64_t offset, uint32_t drawCount, uint32_t stride);
	void DrawIndirect(std::shared_ptr< IRenderBuffer > buffer, uint64_t offset, uint32_t drawCount, uint32_t stride);
	void EndRenderPass();

	void BindComputePipeline(std::shared_ptr< IRenderComputePipeline > pipeline);
	void BindComputeDescriptorSet(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, std::shared_ptr< IRenderDescriptorSet > descriptorSet, uint32_t dynamicOffset);
	void BindComputeDescriptorSets(std::shared_ptr< IRenderPipelineLayout > layout, uint32_t firstSet, const std::vector< std::shared_ptr< IRenderDescriptorSet > > &descriptorSets, const std::vector< uint32_t > &dynamicOffsets);
	void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
	void DispatchBase(uint32_t baseCountX, uint32_t baseCountY, uint32_t baseCountZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

	void PipelineBarrier(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const std::vector< RenderBufferMemoryBarrier > &bufferMemoryBarriers, const std::vector< RenderImageMemoryBarrier > &imageMemoryBarriers);
	void PipelineBarrier(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const std::vector< RenderMemoryBarrier > &memoryBarriers, const std::vector< RenderBufferMemoryBarrier > &bufferMemoryBarriers, const std::vector< RenderImageMemoryBarrier > &imageMemoryBarriers);
	void PipelineBarrierMemory(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const RenderMemoryBarrier &memoryBarrier);
	void PipelineBarrierBuffer(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const RenderBufferMemoryBarrier &bufferMemoryBarrier);
	void PipelineBarrierImage(EPipelineStage sourceStage, EPipelineStage destStage, EDependencyFlag dependency, const RenderImageMemoryBarrier &imageMemoryBarrier);


	void CopyBuffer(std::shared_ptr< IRenderBuffer > source, std::shared_ptr< IRenderBuffer > dest, const RenderBufferCopy &region);
	void CopyBuffer(std::shared_ptr< IRenderBuffer > source, std::shared_ptr< IRenderBuffer > dest, const std::vector< RenderBufferCopy > &regions);
	void CopyBufferToImage(std::shared_ptr< IRenderBuffer > sourceBuffer, std::shared_ptr< IRenderImage > destImage, EImageLayout destImageLayout, const RenderBufferImageCopy &region);
	void CopyBufferToImage(std::shared_ptr< IRenderBuffer > sourceBuffer, std::shared_ptr< IRenderImage > destImage, EImageLayout destImageLayout, const std::vector< RenderBufferImageCopy > &regions);
	void CopyImage(std::shared_ptr< IRenderImage > source, EImageLayout sourceImageLayout, std::shared_ptr< IRenderImage > dest, EImageLayout destImageLayout, const RenderImageCopy &region);
	void CopyImage(std::shared_ptr< IRenderImage > source, EImageLayout sourceImageLayout, std::shared_ptr< IRenderImage > dest, EImageLayout destImageLayout, const std::vector< RenderImageCopy > &regions);
	void CopyImageToBuffer(std::shared_ptr< IRenderImage > sourceImage, EImageLayout sourceImageLayout, std::shared_ptr< IRenderBuffer > destBuffer, const RenderBufferImageCopy &region);
	void CopyImageToBuffer(std::shared_ptr< IRenderImage > sourceImage, EImageLayout sourceImageLayout, std::shared_ptr< IRenderBuffer > destBuffer, const std::vector< RenderBufferImageCopy > &regions);
};

class VulkanFence : public IRenderFence {
public:
	VulkanDevice *device;
	VkFence handle = VK_NULL_HANDLE;

	VulkanFence() = delete;
	VulkanFence(VulkanDevice *device, bool signaled);

	~VulkanFence();

	bool IsSignaled();

	void Reset();
	void Wait(uint64_t timeout);
};

class VulkanSemaphore : public IRenderSemaphore {
public:
	VulkanDevice *device;
	VkSemaphore handle = VK_NULL_HANDLE;

	VulkanSemaphore() = delete;
	VulkanSemaphore(VulkanDevice *device);

	~VulkanSemaphore();
};

class VulkanSampler : public IRenderSampler {
public:
	VulkanDevice *device;
	VkSampler handle = VK_NULL_HANDLE;

	VulkanSampler() = delete;
	VulkanSampler(VulkanDevice *device, const RenderSamplerStateDescription &state);

	~VulkanSampler();
};

class VulkanDescriptorSetLayout : public IRenderDescriptorSetLayout {
public:
	VulkanDevice *device;
	VkDescriptorSetLayout handle = VK_NULL_HANDLE;

	VulkanDescriptorSetLayout() = delete;
	VulkanDescriptorSetLayout(VulkanDevice *device, const std::vector< RenderDescriptorSetLayoutBinding > &bindings);

	~VulkanDescriptorSetLayout();
};

class VulkanDescriptorPool : public IRenderDescriptorPool {
public:
	VulkanDevice *device;
	VkDescriptorPool handle = VK_NULL_HANDLE;

	VulkanDescriptorPool() = delete;
	VulkanDescriptorPool(VulkanDevice *device, uint32_t maxSets, const std::vector< RenderDescriptorPoolSize > &poolSizes);

	~VulkanDescriptorPool();

	std::shared_ptr< IRenderDescriptorSet > AllocateSingle(std::shared_ptr< IRenderDescriptorSetLayout > layout);
	void AllocateBulk(std::vector< std::shared_ptr< IRenderDescriptorSet > > &descriptorSets, const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &layouts);
};

class VulkanDescriptorSet : public IRenderDescriptorSet {
public:
	VulkanDevice *device;
	VulkanDescriptorPool *pool;
	VkDescriptorSet handle = VK_NULL_HANDLE;

	VulkanDescriptorSet() = delete;
	VulkanDescriptorSet(VulkanDevice *device, VulkanDescriptorPool *pool, std::shared_ptr< IRenderDescriptorSetLayout > layout);

	~VulkanDescriptorSet();

	void Copy(uint64_t sourceBinding, uint64_t sourceArrayElement, std::shared_ptr< IRenderDescriptorSet > destSet, uint64_t destBinding, uint64_t destArrayElement);
	void Update(uint32_t binding, uint32_t arrayElement, EDescriptorType type, std::shared_ptr< IRenderBuffer > buffer, uint64_t offset, uint64_t range);
};

class VulkanImGui : public IRenderImGui {
public:
	VulkanDevice *device;

	VulkanImGui() = delete;
	VulkanImGui(VulkanDevice *device, std::shared_ptr< IRenderPass > renderPass, uint32_t imageCount);
	~VulkanImGui();

	void NewFrame();
	void Draw(std::shared_ptr< IRenderFramebuffer > framebuffer, uint32_t imageIndex);
	std::shared_ptr< IRenderCommandBuffer > GetCommandBuffer(uint32_t imageIndex);
	std::shared_ptr< IRenderSemaphore > GetSemaphore(uint32_t imageIndex);
private:
	ImGuiContext *m_context;
	VkDescriptorPool m_descriptorPool;
	std::shared_ptr< VulkanRenderPass > m_renderPass;
	std::vector< std::shared_ptr< VulkanCommandPool > > m_commandPools;
	std::vector< std::shared_ptr< VulkanCommandBuffer > > m_commandBuffers;
	std::vector< std::shared_ptr< IRenderSemaphore > > m_semaphores;
	uint32_t m_imageCount;
};

class VulkanPipelineLayout : public IRenderPipelineLayout {
public:
	VulkanDevice *device;
	VkPipelineLayout handle = VK_NULL_HANDLE;

	VulkanPipelineLayout() = delete;
	VulkanPipelineLayout(VulkanDevice *device, const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &layouts);
	VulkanPipelineLayout(VulkanDevice *device, const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &layouts, const std::vector< RenderPushConstantRange > &ranges);

	~VulkanPipelineLayout();
};

class VulkanShaderModule : public IRenderShaderModule {
public:
	VulkanDevice *device;
	VkShaderModule handle = VK_NULL_HANDLE;

	VulkanShaderModule() = delete;

	VulkanShaderModule(VulkanDevice *device, EShaderType type, const void *blob, size_t blobSize);
	~VulkanShaderModule();

	EShaderType GetType();
private:
	EShaderType m_type;
};

class VulkanComputePipeline : public IRenderComputePipeline {
public:
	VulkanDevice *device;
	VkPipeline handle = VK_NULL_HANDLE;

	VulkanComputePipeline() = delete;
	VulkanComputePipeline(VulkanDevice *device, std::shared_ptr< IRenderShaderModule > shaderModule, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderComputePipeline > basePipeline);
	~VulkanComputePipeline();
};

// compute the hash for all states
struct VulkanGraphicsPipelineHash {
	uint8_t checksum[16]; // MD5, the pipeline hashmap does not require cryptography
	VkPipeline pipeline;
};

class VulkanGraphicsPipeline : public IRenderGraphicsPipeline {
public:
	VulkanDevice *device;
	VkPipeline handle = VK_NULL_HANDLE;

	VulkanGraphicsPipeline() = delete;
	VulkanGraphicsPipeline(VulkanDevice *device, const std::vector< std::shared_ptr< IRenderShaderModule > > &shaders, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderGraphicsPipeline > basePipeline, std::shared_ptr< IRenderPass > renderPass, uint32_t subpass);

	~VulkanGraphicsPipeline();

	void Compile();
private:
	std::vector< std::shared_ptr< VulkanShaderModule > > m_shaderModules;
	std::vector< VulkanGraphicsPipelineHash > m_hashmap;
	std::shared_ptr< VulkanPipelineLayout > m_pipelineLayout;
	std::shared_ptr< VulkanGraphicsPipeline > m_basePipeline;
	std::shared_ptr< VulkanRenderPass > m_renderPass;
	uint32_t m_subpass;
};

#endif
