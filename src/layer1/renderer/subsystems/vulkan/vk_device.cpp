#include "pch/pch.h"

#include "vk_device.h"

namespace Renderer {

	static uint32_t GetQueueFamily(uint32_t starting_point, VkPhysicalDevice PhysicalDevice, int flags, uint32_t *QueueCount)
	{
		// Get Queue Family Indices
		uint32_t QueueFamilyIndex = 0;
		uint32_t QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> QueueFamilyProps(QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilyProps.data());

		uint32_t i;
		for (i = starting_point; i < QueueFamilyCount; i++) {
			if (QueueFamilyProps[i].queueCount > 0 && (QueueFamilyProps[i].queueFlags & flags)) {
				QueueFamilyIndex = i;
				*QueueCount = QueueFamilyProps[i].queueCount;
				break;
			}
		}

		if (QueueFamilyIndex == QueueFamilyCount) {
			fprintf(stderr, "Failed to get Queue Family Index");
			abort();
		}

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

	VulkanDevice::VulkanDevice(const VulkanDevice &dev)
	{
		Load(dev);
	}

	void VulkanDevice::Load(const VulkanDevice &dev)
	{
		this->PhysicalDevice = dev.PhysicalDevice;
		this->PhysicalDeviceProps = dev.PhysicalDeviceProps;
		this->device = dev.device;
		this->GraphicsQueue = dev.GraphicsQueue;
		this->TransferQueue = dev.TransferQueue;
		this->TransferPool = dev.TransferPool;
		this->GraphicsPool = dev.GraphicsPool;
		this->ComputePool = dev.ComputePool;
		this->QueueFamilyIndices[0] = dev.QueueFamilyIndices[0];
		this->QueueFamilyIndices[1] = dev.QueueFamilyIndices[1];
		this->id = dev.id;
		this->vendorname = dev.vendorname;
		this->freed = true;
	}

	void VulkanDevice::Load(VkInstance instance, VkPhysicalDevice PhysicalDevice)
	{
		(void)instance;
		this->PhysicalDevice = PhysicalDevice;

		vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProps);
		vendorname = PhysicalDeviceProps.deviceName;

		uint32_t graphicsQueueCount, transferQueueCount;
		this->QueueFamilyIndices[0] = GetQueueFamily(0, PhysicalDevice, VK_QUEUE_GRAPHICS_BIT, &graphicsQueueCount);
		this->QueueFamilyIndices[1] = GetQueueFamily(QueueFamilyIndices[0] + 1, PhysicalDevice, ~VK_QUEUE_GRAPHICS_BIT & VK_QUEUE_TRANSFER_BIT, &transferQueueCount);

		// Create Logical VulkanDevice
		float GraphQueuePriorities = 1.0f;
		VkDeviceQueueCreateInfo GraphQueueCreateInfo = {};
		GraphQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		GraphQueueCreateInfo.pNext = nullptr;
		GraphQueueCreateInfo.flags = 0;
		GraphQueueCreateInfo.queueFamilyIndex = QueueFamilyIndices[0];
		GraphQueueCreateInfo.queueCount = 1;
		GraphQueueCreateInfo.pQueuePriorities = &GraphQueuePriorities;

		float TransQueuePriorities = 1.0f;
		VkDeviceQueueCreateInfo TransQueueCreateInfo = {};
		TransQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		TransQueueCreateInfo.pNext = nullptr;
		TransQueueCreateInfo.flags = 0;
		TransQueueCreateInfo.queueFamilyIndex = QueueFamilyIndices[1];
		TransQueueCreateInfo.queueCount = 1;
		TransQueueCreateInfo.pQueuePriorities = &TransQueuePriorities;

		VkDeviceQueueCreateInfo QueueCreateInfos[2];
		QueueCreateInfos[0] = GraphQueueCreateInfo;
		QueueCreateInfos[1] = TransQueueCreateInfo;

		std::vector<const char *> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkPhysicalDeviceFeatures PhysDevFeatures = {};
		VkDeviceCreateInfo DevCreateInfo = {};
		DevCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		DevCreateInfo.pNext = nullptr;
		DevCreateInfo.flags = 0;
		DevCreateInfo.queueCreateInfoCount = 2;
		DevCreateInfo.pQueueCreateInfos = QueueCreateInfos;
		DevCreateInfo.enabledLayerCount = 0;
		DevCreateInfo.ppEnabledLayerNames = nullptr;
		DevCreateInfo.enabledExtensionCount = deviceExtensions.size();
		DevCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		DevCreateInfo.pEnabledFeatures = &PhysDevFeatures;

		VK_ASSERT(vkCreateDevice(PhysicalDevice, &DevCreateInfo, nullptr, &device), "Failed to create device")

		vkGetDeviceQueue(device, QueueFamilyIndices[0], 0, &GraphicsQueue);
		vkGetDeviceQueue(device, QueueFamilyIndices[1], 0, &TransferQueue);

		VkCommandPoolCreateInfo TransferPoolInfo = {};
		TransferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		TransferPoolInfo.queueFamilyIndex = QueueFamilyIndices[1];
		TransferPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		VkCommandPoolCreateInfo GraphicsPoolInfo = {};
		GraphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		GraphicsPoolInfo.queueFamilyIndex = QueueFamilyIndices[0];
		GraphicsPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VkCommandPoolCreateInfo ComputePoolInfo = {};
		ComputePoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		ComputePoolInfo.queueFamilyIndex = QueueFamilyIndices[1];
		ComputePoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VK_ASSERT(vkCreateCommandPool(device, &TransferPoolInfo, nullptr, &TransferPool), "Failed to create Transfer Command Pool")
		VK_ASSERT(vkCreateCommandPool(device, &GraphicsPoolInfo, nullptr, &GraphicsPool), "Failed to create Graphics Command Pool")
		VK_ASSERT(vkCreateCommandPool(device,  &ComputePoolInfo, nullptr,  &ComputePool), "Failed to create Compute Command Pool")

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
			vkDestroyCommandPool(device, TransferPool, nullptr);
			vkDestroyCommandPool(device, GraphicsPool, nullptr);
			vkDestroyCommandPool(device, ComputePool, nullptr);
			vkDestroyDevice(device, nullptr);
		}
	}

	VulkanDevice &VulkanDevice::operator= (const VulkanDevice &devb)
	{
		if (&devb != this) {
			this->PhysicalDevice = devb.PhysicalDevice;
			this->PhysicalDeviceProps = devb.PhysicalDeviceProps;
			this->device = devb.device;
			this->GraphicsQueue = devb.GraphicsQueue;
			this->TransferQueue = devb.TransferQueue;
			this->TransferPool = devb.TransferPool;
			this->GraphicsPool = devb.GraphicsPool;
			this->ComputePool = devb.ComputePool;
			this->QueueFamilyIndices[0] = devb.QueueFamilyIndices[0];
			this->QueueFamilyIndices[1] = devb.QueueFamilyIndices[1];
			this->freed = devb.freed;
		}

		return *this;
	}

	std::vector<VulkanDevice> QueryAllDevices(VulkanInstance vkinstance)
	{
		std::vector<Renderer::VulkanDevice> devices;

		if (!vkinstance.getLoaded())
			return devices;

		std::vector<VkPhysicalDevice> physdevs = QueryPhysicalDevices(vkinstance.get());

		for (uint32_t i = 0; i < physdevs.size(); i++) {
			Renderer::VulkanDevice device(vkinstance.get(), physdevs[i]);
			device.setId(i);
			devices.push_back(device);
		}

		return devices;
	}

	VulkanDevice QueryDevice(VulkanInstance vkinstance)
	{
		std::vector<Renderer::VulkanDevice> devices;
		std::vector<VkPhysicalDevice> physdevs = QueryPhysicalDevices(vkinstance.get());

		return Renderer::VulkanDevice(vkinstance.get(), physdevs[0]);
	}

}
