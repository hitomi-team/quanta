#include "pch/pch.h"

#include "vulkanctx.h"

CVulkanCtx *g_vulkanCtx = nullptr;

#ifdef __DEBUG
static const char *layer_names[] = {
	"VK_LAYER_KHRONOS_validation"
};

// for debugging
static VkBool32 vkdbgcallback_fin() { return VK_FALSE; }

static VkBool32 vkdbgcallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_types,
	const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
	void *user_data)
{
	(void)message_severity;
	(void)message_types;

	global_log.Debug(FMT_STRING("[vulkan : ctx 0x{:016X}] {}"), reinterpret_cast< uintptr_t >(user_data), callback_data->pMessage);

	return vkdbgcallback_fin();
}
#endif

static uint32_t VK_FindQueueFamily(const std::vector< VkQueueFamilyProperties > &queueFamilies, VkQueueFlags flags, VkQueueFlags mask)
{
	for (uint32_t i = 0; i < queueFamilies.size(); i++) {
		if ((queueFamilies[i].queueFlags & mask) == flags)
			return i;
	}

	return VK_QUEUE_FAMILY_IGNORED;
}

static VkSurfaceFormatKHR VK_ChooseSurfaceFormat(const std::vector< VkSurfaceFormatKHR > &formats)
{
	for (const auto &i : formats) {
		// use something else (R8G8B8A8_UNORM) for Bi/Big endian architectures?
		if (i.format == VK_FORMAT_B8G8R8A8_UNORM && i.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return i;
	}

	return formats[0];
}

static VkPresentModeKHR VK_ChoosePresentMode(const std::vector< VkPresentModeKHR > &presentmodes, VkPresentModeKHR desired)
{
	for (const auto &i : presentmodes) {
		if (i == desired)
			return i;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D VK_ChooseExtent(const VkSurfaceCapabilitiesKHR &caps, SDL_Window *window)
{
	if (caps.currentExtent.width != UINT32_MAX) {
		return caps.currentExtent;
	} else {
		int width, height;

		SDL_Vulkan_GetDrawableSize(window, &width, &height);

		VkExtent2D extent = {
			static_cast< uint32_t >(width),
			static_cast< uint32_t >(height)
		};

		extent.width = std::max(caps.minImageExtent.width, std::min(caps.maxImageExtent.width, extent.width));
		extent.height = std::max(caps.minImageExtent.height, std::min(caps.maxImageExtent.height, extent.height));

		return extent;
	}
}

void CVulkanSwapchainDetails::Query(VkPhysicalDevice adapter, VkSurfaceKHR surface)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter, surface, &this->caps);

	uint32_t format_count, presentmode_count;

	vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, surface, &format_count, nullptr);
	if (format_count != 0) {
		this->formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, surface, &format_count, this->formats.data());
	}

	vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, surface, &presentmode_count, nullptr);
	if (presentmode_count != 0) {
		this->presentmodes.resize(presentmode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(adapter, surface, &presentmode_count, this->presentmodes.data());
	}
}

// create an instance and choose the first capable Vulkan adapter
bool CVulkanCtx::InitInstance()
{
	unsigned ext_count;
	if (SDL_Vulkan_GetInstanceExtensions(this->window, &ext_count, nullptr) == SDL_FALSE) {
		global_log.Error("cannot get Vulkan instance extensions from SDL2");
		return false;
	}

	std::vector< const char * > exts(ext_count);
	SDL_Vulkan_GetInstanceExtensions(this->window, &ext_count, exts.data());

#ifdef __DEBUG
	ext_count++;
	exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	volkInitializeCustom(reinterpret_cast< PFN_vkGetInstanceProcAddr >(SDL_Vulkan_GetVkGetInstanceProcAddr()));

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = APP_TITLE;
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pEngineName = "quanta";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.apiVersion = VK_API_VERSION_1_0;

#ifdef __DEBUG
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo.pNext = nullptr;
	debugCreateInfo.flags = 0;
	debugCreateInfo.messageSeverity = \
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | \
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugCreateInfo.messageType = \
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | \
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugCreateInfo.pfnUserCallback = vkdbgcallback;
	debugCreateInfo.pUserData = this;
#endif

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#ifdef __DEBUG
	createInfo.pNext = &debugCreateInfo;
#else
	createInfo.pNext = nullptr;
#endif
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
#ifdef __DEBUG
	createInfo.enabledLayerCount = 1;
	createInfo.ppEnabledLayerNames = layer_names;
#else
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
#endif
	createInfo.enabledExtensionCount = ext_count;
	createInfo.ppEnabledExtensionNames = exts.data();

#ifdef __DEBUG
	global_log.Debug("Using Vulkan validation layers");
#endif
	VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &this->instance), "Failed to create Vulkan instance");
	volkLoadInstance(this->instance);

