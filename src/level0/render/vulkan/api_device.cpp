#include "level0/pch.h"

#include "api.h"

static const float queuePriority = 1.f;

VulkanDevice::VulkanDevice(VulkanPhysicalDevice *physicalDevice)
{
	this->phy = physicalDevice;

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->handle, &queueFamilyCount, nullptr);

	std::vector< VkQueueFamilyProperties > queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->handle, &queueFamilyCount, queueFamilies.data());

	auto findQueueFamilyIndex = [&](VkQueueFlags mask, VkQueueFlags flags) -> uint32_t {
		uint32_t i;

		for (i = 0; i < static_cast< uint32_t >(queueFamilies.size()); i++) {
			if ((queueFamilies[i].queueFlags & mask) == flags)
				return i;

		}

		return VK_QUEUE_FAMILY_IGNORED;
	};

	m_queueFamilyIndices[DEVICE_QUEUE_GRAPHICS] = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
	m_queueFamilyIndices[DEVICE_QUEUE_COMPUTE] = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, VK_QUEUE_COMPUTE_BIT);
	m_queueFamilyIndices[DEVICE_QUEUE_TRANSFER] = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, VK_QUEUE_TRANSFER_BIT);

	std::vector< VkDeviceQueueCreateInfo > queueCreateInfos;

	auto createQueueCreateInfo = [&](uint32_t queueFamilyIndex) {
		VkDeviceQueueCreateInfo createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.queueFamilyIndex = queueFamilyIndex;
		createInfo.queueCount = 1;
		createInfo.pQueuePriorities = &queuePriority;
		return createInfo;
	};

	auto checkQueue = [&](EDeviceQueue queue, EDeviceQueue subsitute) -> bool {
		if (m_queueFamilyIndices[queue] == VK_QUEUE_FAMILY_IGNORED) {
			m_queueFamilyIndices[queue] = m_queueFamilyIndices[subsitute];
			return false;
		} else {
			queueCreateInfos.push_back(createQueueCreateInfo(m_queueFamilyIndices[queue]));
			return true;
		}
	};

	if (!(m_supportsQueue[DEVICE_QUEUE_GRAPHICS] = checkQueue(DEVICE_QUEUE_GRAPHICS, MAX_DEVICE_QUEUE_ENUM)))
		throw std::runtime_error("VulkanDevice: No graphics queue family found!");

	if (!(m_supportsQueue[DEVICE_QUEUE_COMPUTE] = checkQueue(DEVICE_QUEUE_COMPUTE, DEVICE_QUEUE_GRAPHICS)))
		g_Log.Warn("VulkanDevice: No compute queue found");

	if (!(m_supportsQueue[DEVICE_QUEUE_TRANSFER] = checkQueue(DEVICE_QUEUE_TRANSFER, DEVICE_QUEUE_COMPUTE)))
		g_Log.Warn("VulkanDevice: No transfer queue found");

	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddress;
	bufferDeviceAddress.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	bufferDeviceAddress.pNext = nullptr;
	bufferDeviceAddress.bufferDeviceAddress = VK_TRUE;
	bufferDeviceAddress.bufferDeviceAddressCaptureReplay = VK_FALSE;
	bufferDeviceAddress.bufferDeviceAddressMultiDevice = VK_FALSE;

	VkPhysicalDeviceFeatures2 deviceFeatures;
	deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures.pNext = &bufferDeviceAddress;
	vkGetPhysicalDeviceFeatures2(physicalDevice->handle, &deviceFeatures);

	if (!deviceFeatures.features.samplerAnisotropy)
		throw std::runtime_error("VulkanDevice: Does not support Anisotropic Filtering!");

	static const std::array< const char *, 4 > extensionNames {
		"VK_KHR_bind_memory2",
		"VK_KHR_dedicated_allocation",
		"VK_KHR_get_memory_requirements2",
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	VkDeviceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = &deviceFeatures;
	createInfo.flags = 0;
	createInfo.queueCreateInfoCount = static_cast< uint32_t >(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
#ifndef NDEBUG
	createInfo.enabledLayerCount = static_cast< uint32_t >(g_VulkanLayerNames.size());
	createInfo.ppEnabledLayerNames = g_VulkanLayerNames.data();
#else
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
#endif
	createInfo.enabledExtensionCount = static_cast< uint32_t >(extensionNames.size());
	createInfo.ppEnabledExtensionNames = extensionNames.data();
	createInfo.pEnabledFeatures = nullptr;

	if (vkCreateDevice(physicalDevice->handle, &createInfo, nullptr, &this->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanDevice: Cannot create device handle!");

	volkLoadDeviceTable(&this->ftbl, this->handle);

	this->ftbl.vkGetDeviceQueue(this->handle, m_queueFamilyIndices[DEVICE_QUEUE_GRAPHICS], 0, &m_queues[DEVICE_QUEUE_GRAPHICS]);
	this->ftbl.vkGetDeviceQueue(this->handle, m_queueFamilyIndices[DEVICE_QUEUE_COMPUTE], 0, &m_queues[DEVICE_QUEUE_COMPUTE]);
	this->ftbl.vkGetDeviceQueue(this->handle, m_queueFamilyIndices[DEVICE_QUEUE_TRANSFER], 0, &m_queues[DEVICE_QUEUE_TRANSFER]);

	VmaVulkanFunctions vulkanFuncs;
	vulkanFuncs.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	vulkanFuncs.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
	vulkanFuncs.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
	vulkanFuncs.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
	vulkanFuncs.vkAllocateMemory = this->ftbl.vkAllocateMemory;
	vulkanFuncs.vkFreeMemory = this->ftbl.vkFreeMemory;
	vulkanFuncs.vkMapMemory = this->ftbl.vkMapMemory;
	vulkanFuncs.vkUnmapMemory = this->ftbl.vkUnmapMemory;
	vulkanFuncs.vkFlushMappedMemoryRanges = this->ftbl.vkFlushMappedMemoryRanges;
	vulkanFuncs.vkInvalidateMappedMemoryRanges = this->ftbl.vkInvalidateMappedMemoryRanges;
	vulkanFuncs.vkBindBufferMemory = this->ftbl.vkBindBufferMemory;
	vulkanFuncs.vkBindImageMemory = this->ftbl.vkBindImageMemory;
	vulkanFuncs.vkGetBufferMemoryRequirements = this->ftbl.vkGetBufferMemoryRequirements;
	vulkanFuncs.vkGetImageMemoryRequirements = this->ftbl.vkGetImageMemoryRequirements;
	vulkanFuncs.vkCreateBuffer = this->ftbl.vkCreateBuffer;
	vulkanFuncs.vkDestroyBuffer = this->ftbl.vkDestroyBuffer;
	vulkanFuncs.vkCreateImage = this->ftbl.vkCreateImage;
	vulkanFuncs.vkDestroyImage = this->ftbl.vkDestroyImage;
	vulkanFuncs.vkCmdCopyBuffer = this->ftbl.vkCmdCopyBuffer;
	vulkanFuncs.vkGetBufferMemoryRequirements2KHR = this->ftbl.vkGetBufferMemoryRequirements2KHR;
	vulkanFuncs.vkGetImageMemoryRequirements2KHR = this->ftbl.vkGetImageMemoryRequirements2KHR;
	vulkanFuncs.vkBindBufferMemory2KHR = this->ftbl.vkBindBufferMemory2KHR;
	vulkanFuncs.vkBindImageMemory2KHR = this->ftbl.vkBindImageMemory2KHR;
	vulkanFuncs.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;

	VmaAllocatorCreateInfo allocCreateInfo = {};
	allocCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	allocCreateInfo.vulkanApiVersion = VK_MAKE_VERSION(1, 2, 0);
	allocCreateInfo.physicalDevice = physicalDevice->handle;
	allocCreateInfo.device = this->handle;
	allocCreateInfo.instance = g_VulkanAPI->inst;
	allocCreateInfo.pVulkanFunctions = &vulkanFuncs;

	if (vmaCreateAllocator(&allocCreateInfo, &this->allocator) != VK_SUCCESS)
		throw std::runtime_error("VulkanDevice: Cannot create allocator!");
}

VulkanDevice::~VulkanDevice()
{
	if (this->allocator != VK_NULL_HANDLE) {
		vmaDestroyAllocator(this->allocator);
		this->allocator = VK_NULL_HANDLE;
	}

	if (this->handle != VK_NULL_HANDLE) {
		this->ftbl.vkDestroyDevice(this->handle, nullptr);
		this->handle = VK_NULL_HANDLE;
	}
}

void VulkanDevice::WaitIdle()
{
	this->ftbl.vkDeviceWaitIdle(this->handle);
}

void VulkanDevice::WaitIdleQueue(EDeviceQueue queue)
{
	this->ftbl.vkQueueWaitIdle(m_queues[queue]);
}

std::shared_ptr< IRenderAllocator > VulkanDevice::CreateAllocator(const std::string &name, EResourceMemoryUsage memoryUsage, uint64_t minAlign, uint64_t blockSize, size_t minBlockCount, size_t maxBlockCount)
{
	return std::dynamic_pointer_cast< IRenderAllocator >(std::make_shared< VulkanAllocator >(this, name, memoryUsage, minAlign, blockSize, minBlockCount, maxBlockCount));
}

std::shared_ptr< IRenderCommandPool > VulkanDevice::CreateCommandPool(EDeviceQueue queue, ECommandPoolUsage usage)
{
	return std::dynamic_pointer_cast< IRenderCommandPool >(std::make_shared< VulkanCommandPool >(this, queue, usage));
}

std::shared_ptr< IRenderDescriptorPool > VulkanDevice::CreateDescriptorPool(uint32_t maxSets, const std::vector< RenderDescriptorPoolSize > &poolSizes)
{
	return std::dynamic_pointer_cast< IRenderDescriptorPool >(std::make_shared< VulkanDescriptorPool >(this, maxSets, poolSizes));
}

std::shared_ptr< IRenderFence > VulkanDevice::CreateFence(bool signaled)
{
	return std::dynamic_pointer_cast< IRenderFence >(std::make_shared< VulkanFence >(this, signaled));
}

std::shared_ptr< IRenderSemaphore > VulkanDevice::CreateSemaphore()
{
	return std::dynamic_pointer_cast< IRenderSemaphore >(std::make_shared< VulkanSemaphore >(this));
}

std::shared_ptr< IRenderDescriptorSetLayout > VulkanDevice::CreateDescriptorSetLayout(const std::vector< RenderDescriptorSetLayoutBinding > &bindings)
{
	return std::dynamic_pointer_cast< IRenderDescriptorSetLayout >(std::make_shared< VulkanDescriptorSetLayout >(this, bindings));
}

std::shared_ptr< IRenderPipelineLayout > VulkanDevice::CreatePipelineLayout(const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &layouts)
{
	return std::dynamic_pointer_cast< IRenderPipelineLayout >(std::make_shared< VulkanPipelineLayout >(this, layouts));
}

std::shared_ptr< IRenderPipelineLayout > VulkanDevice::CreatePipelineLayout(const std::vector< std::shared_ptr< IRenderDescriptorSetLayout > > &layouts, const std::vector< RenderPushConstantRange > &ranges)
{
	return std::dynamic_pointer_cast< IRenderPipelineLayout >(std::make_shared< VulkanPipelineLayout >(this, layouts, ranges));
}

std::shared_ptr< IRenderPass > VulkanDevice::CreateRenderPass(const std::vector< RenderAttachmentDescription > &attachments, const std::vector< RenderSubpassDescription > &subpasses, const std::vector< RenderSubpassDependency > &subpassDependencies)
{
	return std::dynamic_pointer_cast< IRenderPass >(std::make_shared< VulkanRenderPass >(this, attachments, subpasses, subpassDependencies));
}

std::shared_ptr< IRenderFramebuffer > VulkanDevice::CreateFramebuffer(std::shared_ptr< IRenderPass > renderPass, const std::vector< std::shared_ptr< IRenderImage > > &images, const RenderExtent2D &extent)
{
	return std::dynamic_pointer_cast< IRenderFramebuffer >(std::make_shared< VulkanFramebuffer >(this, renderPass, images, extent));
}

std::shared_ptr< IRenderFramebuffer > VulkanDevice::CreateFramebuffer(std::shared_ptr< IRenderPass > renderPass, std::shared_ptr< IRenderImage > image, const RenderExtent2D &extent)
{
	return std::dynamic_pointer_cast< IRenderFramebuffer >(std::make_shared< VulkanFramebuffer >(this, renderPass, image, extent));
}

std::shared_ptr< IRenderSwapchain > VulkanDevice::CreateSwapchain(ESwapchainPresentMode presentMode, EDeviceQueue preferPresentQueue)
{
	return std::dynamic_pointer_cast< IRenderSwapchain >(std::make_shared< VulkanSwapchain >(this, presentMode, preferPresentQueue));
}

std::shared_ptr< IRenderSampler > VulkanDevice::CreateSampler(const RenderSamplerStateDescription &state)
{
	return std::dynamic_pointer_cast< IRenderSampler >(std::make_shared< VulkanSampler >(this, state));
}

std::shared_ptr< IRenderShaderModule > VulkanDevice::CreateShaderModule(EShaderType type, const void *blob, size_t blobSize)
{
	return std::dynamic_pointer_cast< IRenderShaderModule >(std::make_shared< VulkanShaderModule >(this, type, blob, blobSize));
}

std::shared_ptr< IRenderComputePipeline > VulkanDevice::CreateComputePipeline(std::shared_ptr< IRenderShaderModule > shaderModule, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderComputePipeline > basePipeline)
{
	return std::dynamic_pointer_cast< IRenderComputePipeline >(std::make_shared< VulkanComputePipeline >(this, shaderModule, pipelineLayout, basePipeline));
}

std::shared_ptr< IRenderGraphicsPipeline > VulkanDevice::CreateGraphicsPipeline(const std::vector< std::shared_ptr< IRenderShaderModule > > &shaderModules, std::shared_ptr< IRenderPipelineLayout > pipelineLayout, std::shared_ptr< IRenderGraphicsPipeline > basePipeline, std::shared_ptr< IRenderPass > renderPass, uint32_t subpass)
{
	return std::dynamic_pointer_cast< IRenderGraphicsPipeline >(std::make_shared< VulkanGraphicsPipeline >(this, shaderModules, pipelineLayout, basePipeline, renderPass, subpass));
}

void VulkanDevice::Submit(EDeviceQueue queue, std::shared_ptr< IRenderCommandBuffer > _commandBuffer, std::shared_ptr< IRenderSemaphore > _waitSemaphore, EPipelineStage waitPipelineStage, std::shared_ptr< IRenderSemaphore > _signalSemaphore, std::shared_ptr< IRenderFence > _fence)
{
	VkQueue queueObject = m_queues[queue];
	auto commandBuffer = std::dynamic_pointer_cast< VulkanCommandBuffer >(_commandBuffer);
	auto waitSemaphore = std::dynamic_pointer_cast< VulkanSemaphore >(_waitSemaphore);
	auto signalSemaphore = std::dynamic_pointer_cast< VulkanSemaphore >(_signalSemaphore);
	auto fence = std::dynamic_pointer_cast< VulkanFence >(_fence);

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = waitSemaphore != nullptr ? 1 : 0;
	submitInfo.pWaitSemaphores = waitSemaphore != nullptr ? &waitSemaphore->handle : nullptr;
	submitInfo.pWaitDstStageMask = reinterpret_cast< VkPipelineStageFlags * >(&waitPipelineStage);
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer->handle;
	submitInfo.signalSemaphoreCount = signalSemaphore != nullptr ? 1 : 0;
	submitInfo.pSignalSemaphores = signalSemaphore != nullptr ? &signalSemaphore->handle : nullptr;

	if (this->ftbl.vkQueueSubmit(queueObject, 1, &submitInfo, fence->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanDevice: Failed to submit command buffer!");
}

void VulkanDevice::Submit(EDeviceQueue queue, std::shared_ptr< IRenderCommandBuffer > _commandBuffer, const std::vector< std::shared_ptr< IRenderSemaphore > > &_waitSemaphores, const std::vector< EPipelineStage > &_waitPipelineStages, std::shared_ptr< IRenderSemaphore > _signalSemaphore, std::shared_ptr< IRenderFence > _fence)
{
	VkQueue queueObject = m_queues[queue];
	auto commandBuffer = std::dynamic_pointer_cast< VulkanCommandBuffer >(_commandBuffer);
	std::vector< VkSemaphore > waitSemaphores(_waitSemaphores.size());
	std::vector< VkPipelineStageFlags > waitPipelineStages(_waitPipelineStages.size());
	auto signalSemaphore = std::dynamic_pointer_cast< VulkanSemaphore >(_signalSemaphore);
	auto fence = std::dynamic_pointer_cast< VulkanFence >(_fence);

	for (size_t i = 0; i < _waitSemaphores.size(); i++) {
		waitSemaphores[i] = std::dynamic_pointer_cast< VulkanSemaphore >(_waitSemaphores[i])->handle;
		waitPipelineStages[i] = static_cast< VkPipelineStageFlags >(_waitPipelineStages[i]);
	}

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = static_cast< uint32_t >(waitSemaphores.size());
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.pWaitDstStageMask = waitPipelineStages.data();
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer->handle;
	submitInfo.signalSemaphoreCount = signalSemaphore != nullptr ? 1 : 0;
	submitInfo.pSignalSemaphores = signalSemaphore != nullptr ? &signalSemaphore->handle : nullptr;

	if (this->ftbl.vkQueueSubmit(queueObject, 1, &submitInfo, fence->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanDevice: Failed to submit command buffer!");
}

void VulkanDevice::Submit(EDeviceQueue queue, std::shared_ptr< IRenderCommandBuffer > _commandBuffer, const std::vector< std::shared_ptr< IRenderSemaphore > > &_waitSemaphores, const std::vector< EPipelineStage > &_waitPipelineStages, const std::vector< std::shared_ptr< IRenderSemaphore > > &_signalSemaphores, std::shared_ptr< IRenderFence > _fence)
{
	VkQueue queueObject = m_queues[queue];
	auto commandBuffer = std::dynamic_pointer_cast< VulkanCommandBuffer >(_commandBuffer);
	std::vector< VkSemaphore > waitSemaphores(_waitSemaphores.size()), signalSemaphores(_signalSemaphores.size());
	std::vector< VkPipelineStageFlags > waitPipelineStages(_waitPipelineStages.size());
	auto fence = std::dynamic_pointer_cast< VulkanFence >(_fence);

	for (size_t i = 0; i < _waitSemaphores.size(); i++) {
		waitSemaphores[i] = std::dynamic_pointer_cast< VulkanSemaphore >(_waitSemaphores[i])->handle;
		waitPipelineStages[i] = static_cast< VkPipelineStageFlags >(_waitPipelineStages[i]);
	}

	for (size_t i = 0; i < _signalSemaphores.size(); i++)
		signalSemaphores[i] = std::dynamic_pointer_cast< VulkanSemaphore >(_signalSemaphores[i])->handle;

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = static_cast< uint32_t >(waitSemaphores.size());
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.pWaitDstStageMask = waitPipelineStages.data();
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer->handle;
	submitInfo.signalSemaphoreCount = static_cast< uint32_t >(signalSemaphores.size());
	submitInfo.pSignalSemaphores = signalSemaphores.data();

	if (this->ftbl.vkQueueSubmit(queueObject, 1, &submitInfo, fence->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanDevice: Failed to submit command buffer!");
}

void VulkanDevice::Submit(EDeviceQueue queue, const std::vector< std::shared_ptr< IRenderCommandBuffer > > &_commandBuffers, std::shared_ptr< IRenderSemaphore > _waitSemaphore, EPipelineStage waitPipelineStage, std::shared_ptr< IRenderSemaphore > _signalSemaphore, std::shared_ptr< IRenderFence > _fence)
{
	VkQueue queueObject = m_queues[queue];
	std::vector< VkCommandBuffer > commandBuffers(_commandBuffers.size());
	auto waitSemaphore = std::dynamic_pointer_cast< VulkanSemaphore >(_waitSemaphore);
	auto signalSemaphore = std::dynamic_pointer_cast< VulkanSemaphore >(_signalSemaphore);
	auto fence = std::dynamic_pointer_cast< VulkanFence >(_fence);

	for (size_t i = 0; i < _commandBuffers.size(); i++)
		commandBuffers[i] = std::dynamic_pointer_cast< VulkanCommandBuffer >(_commandBuffers[i])->handle;

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = waitSemaphore != nullptr ? 1 : 0;
	submitInfo.pWaitSemaphores = waitSemaphore != nullptr ? &waitSemaphore->handle : nullptr;
	submitInfo.pWaitDstStageMask = reinterpret_cast< VkPipelineStageFlags * >(&waitPipelineStage);
	submitInfo.commandBufferCount = static_cast< uint32_t >(commandBuffers.size());
	submitInfo.pCommandBuffers = commandBuffers.data();
	submitInfo.signalSemaphoreCount = signalSemaphore != nullptr ? 1 : 0;
	submitInfo.pSignalSemaphores = signalSemaphore != nullptr ? &signalSemaphore->handle : nullptr;

	if (this->ftbl.vkQueueSubmit(queueObject, 1, &submitInfo, fence->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanDevice: Failed to submit command buffer!");
}

void VulkanDevice::Submit(EDeviceQueue queue, const std::vector< std::shared_ptr< IRenderCommandBuffer > > &_commandBuffers, const std::vector< std::shared_ptr< IRenderSemaphore > > &_waitSemaphores, const std::vector< EPipelineStage > &_waitPipelineStages, std::shared_ptr< IRenderSemaphore > _signalSemaphore, std::shared_ptr< IRenderFence > _fence)
{
	VkQueue queueObject = m_queues[queue];
	std::vector< VkCommandBuffer > commandBuffers(_commandBuffers.size());
	std::vector< VkSemaphore > waitSemaphores(_waitSemaphores.size());
	std::vector< VkPipelineStageFlags > waitPipelineStages(_waitPipelineStages.size());
	auto signalSemaphore = std::dynamic_pointer_cast< VulkanSemaphore >(_signalSemaphore);
	auto fence = std::dynamic_pointer_cast< VulkanFence >(_fence);

	for (size_t i = 0; i < _commandBuffers.size(); i++)
		commandBuffers[i] = std::dynamic_pointer_cast< VulkanCommandBuffer >(_commandBuffers[i])->handle;

	for (size_t i = 0; i < _waitSemaphores.size(); i++) {
		waitSemaphores[i] = std::dynamic_pointer_cast< VulkanSemaphore >(_waitSemaphores[i])->handle;
		waitPipelineStages[i] = static_cast< VkPipelineStageFlags >(_waitPipelineStages[i]);
	}

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = static_cast< uint32_t >(waitSemaphores.size());
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.pWaitDstStageMask = waitPipelineStages.data();
	submitInfo.commandBufferCount = static_cast< uint32_t >(commandBuffers.size());
	submitInfo.pCommandBuffers = commandBuffers.data();
	submitInfo.signalSemaphoreCount = signalSemaphore != nullptr ? 1 : 0;
	submitInfo.pSignalSemaphores = signalSemaphore != nullptr ? &signalSemaphore->handle : nullptr;

	if (this->ftbl.vkQueueSubmit(queueObject, 1, &submitInfo, fence->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanDevice: Failed to submit command buffer!");
}

void VulkanDevice::Submit(EDeviceQueue queue, const std::vector< std::shared_ptr< IRenderCommandBuffer > > &_commandBuffers, const std::vector< std::shared_ptr< IRenderSemaphore > > &_waitSemaphores, const std::vector< EPipelineStage > &_waitPipelineStages, const std::vector< std::shared_ptr< IRenderSemaphore > > &_signalSemaphores, std::shared_ptr< IRenderFence > _fence)
{
	VkQueue queueObject = m_queues[queue];
	std::vector< VkCommandBuffer > commandBuffers(_commandBuffers.size());
	std::vector< VkSemaphore > waitSemaphores(_waitSemaphores.size()), signalSemaphores(_signalSemaphores.size());
	std::vector< VkPipelineStageFlags > waitPipelineStages(_waitPipelineStages.size());
	auto fence = std::dynamic_pointer_cast< VulkanFence >(_fence);

	for (size_t i = 0; i < _commandBuffers.size(); i++)
		commandBuffers[i] = std::dynamic_pointer_cast< VulkanCommandBuffer >(_commandBuffers[i])->handle;

	for (size_t i = 0; i < _waitSemaphores.size(); i++) {
		waitSemaphores[i] = std::dynamic_pointer_cast< VulkanSemaphore >(_waitSemaphores[i])->handle;
		waitPipelineStages[i] = static_cast< VkPipelineStageFlags >(_waitPipelineStages[i]);
	}

	for (size_t i = 0; i < _signalSemaphores.size(); i++)
		signalSemaphores[i] = std::dynamic_pointer_cast< VulkanSemaphore >(_signalSemaphores[i])->handle;

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = static_cast< uint32_t >(waitSemaphores.size());
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.pWaitDstStageMask = waitPipelineStages.data();
	submitInfo.commandBufferCount = static_cast< uint32_t >(commandBuffers.size());
	submitInfo.pCommandBuffers = commandBuffers.data();
	submitInfo.signalSemaphoreCount = static_cast< uint32_t >(signalSemaphores.size());
	submitInfo.pSignalSemaphores = signalSemaphores.data();

	if (this->ftbl.vkQueueSubmit(queueObject, 1, &submitInfo, fence->handle) != VK_SUCCESS)
		throw std::runtime_error("VulkanDevice: Failed to submit command buffer!");
}
