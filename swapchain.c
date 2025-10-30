#include "swapchain.h"
#include "app.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdlib.h>

void swapchain_info_gather(swapchain_info *info, VkPhysicalDevice device, VkSurfaceKHR surface)
{
    if (info == NULL) return;
    if (device == VK_NULL_HANDLE) return;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &info->capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &info->format_count, NULL);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &info->present_count, NULL);
    
    /* abort ship */
    if (info->format_count >= 64) 
    {
        info->format_count = 0; return;
    }

    if (info->present_count >= 64) 
    {
        info->present_count = 0; return;
    }

    uint32_t _unused;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &_unused, info->formats);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &_unused, info->presents);
}


static VkPresentModeKHR choose_swap_surface_present_mode(const VkPresentModeKHR *presents, size_t count)
{
    return VK_PRESENT_MODE_FIFO_KHR; /* vsync */
}

/* UNTESTED */

#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))

static VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR *capabilities, uint32_t width, uint32_t height)
{
    VkExtent2D extent = {0, 0};

    if (capabilities == NULL) return extent;

    if (capabilities->currentExtent.width != (uint32_t)(-1))
    {
        return capabilities->currentExtent;
    }

    extent.width = CLAMP(extent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
    extent.height = CLAMP(extent.width, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);

    return extent;
}

static VkSurfaceFormatKHR choose_swap_surface_format(const VkSurfaceFormatKHR *formats, size_t count)
{
    if (formats == NULL) fprintf(stderr, "%s: formats null\n", __func__);
    if (count <= 0) fprintf(stderr, "%s: count is zero\n", __func__);

    for (size_t idx=0; idx<count; ++idx)
    {
        VkSurfaceFormatKHR format = formats[idx];

        if (format.format != VK_FORMAT_B8G8R8_SRGB)
        {
            continue;
        }

        if (format.colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            continue;
        }

        return format;
    }

    return formats[0]; /* NOTE: oh well */
}

static int create_swapchain_image_view(VkDevice logical, VkImage image, VkImageView *view, VkFormat format)
{
    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = image;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = format;
    
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    return vkCreateImageView(logical, &create_info, NULL, view) != VK_SUCCESS;
}

static int store_swapchain_images(project_app *app)
{
    if (app == NULL) return 0;

    uint32_t count = 0;
    vkGetSwapchainImagesKHR(app->vk_logical, app->swapchain, &count, NULL);

    app->images = calloc(count, sizeof(VkImage));
    app->image_views = calloc(count, sizeof(VkImageView));

    if (app->images == NULL)
    {
        fprintf(stderr, "%s: app->images failed to allocate\n", __func__);
        return 0;
    }

    if (app->image_views == NULL)
    {
        fprintf(stderr, "%s: app->image_views failed to allocate\n", __func__);
        return 0;
    }

    printf("%s: get swapchain images of %d\n", __func__, count);
    app->image_count = count;
    vkGetSwapchainImagesKHR(app->vk_logical, app->swapchain, &count, app->images);

    for (size_t idx=0; idx<count; ++idx)
    {
        if (create_swapchain_image_view(app->vk_logical, app->images[idx], app->image_views + idx, app->swapchain_format) != VK_SUCCESS) 
        {
            fprintf(stderr, "%s: %zu: failed to create image view\n", __func__, idx);
            return 0;
        }
    }

    return 1;
}

int app_graphics_create_swapchain(project_app *app, uint32_t width, uint32_t height)
{
    if (app == NULL) return 0;
    if (app->vk_device == VK_NULL_HANDLE) return 0;
    if (app->surface == VK_NULL_HANDLE) return 0;

    swapchain_info info = {};
    swapchain_info_gather(&info, app->vk_device, app->surface);

    VkSurfaceFormatKHR format = choose_swap_surface_format(info.formats, info.format_count);
    VkPresentModeKHR present_mode = choose_swap_surface_present_mode(info.presents, info.present_count);
    VkExtent2D extent = choose_swap_extent(&info.capabilities, width, height);

    uint32_t image_count = info.capabilities.minImageCount + 1;
    if (image_count > info.capabilities.maxImageCount) image_count = info.capabilities.maxImageCount;

    printf("%s: image_count: %d\n", __func__, image_count);

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = app->surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = format.format;
    create_info.imageColorSpace = format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    gpu_queue_indexes indexes = gpu_find_queue_families(app->vk_device, app->surface);
    uint32_t indices[2] = {indexes.graphics, indexes.present};

    if (indexes.graphics != indexes.present)
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = indices;
    }
    else
    {
        /* optional */
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = NULL;
    }

    create_info.preTransform = info.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(app->vk_logical, &create_info, NULL, &app->swapchain) != VK_SUCCESS) return 0;

    app->swapchain_format = format.format;
    app->swapchain_extent = extent;

    if (!store_swapchain_images(app)) return 0;

    return 1;
}
