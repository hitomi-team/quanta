#include "level0/pch.h"
#include "level0/log.h"

#include "api.h"

#ifndef NDEBUG
const std::array< const char *, 1 > g_VulkanLayerNames {
	"VK_LAYER_KHRONOS_validation"
};

VkBool32 g_DebugUtilsMessenger(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
	void *userData
)
{
	auto api = reinterpret_cast< VulkanAPI * >(userData);

	if (api != nullptr) {
		api->DebugUtilsMessenger(
			messageSeverity,
			messageType,
			callbackData
		);
	}

	return VK_FALSE;
}
#endif

VulkanAPI::VulkanAPI(const char *titleName) : RenderAPI("Vulkan")
{
	m_window = SDL_CreateWindow(
		titleName,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1280,
		720,
		SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_VULKAN
	);

	if (m_window == nullptr)
		throw std::runtime_error("Cannot create SDL2 Window for Rendering!");

	unsigned numExtensions;
	SDL_Vulkan_GetInstanceExtensions(m_window, &numExtensions, nullptr);
	requiredExtensions.resize(numExtensions);
	SDL_Vulkan_GetInstanceExtensions(m_window, &numExtensions, requiredExtensions.data());

	requiredExtensions.push_back("VK_KHR_get_physical_device_properties2");

	volkInitializeCustom(reinterpret_cast< PFN_vkGetInstanceProcAddr >(SDL_Vulkan_GetVkGetInstanceProcAddr()));

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = titleName;
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pEngineName = "quanta";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.apiVersion = VK_MAKE_VERSION(1, 2, 0);

#ifndef NDEBUG
	requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	requiredExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);

	static const std::array< VkValidationFeatureEnableEXT, 4 > validationEnable {
		VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
		VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
		VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
		VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
	};

	VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
	debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugMessengerCreateInfo.pNext = nullptr;
	debugMessengerCreateInfo.flags = 0;
	debugMessengerCreateInfo.messageSeverity = \
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugMessengerCreateInfo.messageType = \
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugMessengerCreateInfo.pfnUserCallback = g_DebugUtilsMessenger;
	debugMessengerCreateInfo.pUserData = this;

	VkValidationFeaturesEXT validationFeatures;
	validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	validationFeatures.pNext = &debugMessengerCreateInfo;
	validationFeatures.enabledValidationFeatureCount = static_cast< uint32_t >(validationEnable.size());
	validationFeatures.pEnabledValidationFeatures = validationEnable.data();
	validationFeatures.disabledValidationFeatureCount = 0;
	validationFeatures.pDisabledValidationFeatures = nullptr;
#endif

	VkInstanceCreateInfo instCreateInfo;
	instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#ifndef NDEBUG
	instCreateInfo.pNext = &validationFeatures;
#else
	instCreateInfo.pNext = nullptr;
#endif
	instCreateInfo.flags = 0;
	instCreateInfo.pApplicationInfo = &appInfo;
#ifndef NDEBUG
	instCreateInfo.enabledLayerCount = static_cast< uint32_t >(g_VulkanLayerNames.size());
	instCreateInfo.ppEnabledLayerNames = g_VulkanLayerNames.data();
#else
	instCreateInfo.enabledLayerCount = 0;
	instCreateInfo.ppEnabledLayerNames = nullptr;
#endif
	instCreateInfo.enabledExtensionCount = static_cast< uint32_t >(requiredExtensions.size());
	instCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

	if (vkCreateInstance(&instCreateInfo, nullptr, &this->inst) != VK_SUCCESS)
		throw std::runtime_error("VulkanAPI: Cannot create VkInstance!");

	volkLoadInstance(this->inst);

#ifndef NDEBUG
	if (vkCreateDebugUtilsMessengerEXT(this->inst, &debugMessengerCreateInfo, nullptr, &m_debugUtilsMessenger) != VK_SUCCESS)
		g_Log.Warn("Continuing without VkDebugUtilsMessengerEXT support");
#endif

	uint32_t numPhy;
	vkEnumeratePhysicalDevices(this->inst, &numPhy, nullptr);

	if (numPhy == 0)
		throw std::runtime_error("VulkanAPI: Cannot find hardware with Vulkan support!");

	std::vector< VkPhysicalDevice > vulkanPhysicalDevices(numPhy);
	vkEnumeratePhysicalDevices(this->inst, &numPhy, vulkanPhysicalDevices.data());

	for (auto &vulkanPhysicalDevice : vulkanPhysicalDevices) {
		auto physicalDevice = std::make_shared< VulkanPhysicalDevice >(vulkanPhysicalDevice);
		this->physicalDevices.push_back(std::dynamic_pointer_cast< IRenderPhysicalDevice >(physicalDevice));
		g_Log.Info(FMT_COMPILE("Found Vulkan hardware: {} (ReBAR/UMA: {})"), physicalDevice->name, physicalDevice->apiFeatures.hasReBARUMA);
	}

	if (!SDL_Vulkan_CreateSurface(m_window, this->inst, &this->surface))
		throw std::runtime_error("VulkanAPI: Cannot create Vulkan surface!");
}

VulkanAPI::~VulkanAPI()
{
#ifndef NDEBUG
	if (m_debugUtilsMessenger != VK_NULL_HANDLE) {
		vkDestroyDebugUtilsMessengerEXT(this->inst, m_debugUtilsMessenger, nullptr);
		m_debugUtilsMessenger = VK_NULL_HANDLE;
	}
#endif

	if (this->surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(this->inst, this->surface, nullptr);
		this->surface = VK_NULL_HANDLE;
	}

	if (this->inst != VK_NULL_HANDLE) {
		vkDestroyInstance(this->inst, nullptr);
		this->inst = VK_NULL_HANDLE;
	}

	if (m_window != nullptr) {
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}
}

#ifndef NDEBUG
void VulkanAPI::DebugUtilsMessenger(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT *callbackData
)
{
	g_Log.Debug(
		FMT_COMPILE("VulkanAPI::DebugUtilsMessenger messageSeverity:{:08X} messageType:{} id:{}({:X}) message:{}"),
		static_cast< uint32_t >(messageSeverity), messageType, callbackData->pMessageIdName != nullptr ? callbackData->pMessageIdName : "unknown", callbackData->messageIdNumber, callbackData->pMessage != nullptr ? callbackData->pMessage : "unknown"
	);
}
#endif

VulkanAPI *g_VulkanAPI = nullptr;
