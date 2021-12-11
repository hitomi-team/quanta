#include "level0/pch.h"

#include "api.h"

// CPU Accessible Device Memory
// (Usually the PCI BAR on x86)
static const VkMemoryPropertyFlags VULKAN_ACCESSIBLE_DEVICE_MEMORY = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice _handle)
{
	static const std::array< EPhysicalDeviceType, 5 > deviceTypes {
		PHYSICAL_DEVICE_TYPE_OTHER,
		PHYSICAL_DEVICE_TYPE_INTEGRATED,
		PHYSICAL_DEVICE_TYPE_DISCRETE,
		PHYSICAL_DEVICE_TYPE_VIRTUAL,
		PHYSICAL_DEVICE_TYPE_CPU
	};

	this->handle = _handle;

	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;

	vkGetPhysicalDeviceProperties(this->handle, &properties);
	vkGetPhysicalDeviceFeatures(this->handle, &features);

	uint32_t numExtensions;
	vkEnumerateDeviceExtensionProperties(this->handle, nullptr, &numExtensions, nullptr);
	std::vector< VkExtensionProperties > extensions;
	vkEnumerateDeviceExtensionProperties(this->handle, nullptr, &numExtensions, extensions.data());

	vkGetPhysicalDeviceMemoryProperties(this->handle, &this->memProperties);

	this->name = properties.deviceName;

	this->cachedInfo = {};
	this->cachedInfo.hardware.name = properties.deviceName;
	this->cachedInfo.hardware.vendorID = properties.vendorID;
	this->cachedInfo.hardware.deviceID = properties.deviceID;
	this->cachedInfo.hardware.type = deviceTypes[properties.deviceType];

	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(this->handle, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		VkMemoryType type = memoryProperties.memoryTypes[i];

		// look for device local host visible heap
		// * Assume SAM off when no such heap is present
		if ((type.propertyFlags & VULKAN_ACCESSIBLE_DEVICE_MEMORY) == VULKAN_ACCESSIBLE_DEVICE_MEMORY) {
			VkMemoryHeap heap = memoryProperties.memoryHeaps[type.heapIndex];

			// now check if the heap is device local
			if (heap.flags & (VK_MEMORY_HEAP_DEVICE_LOCAL_BIT | VK_MEMORY_HEAP_MULTI_INSTANCE_BIT)) {
				// determine SAM support by checking that BAR heap size is over 256 MiB
				// for SoC / integrated solutions we may toggle this always on
				if (heap.size > 0x10000000 || this->cachedInfo.hardware.type == PHYSICAL_DEVICE_TYPE_INTEGRATED)
					this->apiFeatures.hasSmartAccessMemory = true;
			}
		}
	}

	if (this->cachedInfo.hardware.type == PHYSICAL_DEVICE_TYPE_INTEGRATED && properties.vendorID == 0x1002)
		this->apiFeatures.vulkanAMDAPUHeapWorkaround = true;
}

RenderPhysicalDeviceInfo VulkanPhysicalDevice::GetInfo()
{
	return this->cachedInfo;
}

std::shared_ptr< IRenderDevice > VulkanPhysicalDevice::CreateLogicalDevice()
{
	return std::dynamic_pointer_cast< IRenderDevice >(std::make_shared< VulkanDevice >(this));
}
