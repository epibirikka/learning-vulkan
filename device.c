#include "device.h"
#include <vulkan/vulkan.h>

static int are_features_suitable(VkPhysicalDeviceFeatures features)
{
    return 1;
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

static void gpu_debug_devices(VkPhysicalDevice *devices, uint32_t count)
{

    for (uint32_t idx=0; idx<count; ++idx)
    {
        VkPhysicalDevice device = devices[idx];
        fprintf(stdout, "%s: device %d\n", __func__, idx);
        debug_device(device);
    }
}

VkPhysicalDevice gpu_find_suitable_device(VkPhysicalDevice *devices, uint32_t count)
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

        if (are_features_suitable(features))
        {
            return device;
        }
    }

    return VK_NULL_HANDLE;
}
