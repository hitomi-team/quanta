#ifndef RENDERER_VULKANREQUIRED_H
#define RENDERER_VULKANREQUIRED_H

#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>

#define VK_ASSERT(X, STR) if (X != VK_SUCCESS) { fprintf(stderr, "vk error (%d): %s\n", X, STR); abort(); }
#define VK_FATAL(X, STR) if (X) { fprintf(stderr, "vk error: %s\n", STR); }

#endif
