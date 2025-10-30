#include "device.h"
#include "swapchain.h"

#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int are_device_extensions_supported(VkPhysicalDevice device)
{
    uint32_t count;
    vkEnumerateDeviceExtensionProperties(device, NULL, &count, NULL);
    printf("%s: counting %d device extensions\n", __func__, count);

    VkExtensionProperties *properties = calloc(count, sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(device, NULL, &count, properties);

    if (properties == NULL)
    {
        fprintf(stderr, "%s: properties failed to allocate\n", __func__);
        return 0;
    }

    /* we need to check swapchain support just to be safe */
    static char *extension_list[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        NULL
    };

    size_t _match = 0;
    size_t _target = (sizeof(extension_list) / sizeof(extension_list[0])) - 1;

    /* doesn't matter if it's brute force */
    for (char **p=extension_list; *p != NULL; p++)
    {
        for (size_t idx=0; idx<count; ++idx)
        {
            /* printf("%s: testing %s (%s)\n", __func__, *p, properties[idx].extensionName); */
            if (!strcmp(properties[idx].extensionName, *p)) _match++;
        }
    }

    printf("%s: _match: %zu; target: %zu\n", __func__, _match, _target);
    free(properties);
    return _match >= _target;
}

static int is_swapchain_adequate(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    swapchain_info info = {};
    swapchain_info_gather(&info, device, surface);

    printf("%s: format_count: %d, present_count: %d\n", __func__, info.format_count, info.present_count);
    return info.format_count > 0 && info.present_count > 0;
}

static int are_features_suitable(VkPhysicalDevice device, VkSurfaceKHR surface, VkPhysicalDeviceFeatures features)
{
    return is_swapchain_adequate(device, surface) && are_device_extensions_supported(device);
}

void gpu_debug_device(VkPhysicalDevice device)
{
    static const char *DEVICE_TYPE_NAMES[5] = {
        "OTHER", "INTEGRATED GPU", "DISCRETE GPU", "VIRTUAL GPU", "CPU",
    };

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    fprintf(
        stdout,
        "    [%.*s]\n"
        "    api version: %d\n"
        "    driver version: %d\n"
        "    vendor id: %d\n"
        "    device type: %s\n"
        ,
        VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, properties.deviceName,
        properties.apiVersion,
        properties.driverVersion,
        properties.vendorID,
        DEVICE_TYPE_NAMES[properties.deviceType]
    );
}

void gpu_debug_devices(VkPhysicalDevice *devices, uint32_t count)
{

    for (uint32_t idx=0; idx<count; ++idx)
    {
        VkPhysicalDevice device = devices[idx];
        fprintf(stdout, "%s: device %d\n", __func__, idx);
        gpu_debug_device(device);
    }
}

VkPhysicalDevice gpu_find_suitable_device(VkSurfaceKHR surface, VkPhysicalDevice *devices, uint32_t count)
{
    /* now we're just gonna say that's it, but in the real world, 
     * we need to be able for the program and the user to automatically configure to their GPU device. 
     * we can score for the best GPU when writing settings for the first time. */

    /* maybe a good seperate function would be
     * gpu_find_recommended_device 
     *
     * along with gpu_device_count() and gpu_device_list(dst device buffer, count)*/

    for (uint32_t idx=0; idx<count; ++idx)
    {
        VkPhysicalDevice device = devices[idx];

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        if (are_features_suitable(device, surface, features))
        {
            fprintf(stdout, "%s: device idx %d capable\n", __func__, idx);
            return device;
        }
    }

    return VK_NULL_HANDLE;
}

gpu_queue_indexes gpu_find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    gpu_queue_indexes indexes = {};
    /* -1 could mean any but for now let's keep it this way */
    indexes.graphics = -1;
    indexes.present = -1;

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, NULL);

    if (count <= 0)
    {
        fprintf(stderr, "%s: count <= 0\n", __func__);
        goto done;
    }

    VkQueueFamilyProperties *properties = calloc(count, sizeof(VkQueueFamilyProperties));

    if (properties == NULL)
    {
        fprintf(stderr, "%s: properties allocation failed\n", __func__);
        goto done;
    }

    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties);

    for (uint32_t idx=0; idx<count; ++idx)
    {

        if (properties[idx].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indexes.graphics = idx;
            printf("%s: queue index %d: has graphics bit\n", __func__, idx);
        }

        VkBool32 _check = 0;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, surface, &_check);

        if (_check)
        {
            printf("%s: queue index %d: has surface support\n", __func__, idx);
            indexes.present = idx;
        }

        if (gpu_queue_indexes_complete(&indexes))
        {
            break;
        }
    }

done:
    return indexes;
}
