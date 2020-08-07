#include "vk_instance.h"

#include <iostream>
#include <string>
#include <vector>

namespace Renderer {

	VkResult __CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (!func)
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}

	void __DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func)
			func(instance, debugMessenger, pAllocator);
	}


	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *data, void *userdata)
	{
		std::cerr << data->pMessage << std::endl;

		return VK_FALSE;
	}

	void VulkanInstance::Release()
	{
#ifdef __DEBUG
		__DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
		vkDestroyInstance(instance, nullptr);
	}

	bool VulkanInstance::Load(SDL_Window *window)
	{
		if (Loaded == true)
			return true;
		else
			Loaded = true;
		
		
		unsigned int extensionCount = 0;
		std::vector<const char *> extensions;

		if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr)) {
			FATAL("Cannot get Vulkan Extension Count!");
			return false;
		}

		extensions.reserve(extensionCount);

		if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data())) {
			FATAL("Cannot get Vulkan Extensions!");
			return false;
		}

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "vkcl";
		appInfo.applicationVersion = 0;
		appInfo.pEngineName = "vkcl";
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
		std::vector<const char *>layernames = {
			"VK_LAYER_KHRONOS_validation" // depending on how your system's vulkan sdk is setup, you might wanna change this.
		};
		
		VkDebugUtilsMessengerCreateInfoEXT createinfo = {};
		createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createinfo.pfnUserCallback = debugCallback;

		instInfo.enabledLayerCount = 1;
		instInfo.ppEnabledLayerNames = layernames.data();
		instInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &createinfo;

		extensionCount++;
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		instInfo.enabledExtensionCount = extensionCount;
		instInfo.ppEnabledExtensionNames = extensions.data();
		VK_ASSERT(vkCreateInstance(&instInfo, nullptr, &instance), "Failed to create instance")

#ifdef __DEBUG
		// setup debug messenger
		VK_ASSERT(__CreateDebugUtilsMessengerEXT(instance, &createinfo, nullptr, &debugMessenger), "Failed to create debugger messenger callback")
#endif

		return true;
	}

}
