#include "order.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <stdio.h>

int order_create_semaphore(VkDevice logical, VkSemaphore *semaphore)
{
    if (logical == VK_NULL_HANDLE) return 0;
    if (semaphore == NULL) return 0;

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(logical, &semaphore_create_info, NULL, semaphore) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: failed to create semaphore %p\n", __func__, semaphore);
        return 0;
    }

    fprintf(stderr, "%s: created semaphore %p\n", __func__, semaphore);
    return 1;
}

int order_create_fence(VkDevice logical, VkFence *fence, int signaled)
{
    if (logical == VK_NULL_HANDLE) return 0;
    if (fence == NULL) return 0;

    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (signaled) fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(logical, &fence_create_info, NULL, fence) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: failed to create fence %p\n", __func__, fence);
        return 0;
    }

    fprintf(stderr, "%s: created fence %p\n", __func__, fence);
    return 1;
}

void order_destroy_semaphore(VkDevice logical, VkSemaphore *semaphore)
{
    if (logical == VK_NULL_HANDLE) return;
    if (semaphore == NULL) return;
    if (*semaphore == VK_NULL_HANDLE) return;

    vkDestroySemaphore(logical, *semaphore, NULL);
    *semaphore = VK_NULL_HANDLE;
}

void order_destroy_fence(VkDevice logical, VkFence *fence)
{
    if (logical == VK_NULL_HANDLE) return;
    if (fence == NULL) return;
    if (fence == VK_NULL_HANDLE) return;

    vkDestroyFence(logical, *fence, NULL);
    *fence = VK_NULL_HANDLE;
}
