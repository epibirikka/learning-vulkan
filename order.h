#ifndef _GRAPHICS_ORDER_H
#define _GRAPHICS_ORDER_H

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

int order_create_semaphore(VkDevice logical, VkSemaphore *semaphore);
int order_create_fence(VkDevice logical, VkFence *fence, int signaled);

void order_destroy_semaphore(VkDevice logical, VkSemaphore *semaphore);
void order_destroy_fence(VkDevice logical, VkFence *fence);

#endif
