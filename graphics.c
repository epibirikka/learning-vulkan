#include "app.h"

#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>

#include "validation.h"

static VkResult create_vk_instance(VkInstance *instance)
{
    if (instance == NULL)
    {
        fprintf(stderr, "%s: instance ref passed null\n", __func__);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "vulkan_test_program";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info = {};

    /* we're on our own! */
    create_info.enabledExtensionCount = 0;
    create_info.ppEnabledExtensionNames = NULL;

    if (VALIDATION_LAYERS_ENABLED)
    {
        /* UNTESTED */
        create_info.enabledLayerCount = VALIDATION_LAYERS_COUNT;
        create_info.ppEnabledLayerNames = VALIDATION_LAYERS_LIST;
        printf("validation layers enabled\n");
    }
    else
    {
        printf("validation layers disabled\n");
        create_info.enabledLayerCount = 0;
    }

    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    
    return vkCreateInstance(&create_info, NULL, instance);
}

static int are_features_suitable(VkPhysicalDeviceFeatures features)
{
    return 1;
}

static void debug_device(VkPhysicalDevice device)
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

static void debug_devices(VkPhysicalDevice *devices, uint32_t count)
{

    for (uint32_t idx=0; idx<count; ++idx)
    {
        VkPhysicalDevice device = devices[idx];
        fprintf(stdout, "%s: device %d\n", __func__, idx);
        debug_device(device);
    }
}

static VkPhysicalDevice find_suitable_device(VkPhysicalDevice *devices, uint32_t count)
{
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

static int pick_physical_device(project_app *app, VkInstance instance)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);

    if (device_count <= 0)
    {
        fprintf(stderr, "%s: no graphic devices supporting vulkan!\n", __func__);
        return 0;
    }

    VkPhysicalDevice *devices = calloc(device_count, sizeof(VkPhysicalDevice));

    if (devices == NULL)
    {
        fprintf(stderr, "%s: out of memory bitch\n", __func__);
        return 0;
    }

    vkEnumeratePhysicalDevices(instance, &device_count, devices);

    debug_devices(devices, device_count);
    app->vk_device = find_suitable_device(devices, device_count);

    return 1;
}

void app_graphics_setup(project_app *app)
{
    if (app == NULL)
    {
        return;
    }

    VkInstance blank = {};
    app->vk_instance = blank;
    app->vk_device = VK_NULL_HANDLE;

    if (VALIDATION_LAYERS_ENABLED && !graphics_check_validation_support())
    {
        fprintf(stderr, "%s: validation layers are not supported (validation layers enabled)\n", __func__);
        return;
    }

    if (create_vk_instance(&app->vk_instance) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: cannot create VkInstance\n", __func__);
        return;
    }

    if (!pick_physical_device(app, app->vk_instance))
    {
        fprintf(stderr, "%s: failed to pick graphics device\n", __func__);
        return;
    }
    else
    {
        printf("%s: selected device\n", __func__);
        debug_device(app->vk_device);
    }

    /* now we're just gonna say that's it, but in the real world, 
     * we need to be able for the program and the user to automatically configure to their GPU device. 
     * we can score for the best GPU when writing settings for the first time. */

    /* queue GPU families */

    printf("%s: setting up vk success\n", __func__);
}

void app_graphics_cleanup(project_app *app)
{
    if (app == NULL)
    {
        return;
    }

    vkDestroyInstance(app->vk_instance, NULL);
    vkDestroyDevice(app->vk_device, NULL);
}
