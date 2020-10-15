#ifndef VK_DEVICE_H
#define VK_DEVICE_H

#include "vulkanrequired.h"
#include "vk_instance.h"

namespace Renderer {

	std::vector<VkPhysicalDevice> QueryPhysicalDevices(VkInstance instance);

	class VulkanDevice {
	public:
		VulkanDevice() { freed = true; }
		VulkanDevice(VkInstance instance, VkPhysicalDevice PhysicalDevice);

		inline VulkanDevice(VkInstance instance, VkPhysicalDevice adapter, int id)
		{
			this->Load(instance, adapter);
			this->setId(id);
		}

		inline virtual ~VulkanDevice() {  }
		void Release();

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
		inline VkCommandPool getGraphicsCommandPool() { return GraphicsPool; }
		inline VkCommandPool getTransferCommandPool() { return TransferPool; }
		inline VkCommandPool getComputeCommandPool() { return ComputePool; }
		inline VmaAllocator getAllocator() { return allocator; }
		inline const uint32_t *getQueueFamilyIndices() { return QueueFamilyIndices; } // [0] = Graphics, [1] = Transfer, [2] = Compute

		inline void WaitIdle() { this->fn.vkDeviceWaitIdle(this->device); }

		VolkDeviceTable fn;
	protected:
		VkPhysicalDevice PhysicalDevice;
		VkPhysicalDeviceProperties PhysicalDeviceProps;
		VkDevice device;
		VkQueue GraphicsQueue;
		VkQueue TransferQueue;
		VkQueue ComputeQueue;
		VkCommandPool GraphicsPool;
		VkCommandPool TransferPool;
		VkCommandPool ComputePool;
		VmaAllocator allocator;
		uint32_t QueueFamilyIndices[3];
		uint32_t id;
		std::string vendorname;
		bool freed;
	};

	std::vector<VulkanDevice> QueryAllDevices(VulkanInstance *vkinstance);
	VulkanDevice QueryDevice(VulkanInstance *vkinstance);

}

#endif
