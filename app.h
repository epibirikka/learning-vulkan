#ifndef _APP_H
#define _APP_H

#include <windows.h>
#include <vulkan/vulkan.h>

typedef struct
{
    HWND window;

    VkDebugUtilsMessengerEXT vk_messenger;
    VkInstance vk_instance;
    VkPhysicalDevice vk_device;
} project_app;

HWND app_create_window(project_app *app, HINSTANCE instance, int width, int height);
project_app *app_from_hwnd(HWND window);

void app_graphics_setup(project_app *app);
void app_graphics_cleanup(project_app *app);

void app_update_procedure(HWND window, UINT idk, UINT_PTR idk2, DWORD idk3);
UINT_PTR app_setup_loop(HWND window, project_app *app);

#endif
