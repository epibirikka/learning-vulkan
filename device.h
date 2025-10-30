#ifndef _GRAPHICS_DEVICE_H
#define _GRAPHICS_DEVICE_H

#include <vulkan/vulkan.h>

typedef struct
{
    uint32_t graphics;
    uint32_t present;
} gpu_queue_indexes;

#define gpu_queue_indexes_complete(idxes) \
    ((idxes)->graphics != -1 && (idxes)->present != -1)

void gpu_debug_device(VkPhysicalDevice device);
void gpu_debug_devices(VkPhysicalDevice *devices, uint32_t count);
VkPhysicalDevice gpu_find_suitable_device(VkSurfaceKHR surface, VkPhysicalDevice *devices, uint32_t count);

/* NOTE: in a real world scenario, we can allow different queues to be written */

/* something like a [x] Improve Rendering Performance button for the same queue 
 * to be written as graphics and presentation queues.*/

/* For now we assume they are in the same queue. Otherwise, abort ship.*/

/* Nevermind! */

gpu_queue_indexes gpu_find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);

#endif
