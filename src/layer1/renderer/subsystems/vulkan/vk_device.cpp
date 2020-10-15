#include "pch/pch.h"

#include "vk_device.h"

namespace Renderer {

	static uint32_t GetQueueFamily(uint32_t starting_point, VkPhysicalDevice PhysicalDevice, VkQueueFlags mask, VkQueueFlags flags, uint32_t *QueueCount, bool required)
	{
		// Get Queue Family Indices
		uint32_t QueueFamilyIndex = UINT32_MAX;
		uint32_t QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> QueueFamilyProps(QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilyProps.data());

		*QueueCount = 0;

		uint32_t i;
		for (i = starting_point; i < QueueFamilyCount; i++) {
			if (QueueFamilyProps[i].queueCount > 0 && (QueueFamilyProps[i].queueFlags & mask) == flags) {				QueueFamilyIndex = i;
				*QueueCount = QueueFamilyProps[i].queueCount;
				break;
			}
		}

		VK_FATAL(QueueFamilyIndex == UINT32_MAX && required, "Failed to get queue family index");

		return QueueFamilyIndex;
	}

	std::vector<VkPhysicalDevice> QueryPhysicalDevices(VkInstance instance)
	{
		std::vector<VkPhysicalDevice> PhysicalDevices;

		uint32_t count;
		VK_ASSERT(vkEnumeratePhysicalDevices(instance, &count, nullptr), "Failed to query amount of physical devices")

		PhysicalDevices.resize(count);
		VK_ASSERT(vkEnumeratePhysicalDevices(instance, &count, PhysicalDevices.data()), "Failed to query physical devices")

		return PhysicalDevices;
	}

	std::vector<std::string> QueryPhysicalDeviceNames(VkInstance instance)
	{
		std::vector<VkPhysicalDevice> PhysicalDevices = QueryPhysicalDevices(instance);
		std::vector<std::string> PhysicalDeviceNames;

		for (VkPhysicalDevice device : PhysicalDevices) {
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(device, &props);
			PhysicalDeviceNames.emplace_back(std::string(props.deviceName));
		}

		return PhysicalDeviceNames;
	}

	VulkanDevice::VulkanDevice(VkInstance instance, VkPhysicalDevice PhysicalDevice)
	{
		Load(instance, PhysicalDevice);
	}

	void VulkanDevice::Load(VkInstance instance, VkPhysicalDevice PhysicalDevice)
	{
		(void)instance;
		this->PhysicalDevice = PhysicalDevice;

		vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProps);
		vendorname = PhysicalDeviceProps.deviceName;

		uint32_t graphicsQueueCount, computeQueueCount, transferQueueCount;

		// required
		this->QueueFamilyIndices[0] = GetQueueFamily(0, PhysicalDevice, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, &graphicsQueueCount, true);

		// ANV does not support any other queue except graphics, use graphics queue for all operations
		this->QueueFamilyIndices[2] = GetQueueFamily(0, PhysicalDevice, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, VK_QUEUE_COMPUTE_BIT, &computeQueueCount, false);
		if (this->QueueFamilyIndices[2] == UINT32_MAX) {
			global_log.Warn(FMT_STRING("VulkanDevice \"{}\" does not support a {} queue!"), this->vendorname, "compute");
			this->QueueFamilyIndices[2] = this->QueueFamilyIndices[0];
		}

		// RADV does not support a transfer queue. use compute bit in place
		this->QueueFamilyIndices[1] = GetQueueFamily(0, PhysicalDevice, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, VK_QUEUE_TRANSFER_BIT, &transferQueueCount, false);
		if (this->QueueFamilyIndices[1] == UINT32_MAX) {
			global_log.Warn(FMT_STRING("VulkanDevice \"{}\" does not support a {} queue!"), this->vendorname, "transfer");
			this->QueueFamilyIndices[1] = this->QueueFamilyIndices[2];
		}

		// Create Logical VulkanDevice
		std::vector< VkDeviceQueueCreateInfo > QueueCreateInfos;

		float graphics_priority = 1.0f, transfer_priority = 1.0f, compute_priority = 1.0f;
		VkDeviceQueueCreateInfo GraphQueueCreateInfo = {};
		GraphQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		GraphQueueCreateInfo.pNext = nullptr;
		GraphQueueCreateInfo.flags = 0;
		GraphQueueCreateInfo.queueFamilyIndex = QueueFamilyIndices[0];
		GraphQueueCreateInfo.queueCount = 1;
		GraphQueueCreateInfo.pQueuePriorities = &graphics_priority;
		QueueCreateInfos.push_back(GraphQueueCreateInfo);

		if (transferQueueCount > 0) {
			GraphQueueCreateInfo.queueFamilyIndex = QueueFamilyIndices[1];
			GraphQueueCreateInfo.pQueuePriorities = &transfer_priority;
			QueueCreateInfos.push_back(GraphQueueCreateInfo);
		}

