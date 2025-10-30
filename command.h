#ifndef _GRAPHICS_COMMAND_H
#define _GRAPHICS_COMMAND_H

/* TODO */

/* rainbow triangle commander */

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "framebuffer.h"

typedef struct
{
    VkCommandPool pool;
    VkCommandBuffer buffer;
    VkDevice logical;
    VkPipeline pipeline;
    framebuffer_list *framebuffers;
    uint32_t setup;
    uint32_t submitted;
    uint32_t recorded;
} commander;

void commander_setup(commander *commander, VkPhysicalDevice device, VkDevice logical, VkSurfaceKHR surface, framebuffer_list *framebuffers, VkPipeline pipeline);
void commander_reset(commander *commander);
void commander_start_recording(commander *commander, uint32_t image_index);
void commander_submit(commander *commander, VkQueue queue, VkSemaphore semaphore_wait_for, VkSemaphore semaphore_make_wait, VkFence fence_trigger);
void commander_present(commander *commander, uint32_t image_index, VkSwapchainKHR swapchain, VkQueue present_queue, VkSemaphore semaphore_wait_for);
void commander_free(commander *commander);


#endif
