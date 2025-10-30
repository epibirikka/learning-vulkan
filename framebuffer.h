#ifndef _GRAPHICS_FRAMBUFFER_H
#define _GRAPHICS_FRAMBUFFER_H

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

typedef struct
{
    size_t count;
    VkFramebuffer *framebuffers;
    VkDevice logical;
    VkExtent2D extents;
    VkRenderPass render_pass;

    VkViewport viewport;
    VkRect2D scissor;
} framebuffer_list;

void framebuffer_create(framebuffer_list *list, size_t count, VkDevice logical, VkRenderPass render_pass, VkImageView *image_views, VkExtent2D extents, VkViewport viewport, VkRect2D scissor);
void framebuffer_free(framebuffer_list *list);

#endif
