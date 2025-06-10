#ifndef _GRAPHICS_DEVICE_H
#define _GRAPHICS_DEVICE_H

#include <vulkan/vulkan.h>

void gpu_debug_device(VkPhysicalDevice device);
void gpu_debug_devices(VkPhysicalDevice *devices, uint32_t count);
VkPhysicalDevice gpu_find_suitable_device(VkPhysicalDevice *devices, uint32_t count);

#endif