#ifdef __DEBUG
	VK_ASSERT(vkCreateDebugUtilsMessengerEXT(this->instance, &debugCreateInfo, nullptr, &this->debug_messenger), "Failed to create Vulkan debug messenger");
#endif

	return true;
}

// create a logical device for the Vulkan adapter. choose the first adapter that comes up
// TODO: create a config to use which device?
bool CVulkanCtx::InitDevice()
{
	static const char *device_exts[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_MAINTENANCE1_EXTENSION_NAME
	};

	uint32_t num_adapters;
	vkEnumeratePhysicalDevices(this->instance, &num_adapters, nullptr);
	std::vector< VkPhysicalDevice > adapters(num_adapters);
	vkEnumeratePhysicalDevices(this->instance, &num_adapters, adapters.data());

	this->adapter = adapters.at(0);
	vkGetPhysicalDeviceProperties(this->adapter, &this->adapter_props);

	global_log.Info(FMT_STRING("Using VulkanDevice {:04X}:{:04X} \"{}\""), this->adapter_props.vendorID, this->adapter_props.deviceID, this->adapter_props.deviceName);

	uint32_t num_qfam;
	vkGetPhysicalDeviceQueueFamilyProperties(this->adapter, &num_qfam, nullptr);
	std::vector< VkQueueFamilyProperties > qfam(num_qfam);
	vkGetPhysicalDeviceQueueFamilyProperties(this->adapter, &num_qfam, qfam.data());

	uint32_t graphics_qfam = VK_FindQueueFamily(qfam, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
	uint32_t transfer_qfam = VK_FindQueueFamily(qfam, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, VK_QUEUE_TRANSFER_BIT);
	uint32_t compute_qfam = VK_FindQueueFamily(qfam, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, VK_QUEUE_COMPUTE_BIT);

	float priority = 1.0f;
	std::vector< VkDeviceQueueCreateInfo > queues;
	VkDeviceQueueCreateInfo qcreateInfo;
	qcreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	qcreateInfo.pNext = nullptr;
	qcreateInfo.flags = 0;
	qcreateInfo.queueCount = 1;
	qcreateInfo.pQueuePriorities = &priority;

	if (graphics_qfam == VK_QUEUE_FAMILY_IGNORED) {
		global_log.Error(FMT_STRING("VulkanDevice {:04X}:{:04X} \"{}\" - does not support graphics queue!"), this->adapter_props.vendorID, this->adapter_props.deviceID, this->adapter_props.deviceName);
		return false;
	} else {
		qcreateInfo.queueFamilyIndex = graphics_qfam;
		queues.push_back(qcreateInfo);
	}

	if (compute_qfam == VK_QUEUE_FAMILY_IGNORED) {
		compute_qfam = graphics_qfam;
	} else {
		qcreateInfo.queueFamilyIndex = compute_qfam;
		queues.push_back(qcreateInfo);
	}

	if (transfer_qfam == VK_QUEUE_FAMILY_IGNORED) {
		transfer_qfam = compute_qfam;
	} else {
		qcreateInfo.queueFamilyIndex = transfer_qfam;
		queues.push_back(qcreateInfo);
	}

	this->queue_family_indices[0] = graphics_qfam;
	this->queue_family_indices[1] = transfer_qfam;
	this->queue_family_indices[2] = compute_qfam;

	VkPhysicalDeviceFeatures features = {};
	VkDeviceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.queueCreateInfoCount = static_cast< uint32_t >(queues.size());
	createInfo.pQueueCreateInfos = queues.data();
#ifdef __DEBUG
	createInfo.enabledLayerCount = 1;
	createInfo.ppEnabledLayerNames = layer_names;
#else
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
#endif
	createInfo.enabledExtensionCount = 2;
	createInfo.ppEnabledExtensionNames = device_exts;
	createInfo.pEnabledFeatures = &features;

	VK_ASSERT(vkCreateDevice(this->adapter, &createInfo, nullptr, &this->device), "Failed to create VkDevice");
	this->DebugSetObjectName(this->device, VK_OBJECT_TYPE_DEVICE, this->adapter_props.deviceName);

#if 0
	VkQueue gq, tq, cq;
	vkGetDeviceQueue(this->device, this->queue_family_indices[0], 0, &gq);
	vkGetDeviceQueue(this->device, this->queue_family_indices[1], 0, &tq);
	vkGetDeviceQueue(this->device, this->queue_family_indices[2], 0, &cq);

	this->graphics_queue.Load(this->device, gq, this->queue_family_indices[0]);
	this->transfer_queue.Load(this->device, tq, this->queue_family_indices[1]);
	this->compute_queue.Load(this->device, cq, this->queue_family_indices[2]);
#endif

	vkGetDeviceQueue(this->device, this->queue_family_indices[0], 0, &this->queues[0]);
	vkGetDeviceQueue(this->device, this->queue_family_indices[1], 0, &this->queues[1]);
	vkGetDeviceQueue(this->device, this->queue_family_indices[2], 0, &this->queues[2]);

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;
	allocatorInfo.physicalDevice = this->adapter;
	allocatorInfo.device = this->device;
	allocatorInfo.instance = this->instance;

	vmaCreateAllocator(&allocatorInfo, &this->allocator);

	VkCommandPoolCreateInfo cmdCreateInfo;
	cmdCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdCreateInfo.pNext = nullptr;

	cmdCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	cmdCreateInfo.queueFamilyIndex = this->queue_family_indices[1];
	VK_ASSERT(vkCreateCommandPool(this->device, &cmdCreateInfo, nullptr, &this->transfer_command_pool), "Failed to create transfer VkCommandPool");
	this->DebugSetObjectName(this->transfer_command_pool, VK_OBJECT_TYPE_COMMAND_POOL, "transfer command pool");

	cmdCreateInfo.flags = 0;
	cmdCreateInfo.queueFamilyIndex = this->queue_family_indices[2];
	VK_ASSERT(vkCreateCommandPool(this->device, &cmdCreateInfo, nullptr, &this->compute_command_pool), "Failed to create compute VkCommandPool");
	this->DebugSetObjectName(this->compute_command_pool, VK_OBJECT_TYPE_COMMAND_POOL, "compute command pool");

	static const VkDescriptorPoolSize poolSizes[] = {
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo descCreateInfo;
	descCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descCreateInfo.pNext = nullptr;
	descCreateInfo.flags = 0;
	descCreateInfo.maxSets = 32;
	descCreateInfo.poolSizeCount = 11;
	descCreateInfo.pPoolSizes = poolSizes;

	static const VkDescriptorSetLayoutBinding bindings[] = {
		{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
		{ 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }
	};

	VkDescriptorSetLayoutCreateInfo setInfo;
	setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setInfo.pNext = nullptr;
	setInfo.flags = 0;
	setInfo.bindingCount = 2;
	setInfo.pBindings = bindings;

	VK_ASSERT(vkCreateDescriptorSetLayout(this->device, &setInfo, nullptr, &this->desc_layout), "Failed to create VkDescriptorSetLayout");
	VK_ASSERT(vkCreateDescriptorPool(this->device, &descCreateInfo, nullptr, &this->desc_pool), "Failed to create VkDescriptorPool");
	this->DebugSetObjectName(this->desc_pool, VK_OBJECT_TYPE_DESCRIPTOR_POOL, "g_vulkanCtx->desc_pool");

	VkPipelineLayoutCreateInfo pipelineInfo;
	pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineInfo.pNext = nullptr;
	pipelineInfo.flags = 0;
	pipelineInfo.setLayoutCount = 1;
	pipelineInfo.pSetLayouts = &this->desc_layout;
	pipelineInfo.pushConstantRangeCount = 0;
	pipelineInfo.pPushConstantRanges = nullptr;

	VK_ASSERT(vkCreatePipelineLayout(this->device, &pipelineInfo, nullptr, &this->pipeline_layout), "Failed to create VkPipelineLayout");

	return true;
}

bool CVulkanCtx::Init(SDL_Window *window)
{
	g_vulkanCtx = this;

	this->window = window;
	return this->InitInstance() && this->InitDevice() && this->InitSwapchain();
}

// create a swapchain
bool CVulkanCtx::InitSwapchain()
{
	if (SDL_Vulkan_CreateSurface(this->window, this->instance, &this->surface) == SDL_FALSE) {
		VK_ASSERT(0, "Failed to create SDL2 window vulkan surface");
		return false;
	}

	CVulkanSwapchainDetails details;
	details.Query(this->adapter, this->surface);

	VkSurfaceFormatKHR surfaceformat = VK_ChooseSurfaceFormat(details.formats);
	VkPresentModeKHR presentmode = VK_ChoosePresentMode(details.presentmodes, VK_PRESENT_MODE_IMMEDIATE_KHR);
	VkExtent2D extent = VK_ChooseExtent(details.caps, this->window);

	uint32_t imageCount = details.caps.minImageCount + 1;
	imageCount = details.caps.maxImageCount > 0 ? std::max(imageCount, details.caps.maxImageCount) : imageCount;

	for (auto &i : this->queue_family_indices) {
		VkBool32 present_support;
		vkGetPhysicalDeviceSurfaceSupportKHR(this->adapter, i, this->surface, &present_support);

		if (present_support) {
			this->present_queue = i;
			break;
		}
	}

	VkSwapchainCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.surface = this->surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceformat.format;
	createInfo.imageColorSpace = surfaceformat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t families[] = { this->queue_family_indices[0], this->present_queue };
	if (this->queue_family_indices[0] != this->present_queue) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = families;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 1;
		createInfo.pQueueFamilyIndices = &this->queue_family_indices[0];
	}

	createInfo.preTransform = details.caps.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentmode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VK_ASSERT(vkCreateSwapchainKHR(this->device, &createInfo, nullptr, &this->swapchain), "Failed to create VkSwapchain");
	this->DebugSetObjectName(this->swapchain, VK_OBJECT_TYPE_SWAPCHAIN_KHR, "swapchain");

	vkGetSwapchainImagesKHR(this->device, this->swapchain, &this->num_swapchain_images, nullptr);
	this->swapchain_images.resize(this->num_swapchain_images);
	vkGetSwapchainImagesKHR(this->device, this->swapchain, &this->num_swapchain_images, this->swapchain_images.data());

	this->swapchain_format = surfaceformat.format;
	this->swapchain_extent = extent;
	this->num_swapchain_images = imageCount;
	this->swapchain_imageviews.resize(this->num_swapchain_images);

	VkImageViewCreateInfo viewCreateInfo;
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.pNext = nullptr;
	viewCreateInfo.flags = 0;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = this->swapchain_format;
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;

	uint32_t i;
	for (i = 0; i < this->num_swapchain_images; i++) {
		viewCreateInfo.image = this->swapchain_images[i];
		VK_ASSERT(vkCreateImageView(this->device, &viewCreateInfo, nullptr, &this->swapchain_imageviews[i]), "Failed to create swapchain VkImageView");
	}

	VkFenceCreateInfo fenceCreateInfo;
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	this->swapchain_sync.resize(this->num_swapchain_images);
	for (i = 0; i < this->num_swapchain_images; i++) {
		VK_ASSERT(vkCreateFence(this->device, &fenceCreateInfo, nullptr, &this->swapchain_sync[i].fence), "Failed to create swapchain VkFence");
		VK_ASSERT(vkCreateSemaphore(this->device, &semaphoreCreateInfo, nullptr, &this->swapchain_sync[i].wait_sync), "Failed to create swapchain VkSemaphore wait");
		VK_ASSERT(vkCreateSemaphore(this->device, &semaphoreCreateInfo, nullptr, &this->swapchain_sync[i].wake_sync), "Failed to create swapchain VkSemaphore wake");
		this->DebugSetObjectName(this->swapchain_sync[i].fence, VK_OBJECT_TYPE_FENCE, "swapchain sync fence");
		this->DebugSetObjectName(this->swapchain_sync[i].wait_sync, VK_OBJECT_TYPE_SEMAPHORE, "swapchain sync semaphore wait");
		this->DebugSetObjectName(this->swapchain_sync[i].wake_sync, VK_OBJECT_TYPE_SEMAPHORE, "swapchain sync semaphore wake");
	}

	VkCommandPoolCreateInfo cmdCreateInfo;
	cmdCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdCreateInfo.pNext = nullptr;
	cmdCreateInfo.flags = 0;
	cmdCreateInfo.queueFamilyIndex = this->queue_family_indices[0];

	this->swapchain_command_pool.resize(this->num_swapchain_images);
	for (i = 0; i < this->num_swapchain_images; i++) {
		VK_ASSERT(vkCreateCommandPool(this->device, &cmdCreateInfo, nullptr, &this->swapchain_command_pool[i]), "Failed to create graphics VkCommandPool");
		this->DebugSetObjectName(this->swapchain_command_pool[i], VK_OBJECT_TYPE_COMMAND_POOL, "graphics command pool");
	}

	VkCommandBufferAllocateInfo commandAllocInfo;
	commandAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandAllocInfo.pNext = nullptr;
	commandAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandAllocInfo.commandBufferCount = 1;

	this->swapchain_command_bufs.resize(this->num_swapchain_images);
	for (i = 0; i < this->num_swapchain_images; i++) {
		commandAllocInfo.commandPool = this->swapchain_command_pool[i];
		VK_ASSERT(vkAllocateCommandBuffers(this->device, &commandAllocInfo, &this->swapchain_command_bufs[i]), "Failed to allocate swapchain VkCommandBuffer");
		this->DebugSetObjectName(this->swapchain_command_bufs[i], VK_OBJECT_TYPE_COMMAND_BUFFER, "swapchain command buffer");
	}

	VkAttachmentDescription colorAttachment;
	colorAttachment.flags = 0;
	colorAttachment.format = this->swapchain_format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef;
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass;
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = nullptr;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	VkSubpassDependency subpassDependency;
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassDependency.dependencyFlags = 0;

	VkRenderPassCreateInfo renderPassCreateInfo;
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext = nullptr;
	renderPassCreateInfo.flags = 0;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &subpassDependency;

	VK_ASSERT(vkCreateRenderPass(this->device, &renderPassCreateInfo, nullptr, &this->swapchain_renderpass), "Failed to create swapchain VkRenderPass");

	VkFramebufferCreateInfo framebufferCreateInfo;
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.pNext = nullptr;
	framebufferCreateInfo.flags = 0;
	framebufferCreateInfo.renderPass = this->swapchain_renderpass;
	framebufferCreateInfo.attachmentCount = 1;
	framebufferCreateInfo.width = this->swapchain_extent.width;
	framebufferCreateInfo.height = this->swapchain_extent.height;
	framebufferCreateInfo.layers = 1;

	this->swapchain_framebuffers.resize(this->num_swapchain_images);
	for (i = 0; i < this->num_swapchain_images; i++) {
		framebufferCreateInfo.pAttachments = &this->swapchain_imageviews[i];
		VK_ASSERT(vkCreateFramebuffer(this->device, &framebufferCreateInfo, nullptr, &this->swapchain_framebuffers[i]), "Failed to create swapchain VkFramebuffer");
	}

	return true;
}

void CVulkanCtx::CloseSwapchain()
{
	for (uint32_t i = 0; i < this->num_swapchain_images; i++) {
		vkFreeCommandBuffers(this->device, this->swapchain_command_pool[i], 1, &this->swapchain_command_bufs[i]);
		vkDestroyCommandPool(this->device, this->swapchain_command_pool[i], nullptr);
	}

	for (auto &i : this->swapchain_sync) {
		vkDestroyFence(this->device, i.fence, nullptr);
		vkDestroySemaphore(this->device, i.wait_sync, nullptr);
		vkDestroySemaphore(this->device, i.wake_sync, nullptr);
	}

	for (auto &i : this->swapchain_framebuffers)
		vkDestroyFramebuffer(this->device, i, nullptr);

	for (auto &i : this->swapchain_imageviews)
		vkDestroyImageView(this->device, i, nullptr);

	vkDestroyRenderPass(this->device, this->swapchain_renderpass, nullptr);
	vkDestroySwapchainKHR(this->device, this->swapchain, nullptr);
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);

	this->swapchain_images.clear();
	this->swapchain_imageviews.clear();
	this->swapchain_framebuffers.clear();
	this->swapchain_command_bufs.clear();
	this->swapchain_sync.clear();

	this->num_swapchain_images = 0;
	this->current_image = 0;
	this->acquire_image = 0;
	this->present_queue = VK_NULL_HANDLE;
}

void CVulkanCtx::Close()
{
	vkDestroyPipelineLayout(this->device, this->pipeline_layout, nullptr);

	vkDestroyDescriptorPool(this->device, this->desc_pool, nullptr);
	vkDestroyDescriptorSetLayout(this->device, this->desc_layout, nullptr);

	vkDestroyCommandPool(this->device, this->transfer_command_pool, nullptr);
	vkDestroyCommandPool(this->device, this->compute_command_pool, nullptr);

	this->CloseSwapchain();

	vmaDestroyAllocator(this->allocator);
	vkDestroyDevice(this->device, nullptr);
#ifdef __DEBUG
	vkDestroyDebugUtilsMessengerEXT(this->instance, this->debug_messenger, nullptr);
#endif
	vkDestroyInstance(this->instance, nullptr);
	this->ResetCache();

	g_vulkanCtx = nullptr;
}

VkCommandBuffer CVulkanCtx::BeginSingleTimeCommands()
{
	VkCommandBuffer command_buf;

	VkCommandBufferAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.commandPool = this->transfer_command_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VK_ASSERT(vkAllocateCommandBuffers(this->device, &allocInfo, &command_buf), "failed to allocate transfer command buffer");
	g_vulkanCtx->DebugSetObjectName(command_buf, VK_OBJECT_TYPE_COMMAND_BUFFER, "transfer command buffer");

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_buf, &beginInfo);

	return command_buf;
}

