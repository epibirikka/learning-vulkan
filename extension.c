#include "extension.h"

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
