#ifndef RENDERER_VULKANREQUIRED_H
#define RENDERER_VULKANREQUIRED_H

#include "volk.h"

#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "vk_mem_alloc.h"

#include <SDL2/SDL_vulkan.h>

#include "../required.h"

#define VK_ASSERT(X, STR) do { if (X != VK_SUCCESS) { global_log.Error(FMT_STRING("vk error {:08X}: {}"), X, STR); abort(); } } while (0);
#define VK_FATAL(X, STR) do { if (X) { global_log.Error(FMT_STRING("vk error: {}"), STR); } } while (0);

#endif
