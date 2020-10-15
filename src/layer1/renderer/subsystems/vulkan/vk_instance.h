#ifndef VK_INSTANCE_H
#define VK_INSTANCE_H

#include "layer1/renderer/subsystems/required.h"
#include "vulkanrequired.h"

namespace Renderer {

	class VulkanInstance {
	public:
		inline VulkanInstance() : Loaded(false) {  }
		inline ~VulkanInstance() {}
		void Release();

		bool Load(SDL_Window *window);

		inline VkInstance get() { return instance; }
		inline bool getLoaded() { return Loaded; }
	protected:
		void InitDebug();
		bool Loaded;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
	};

}

#endif