		if (computeQueueCount > 0) {
			GraphQueueCreateInfo.queueFamilyIndex = QueueFamilyIndices[2];
			GraphQueueCreateInfo.pQueuePriorities = &compute_priority;
			QueueCreateInfos.push_back(GraphQueueCreateInfo);
		}

		std::vector<const char *> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkPhysicalDeviceFeatures PhysDevFeatures = {};
		VkDeviceCreateInfo DevCreateInfo = {};
		DevCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		DevCreateInfo.pNext = nullptr;
		DevCreateInfo.flags = 0;
		DevCreateInfo.queueCreateInfoCount = static_cast< uint32_t >(QueueCreateInfos.size());
		DevCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
		DevCreateInfo.enabledLayerCount = 0;
		DevCreateInfo.ppEnabledLayerNames = nullptr;
		DevCreateInfo.enabledExtensionCount = static_cast< uint32_t >(deviceExtensions.size());
		DevCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		DevCreateInfo.pEnabledFeatures = &PhysDevFeatures;

		VK_ASSERT(vkCreateDevice(PhysicalDevice, &DevCreateInfo, nullptr, &device), "Failed to create device")
		volkLoadDeviceTable(&this->fn, this->device);

		this->fn.vkGetDeviceQueue(device, QueueFamilyIndices[0], 0, &GraphicsQueue);
		if (QueueFamilyIndices[1] != QueueFamilyIndices[0])
			this->fn.vkGetDeviceQueue(device, QueueFamilyIndices[1], 0, &TransferQueue);

		if (QueueFamilyIndices[2] != QueueFamilyIndices[1])
			this->fn.vkGetDeviceQueue(device, QueueFamilyIndices[2], 0, &ComputeQueue);

		VkCommandPoolCreateInfo GraphicsPoolInfo = {};
		GraphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		GraphicsPoolInfo.queueFamilyIndex = QueueFamilyIndices[0];
		GraphicsPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VkCommandPoolCreateInfo TransferPoolInfo = {};
		TransferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		TransferPoolInfo.queueFamilyIndex = QueueFamilyIndices[1];
		TransferPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		VkCommandPoolCreateInfo ComputePoolInfo = {};
		ComputePoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		ComputePoolInfo.queueFamilyIndex = QueueFamilyIndices[2];
		ComputePoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VK_ASSERT(this->fn.vkCreateCommandPool(device, &TransferPoolInfo, nullptr, &TransferPool), "Failed to create Transfer Command Pool")
		VK_ASSERT(this->fn.vkCreateCommandPool(device, &GraphicsPoolInfo, nullptr, &GraphicsPool), "Failed to create Graphics Command Pool")
		VK_ASSERT(this->fn.vkCreateCommandPool(device,  &ComputePoolInfo, nullptr,  &ComputePool), "Failed to create Compute Command Pool")

		VmaAllocatorCreateInfo allocCreateInfo = {};
		allocCreateInfo.physicalDevice = PhysicalDevice;
		allocCreateInfo.device = this->device;
		allocCreateInfo.instance = instance;

		vmaCreateAllocator(&allocCreateInfo, &allocator);

		global_log.Debug(FMT_STRING("VulkanDevice \"{}\" available"), this->vendorname);

		freed = false;
	}

	uint32_t VulkanDevice::MemoryType(uint32_t Type, VkMemoryPropertyFlags Props)
	{
		VkPhysicalDeviceMemoryProperties MemProps;

		vkGetPhysicalDeviceMemoryProperties(this->PhysicalDevice, &MemProps);

		for (uint32_t i = 0; i < MemProps.memoryTypeCount; ++i) {
			if ((Type & (1 << i)) && ((MemProps.memoryTypes[i].propertyFlags & Props) == Props))
				return i;
		}

		return -1;
	}

	void VulkanDevice::Release()
	{
		if (!this->freed) {
			this->freed = true;

			vmaDestroyAllocator(allocator);
			this->fn.vkDestroyCommandPool(device, TransferPool, nullptr);
			this->fn.vkDestroyCommandPool(device, GraphicsPool, nullptr);
			this->fn.vkDestroyCommandPool(device, ComputePool, nullptr);
			this->fn.vkDestroyDevice(device, nullptr);
		}
	}

	std::vector<VulkanDevice> QueryAllDevices(VulkanInstance *vkinstance)
	{
		std::vector<Renderer::VulkanDevice> devices;

		if (!vkinstance->getLoaded())
			return devices;

		std::vector<VkPhysicalDevice> physdevs = QueryPhysicalDevices(vkinstance->get());

		for (uint32_t i = 0; i < physdevs.size(); i++) {
			devices.emplace_back(vkinstance->get(), physdevs[i], i);
		}

		return devices;
	}

	VulkanDevice QueryDevice(VulkanInstance *vkinstance)
	{
		std::vector<Renderer::VulkanDevice> devices;
		std::vector<VkPhysicalDevice> physdevs = QueryPhysicalDevices(vkinstance->get());

		return Renderer::VulkanDevice(vkinstance->get(), physdevs[0], 0);
	}

}
