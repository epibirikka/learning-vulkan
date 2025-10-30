#include "extension.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <stdio.h>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
                                      const VkDebugUtilsMessengerCreateInfoEXT *create_info,
                                      const VkAllocationCallbacks *allocator,
                                      VkDebugUtilsMessengerEXT *debug_messenger)
{
    PFN_vkVoidFunction function = vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    printf("%s: function is %p\n", __func__, function);

    if (function == NULL)
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    return ((PFN_vkCreateDebugUtilsMessengerEXT)(function))(instance, create_info, allocator, debug_messenger);
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
                                       VkDebugUtilsMessengerEXT debug_messenger,
                                       const VkAllocationCallbacks *allocator)
{
    PFN_vkVoidFunction function = vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    printf("%s: function is %p\n", __func__, function);

    if (function != NULL)
    {
        ((PFN_vkDestroyDebugUtilsMessengerEXT)(function))(instance, debug_messenger, allocator);
    }
}


static const char *extension_list[] = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface"
};

static const char *device_extension_list[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

static const size_t extension_count = ((sizeof(extension_list)/sizeof(extension_list[0])));
static const size_t device_extension_count = ((sizeof(device_extension_list)/sizeof(device_extension_list[0])));

void extension_instance_create_info_supplicate(VkInstanceCreateInfo *create_info)
{
    if (create_info == NULL) return;

    create_info->enabledExtensionCount = extension_count;
    create_info->ppEnabledExtensionNames = (extension_count <= 0) ? NULL : extension_list;
    printf("%s: extension count: %d, extension names: %p\n", __func__, 
           create_info->enabledExtensionCount, create_info->ppEnabledExtensionNames);
}

void extension_device_create_info_supplicate(VkDeviceCreateInfo *create_info)
{
    if (create_info == NULL) return;

    create_info->enabledExtensionCount = device_extension_count;
    create_info->ppEnabledExtensionNames = (device_extension_count <= 0) ? NULL : device_extension_list;
    printf("%s: extension count: %d, extension names: %p\n", __func__, 
           create_info->enabledExtensionCount, create_info->ppEnabledExtensionNames);
}
