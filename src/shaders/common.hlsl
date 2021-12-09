#ifndef COMMON_HLSL
#define COMMON_HLSL

#ifdef _VULKAN
#define VK_BINDING(x, y) [[vk::binding(x, y)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x, y)
#define VK_LOCATION(x)
#endif

#endif
