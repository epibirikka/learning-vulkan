#ifndef _EXTENSION_DEFINITION_H
#define _EXTENSION_DEFINITION_H

#include <vulkan/vulkan.h>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
                                      const VkDebugUtilsMessengerCreateInfoEXT *create_info,
                                      const VkAllocationCallbacks *allocator,
                                      VkDebugUtilsMessengerEXT *debug_messenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
                                       VkDebugUtilsMessengerEXT debug_messenger,
                                       const VkAllocationCallbacks *allocator);

#endif
