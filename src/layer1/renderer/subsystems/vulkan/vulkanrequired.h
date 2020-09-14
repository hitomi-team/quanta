#ifndef RENDERER_VULKANREQUIRED_H
#define RENDERER_VULKANREQUIRED_H

#include "volk.h"
#include <SDL2/SDL_vulkan.h>

#define VK_ASSERT(X, STR) if (X != VK_SUCCESS) { global_log.Error(fmt::format("vk error {:08}: {}", X, STR)); abort(); }
#define VK_FATAL(X, STR) if (X) { global_log.Error(fmt::format("vk error: {}", STR)); }

#endif
