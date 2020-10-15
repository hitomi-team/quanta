#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include "vk_device.h"

namespace Renderer {

	class VulkanBuffer {
	public:
		VulkanBuffer() : released(true), freed(true) { }
		VulkanBuffer(VulkanDevice *dev, VkDeviceSize size) { this->Load(dev, size); }
		void Load(VulkanDevice *dev, VkDeviceSize size);
		void Delete();
		~VulkanBuffer();

		void UploadData(void *data); // Uploads with given size from constructor
		void *GetData(); // Returns address to where data is written
		void ReleaseData(); // Releases objects allocated from GetData();

		inline VkDeviceSize getSize() { return this->size; }
		inline VkBuffer getHostBuffer() { return this->host_buffer; }
		inline VkBuffer getDevBuffer() { return this->device_buffer; }
	protected:
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageflags, VmaMemoryUsage memusageflags, VkBuffer &buffer, VmaAllocation &alloc, VmaAllocationInfo *allocinfo);
		void CopyBuffer(VkBuffer src, VkBuffer dst);

		VulkanDevice *dev;
		// device
		VmaAllocation device_alloc;
		VkBuffer device_buffer;
		// staging
		VmaAllocation host_alloc;
		VkBuffer host_buffer;
		// total size of the buffer
		VkDeviceSize size;

		bool released;
		bool freed;
	};

}

#endif
