#ifndef COMMON_HLSL
#define COMMON_HLSL

#ifdef VULKAN
#define VK_BINDING(x, y) [[vk::binding(x, y)]]
#else
#define VK_BINDING(x, y)
#endif

#endif
