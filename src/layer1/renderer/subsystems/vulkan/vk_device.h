#ifndef VK_DEVICE_H
#define VK_DEVICE_H

#include "vulkanrequired.h"
#include "vk_instance.h"

#include <vector>
#include <string>

namespace Renderer {

	std::vector<VkPhysicalDevice> QueryPhysicalDevices(VkInstance instance);

	class VulkanDevice {
	public:
		VulkanDevice() { freed = true; }
		VulkanDevice(const VulkanDevice &dev);
		VulkanDevice(VkInstance instance, VkPhysicalDevice PhysicalDevice);
		virtual ~VulkanDevice() {  }
		void Release();

		void Load(const VulkanDevice &dev);
		void Load(VkInstance instance, VkPhysicalDevice PhysicalDevice);
		uint32_t MemoryType(uint32_t Type, VkMemoryPropertyFlags Props);

		inline void setId(uint32_t id) { this->id = id; }
		inline uint32_t getId() { return id; }
		inline std::string getName() { return vendorname; }

		inline VkDevice get() { return device; }
		inline VkQueue getGraphicsQueue() { return GraphicsQueue; }
		inline VkQueue getTransferQueue() { return TransferQueue; }
		inline VkQueue getComputeQueue()  { return TransferQueue; }
		inline VkPhysicalDeviceProperties getProps() { return PhysicalDeviceProps; }
		inline VkPhysicalDevice getPhysicalDev() { return PhysicalDevice; }
		inline VkCommandPool getTransferCommandPool() { return TransferPool; }
		inline VkCommandPool getGraphicsCommandPool() { return GraphicsPool; }
		inline VkCommandPool  getComputeCommandPool() { return ComputePool; }
		inline uint32_t *getQueueFamilyIndices() { return QueueFamilyIndices; } // [0] = Graphics, [1] = Transfer, [2] = Compute
	
		inline void WaitIdle() { vkDeviceWaitIdle(device); }

		void operator= (const VulkanDevice &devb);
	protected:
		VkPhysicalDevice PhysicalDevice;
		VkPhysicalDeviceProperties PhysicalDeviceProps;
		VkDevice device;
		VkQueue GraphicsQueue;
		VkQueue TransferQueue;
		VkCommandPool TransferPool;
		VkCommandPool GraphicsPool;
		VkCommandPool ComputePool;
		uint32_t QueueFamilyIndices[2];
		uint32_t id;
		std::string vendorname;
		bool freed;
	};

	std::vector<VulkanDevice> QueryAllDevices(VulkanInstance vkinstance);
	VulkanDevice QueryDevice(VulkanInstance vkinstance);

}

#endif
