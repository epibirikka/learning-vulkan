#ifndef MESSAGE_DEBUGGER_H
#define MESSAGE_DEBUGGER_H

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include "app.h"

void debugger_populate(VkDebugUtilsMessengerCreateInfoEXT *info);
void debugger_setup(project_app *app);
void debugger_free(project_app *app);

#endif
