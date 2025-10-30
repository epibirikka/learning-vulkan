#include "command.h"
#include "device.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <stdlib.h>
#include <stdio.h>

void commander_setup(commander *commander, VkPhysicalDevice device, VkDevice logical, VkSurfaceKHR surface, framebuffer_list *framebuffers, VkPipeline pipeline)
{
    if (device == VK_NULL_HANDLE)
    {
        fprintf(stderr, "%s: device is null handle\n", __func__);
        return;
    }

    if (logical == VK_NULL_HANDLE)
    {
        fprintf(stderr, "%s: logical device is null handle\n", __func__);
        return;
    }

    commander->framebuffers = framebuffers;
    commander->pipeline = pipeline;
    commander->logical = logical;

    gpu_queue_indexes indexes = gpu_find_queue_families(device, surface);

    VkCommandPoolCreateInfo cmd_pool_create_info = {};
    cmd_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; 
    cmd_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; 
    cmd_pool_create_info.queueFamilyIndex = indexes.graphics; 

    if (vkCreateCommandPool(logical, &cmd_pool_create_info, NULL, &commander->pool) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: failed to create command pool for %p\n", __func__, commander);
        return;
    }

    VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {};
    cmd_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_buffer_allocate_info.commandPool = commander->pool;
    cmd_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd_buffer_allocate_info.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(logical, &cmd_buffer_allocate_info, &commander->buffer) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: failed to allocate command buffer for %p\n", __func__, commander);
        return;
    }

    printf("%s: commander has an allocated command buffer\n", __func__);
    commander->setup = 1;
    commander->recorded = 0;
}

#define CHECK_IF_INVALID_EXIT(commander) \
    if (commander == NULL) return; \
    if (commander->framebuffers == NULL) return; \
    if (commander->framebuffers->render_pass == VK_NULL_HANDLE) return; \
    if (commander->framebuffers->count <= 0 || commander->framebuffers->framebuffers == NULL) return; \
    if (commander->buffer == VK_NULL_HANDLE) return; \
    if (commander->pipeline == VK_NULL_HANDLE) return; \

void commander_reset(commander *commander)
{
    CHECK_IF_INVALID_EXIT(commander);
    commander->submitted = 0;
    commander->recorded = 0;
    vkResetCommandBuffer(commander->buffer, 0);
}

void commander_start_recording(commander *commander, uint32_t image_index)
{
    CHECK_IF_INVALID_EXIT(commander);

    if (commander->logical == VK_NULL_HANDLE) 
    {
        fprintf(stderr, "%s: cannot start recording without the logical device that created it\n", __func__);
        return;
    };

    if (image_index >= commander->framebuffers->count)
    {
        fprintf(stderr, "%s: image_index %d exceeds %zu framebuffers\n", __func__, image_index, commander->framebuffers->count);
        return;
    }

    commander->recorded = 0;

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(commander->buffer, &begin_info) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: failed to begin command buffer (%p)\n", __func__, commander);
        return;
    }

    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = commander->framebuffers->render_pass;
    render_pass_begin_info.framebuffer = commander->framebuffers->framebuffers[image_index];

    render_pass_begin_info.renderArea.offset.x = 0;
    render_pass_begin_info.renderArea.offset.y = 0;
    render_pass_begin_info.renderArea.extent = commander->framebuffers->extents;

    VkClearValue clear_color = {{{0.f, 0.f, 0.f, 1.f}}};
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(commander->buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commander->buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, commander->pipeline);
    vkCmdSetViewport(commander->buffer, 0, 1, &commander->framebuffers->viewport);
    vkCmdSetScissor(commander->buffer, 0, 1, &commander->framebuffers->scissor);
    vkCmdDraw(commander->buffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commander->buffer);

    if (vkEndCommandBuffer(commander->buffer) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: failed to record command buffer\n", __func__);
        return;
    }

    commander->recorded = 1;
}

#define COMPLAIN(msg, ...) \
    { \
        fprintf(stderr, msg, __VA_ARGS__); \
        return; \
    } \

void commander_present(commander *commander, uint32_t image_index, VkSwapchainKHR swapchain, VkQueue present_queue, VkSemaphore semaphore_wait_for)
{
    CHECK_IF_INVALID_EXIT(commander);

    if (semaphore_wait_for == VK_NULL_HANDLE) COMPLAIN("%s: semaphore_wait_for is null handle\n", __func__);
    if (swapchain == VK_NULL_HANDLE) COMPLAIN("%s: swapchain is null handle\n", __func__);
    if (present_queue == VK_NULL_HANDLE) COMPLAIN("%s: present_queue is null handle\n", __func__);

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &semaphore_wait_for;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain;
    present_info.pImageIndices = &image_index;
    present_info.pResults = NULL;

    vkQueuePresentKHR(present_queue, &present_info);
}

void commander_submit(commander *commander, VkQueue queue, VkSemaphore semaphore_wait_for, VkSemaphore semaphore_make_wait, VkFence fence_trigger)
{
    CHECK_IF_INVALID_EXIT(commander);

    if (semaphore_wait_for == VK_NULL_HANDLE) return;
    if (semaphore_make_wait == VK_NULL_HANDLE) return;
    if (fence_trigger == VK_NULL_HANDLE) return;

    commander->submitted = 0;

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &semaphore_wait_for;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &commander->buffer;

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &semaphore_make_wait;

    if (vkQueueSubmit(queue, 1, &submit_info, fence_trigger) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: failed to submit the command buffer (%p)\n", __func__, commander);
        return;
    }

    commander->submitted = 1;
}

void commander_free(commander *commander)
{
    if (commander == NULL)
    {
        fprintf(stderr, "%s: cannot free a null pointer\n", __func__);
        return; 
    }

    if (commander->logical == VK_NULL_HANDLE) 
    {
        fprintf(stderr, "%s: cannot free vk command pool without the logical device that created it\n", __func__);
        return;
    }

    printf("%s: freeing commander pool\n", __func__);
    vkDestroyCommandPool(commander->logical, commander->pool, NULL);
    commander->pool = VK_NULL_HANDLE;
    commander->submitted = 0;
}
