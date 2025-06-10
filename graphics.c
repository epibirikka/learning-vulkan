#include "app.h"
#include "device.h"

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

    gpu_debug_devices(devices, device_count);
    app->vk_device = gpu_find_suitable_device(devices, device_count);

    free(devices); /* TODO: is this safe? */
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
        gpu_debug_device(app->vk_device);
    }

    /* TODO: queue families and pick logical device */

    /*
    if ()
    {
    }
    */

    /* TODO: link device to window? */

    printf("%s: setting up vk success\n", __func__);
}

void app_graphics_cleanup(project_app *app)
{
    if (app == NULL)
    {
        return;
    }

    vkDestroyInstance(app->vk_instance, NULL);
    // vkDestroyDevice(logical device, NULL);
}
