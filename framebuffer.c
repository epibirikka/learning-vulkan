#include "framebuffer.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdlib.h>

void framebuffer_create(framebuffer_list *list, size_t count, VkDevice logical, VkRenderPass render_pass, VkImageView *image_views, VkExtent2D extents, VkViewport viewport, VkRect2D scissor)
{
    if (list == NULL) return;
    if (count <= 0) return;
    if (render_pass == VK_NULL_HANDLE) return;
    if (image_views == NULL) return;

    list->logical = logical;
    list->extents = extents;
    list->render_pass = render_pass;

    list->viewport = viewport;
    list->scissor = scissor;

    list->count = count;
    list->framebuffers = calloc(list->count, sizeof(VkFramebuffer));
    for (size_t idx=0; idx<list->count; ++idx) list->framebuffers[idx] = VK_NULL_HANDLE;

    if (list->framebuffers == NULL)
    {
        fprintf(stderr, "%s: failed to allocate framebuffer block for list (%p)\n", __func__, list);
        return;
    }

    if (extents.width <= 0 || extents.height <= 0)
    {
        fprintf(stderr, "%s: extents are minimized but framebuffer list of %zu is still allocated\n", __func__, list->count);
        return;
    }

    for (size_t idx=0; idx<list->count; ++idx)
    {
        VkFramebufferCreateInfo framebuffer_create_info = {};
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.renderPass = render_pass;
        framebuffer_create_info.attachmentCount = 1;
        framebuffer_create_info.pAttachments = image_views + idx;
        framebuffer_create_info.width = extents.width;
        framebuffer_create_info.height = extents.height;
        framebuffer_create_info.layers = 1;

        if (vkCreateFramebuffer(logical, &framebuffer_create_info, NULL, list->framebuffers + idx) != VK_SUCCESS)
        {
            fprintf(stderr, "%s: %zu: failed to create framebuffer for list (%p)\n", __func__, idx, list);
            return;
        }
    }

    printf("%s: framebuffers of %zu are allocated\n", __func__, list->count);
}

void framebuffer_free(framebuffer_list *list)
{
    if (list == NULL) return;
    if (list->framebuffers == NULL) return;
    if (list->count <= 0) return;

    if (list->logical == VK_NULL_HANDLE)
    {
        fprintf(stderr, "%s: cannot free vk framebuffers because %p->logical is a null handle\n", __func__, list);
        return;
    }

    for (size_t idx=0; idx<list->count; ++idx)
    {
        vkDestroyFramebuffer(list->logical, list->framebuffers[idx], NULL);
    }

    free(list->framebuffers);
    list->framebuffers = NULL;
    list->count = 0;
}
