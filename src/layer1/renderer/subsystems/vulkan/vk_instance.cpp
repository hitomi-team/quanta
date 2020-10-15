#include "pch/pch.h"

#include "vk_instance.h"

namespace Renderer {

	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *data, void *userdata)
	{
		(void)severity;
		(void)type;
		(void)userdata;

		global_log.Warn(data->pMessage);

		return VK_FALSE;
	}

	void VulkanInstance::Release()
	{
#ifdef __DEBUG
		vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
		vkDestroyInstance(instance, nullptr);
	}

	bool VulkanInstance::Load(SDL_Window *window)
	{
		if (Loaded == true)
			return true;
		else
			Loaded = true;

		uint32_t extensionCount = 0;
		std::vector<const char *> extensions;

		if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr)) {
			FATAL("Cannot get Vulkan Extension Count!");
			return false;
		}

		extensions.resize(extensionCount);

		if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data())) {
			FATAL("Cannot get Vulkan Extensions!");
			return false;
		}

		volkInitializeCustom(reinterpret_cast< PFN_vkGetInstanceProcAddr >(SDL_Vulkan_GetVkGetInstanceProcAddr()));

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "quanta";
		appInfo.applicationVersion = 0;
		appInfo.pEngineName = "quanta";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
		appInfo.apiVersion = VK_MAKE_VERSION(1, 1, 0);

		VkInstanceCreateInfo instInfo = {};
		instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instInfo.pNext = nullptr;
		instInfo.flags = 0;
		instInfo.pApplicationInfo = &appInfo;
#ifndef __DEBUG
		instInfo.enabledLayerCount = 0;
		instInfo.ppEnabledLayerNames = nullptr;
#else
		static const char *layernames[] = {
			"VK_LAYER_KHRONOS_validation" // depending on how your system's vulkan sdk is setup, you might wanna change this.
		};

		VkDebugUtilsMessengerCreateInfoEXT createinfo = {};
		createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createinfo.pfnUserCallback = debugCallback;

		instInfo.enabledLayerCount = ARRAY_SIZE(layernames);
		instInfo.ppEnabledLayerNames = layernames;
		instInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &createinfo;

		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		instInfo.enabledExtensionCount = static_cast< uint32_t >(extensions.size());
		instInfo.ppEnabledExtensionNames = extensions.data();
		VK_ASSERT(vkCreateInstance(&instInfo, nullptr, &instance), "Failed to create instance")

		volkLoadInstance(instance);

#ifdef __DEBUG
		// setup debug messenger
		if (vkCreateDebugUtilsMessengerEXT(instance, &createinfo, nullptr, &debugMessenger) != VK_SUCCESS)
			global_log.Warn("Failed to create Vulkan debug messenger; continuing without this support");
#endif

		return true;
	}

}
