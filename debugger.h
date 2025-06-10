#ifndef MESSAGE_DEBUGGER_H
#define MESSAGE_DEBUGGER_H

#include <vulkan/vulkan.h>
#include "app.h"

void debugger_populate(VkDebugUtilsMessengerCreateInfoEXT *info);
void debugger_setup(project_app *app);

#endif
