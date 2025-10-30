#ifndef _EXTENSION_DEFINITION_H
#define _EXTENSION_DEFINITION_H

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

void extension_instance_create_info_supplicate(VkInstanceCreateInfo *create_info);
void extension_device_create_info_supplicate(VkDeviceCreateInfo *create_info);

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
                                      const VkDebugUtilsMessengerCreateInfoEXT *create_info,
                                      const VkAllocationCallbacks *allocator,
                                      VkDebugUtilsMessengerEXT *debug_messenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
                                       VkDebugUtilsMessengerEXT debug_messenger,
                                       const VkAllocationCallbacks *allocator);

#endif
