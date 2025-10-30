#ifndef _GRAPHICS_SWAPCHAIN_H
#define _GRAPHICS_SWAPCHAIN_H

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

typedef struct
{
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR formats[64];
    VkPresentModeKHR presents[64];

    uint32_t format_count;
    uint32_t present_count;
} swapchain_info;

void swapchain_info_gather(swapchain_info *info, VkPhysicalDevice device, VkSurfaceKHR surface);
void swapchain_format_suitable();

#endif
