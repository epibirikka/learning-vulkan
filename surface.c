#define VK_USE_PLATFORM_WIN32_KHR
#include "app.h"

#include <stdio.h>
#include <vulkan/vulkan.h>

int app_graphics_create_surface(project_app *app)
{
    if (app == NULL) return 0;

    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hwnd = app->window;
    create_info.hinstance = app->hinstance;

    if (vkCreateWin32SurfaceKHR(app->vk_instance, &create_info, NULL, &app->surface) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: failed to connect win32 surface (hwnd=%p, hinstance=%p)\n", __func__, 
                app->window, app->hinstance);
        return 0;
    }
    
    return 1;
}
