#ifndef RENDERER_VULKANREQUIRED_H
#define RENDERER_VULKANREQUIRED_H

#include "volk.h"
#include <SDL2/SDL_vulkan.h>

#define VK_ASSERT(X, STR) if (X != VK_SUCCESS) { global_log.Error(StringFormat("vk error (", std::hex, X, std::dec, "): ", STR)); abort(); }
#define VK_FATAL(X, STR) if (X) { global_log.Error(StringFormat("vk error: ", STR)); }

#endif