void CVulkanCtx::EndSingleTimeCommands(VkCommandBuffer command_buf)
{
	vkEndCommandBuffer(command_buf);

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	VkFence fence;

	VK_ASSERT(vkCreateFence(this->device, &fenceCreateInfo, nullptr, &fence), "failed to create transfer fence");
	this->DebugSetObjectName(fence, VK_OBJECT_TYPE_FENCE, "transfer fence");

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command_buf;

	vkQueueSubmit(this->queues[1], 1, &submitInfo, fence);
	vkWaitForFences(this->device, 1, &fence, VK_TRUE, UINT64_MAX);

	vkDestroyFence(this->device, fence, nullptr);
	vkFreeCommandBuffers(this->device, this->transfer_command_pool, 1, &command_buf);
}

#ifdef __DEBUG
void CVulkanCtx::DebugSetObjectName(void *obj, VkObjectType objtype, const char *objname)
{
	VkDebugUtilsObjectNameInfoEXT info;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	info.pNext = nullptr;
	info.objectType = objtype;
	info.objectHandle = reinterpret_cast< uint64_t >(obj);
	info.pObjectName = objname;
	vkSetDebugUtilsObjectNameEXT(this->device, &info);
}

void CVulkanCtx::DebugSetObjectTag(void *obj, VkObjectType objtype, uint64_t tagname, size_t tagsize, const void *tagdata)
{
	VkDebugUtilsObjectTagInfoEXT info;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_TAG_INFO_EXT;
	info.pNext = nullptr;
	info.objectType = objtype;
	info.objectHandle = reinterpret_cast< uint64_t >(obj);
	info.tagName = tagname;
	info.tagSize = tagsize;
	info.pTag = tagdata;
	vkSetDebugUtilsObjectTagEXT(this->device, &info);
}

void CVulkanCtx::DebugLabelBegin(VkCommandBuffer buffer, const char *labelname, float r, float g, float b)
{
	VkDebugUtilsLabelEXT info;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	info.pNext = nullptr;
	info.pLabelName = labelname;
	info.color[0] = r;
	info.color[1] = g;
	info.color[2] = b;
	info.color[3] = 1.0f;
	vkCmdBeginDebugUtilsLabelEXT(buffer, &info);
}

void CVulkanCtx::DebugLabelEnd(VkCommandBuffer buffer)
{
	vkCmdEndDebugUtilsLabelEXT(buffer);
}

void CVulkanCtx::DebugLabelInsert(VkCommandBuffer buffer, const char *labelname, float r, float g, float b)
{
	VkDebugUtilsLabelEXT info;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	info.pNext = nullptr;
	info.pLabelName = labelname;
	info.color[0] = r;
	info.color[1] = g;
	info.color[2] = b;
	info.color[3] = 1.0f;
	vkCmdInsertDebugUtilsLabelEXT(buffer, &info);
}
#endif
