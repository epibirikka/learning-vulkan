#ifndef _APP_H
#define _APP_H

#define VK_USE_PLATFORM_WIN32_KHR

#include "shader.h"
#include "framebuffer.h"
#include "command.h"

#include <windows.h>
#include <vulkan/vulkan.h>

#include "device.h"

#define APP_FRAMES_INFLIGHT (2)

typedef struct
{
    HWND window;
    HINSTANCE hinstance;

    VkDebugUtilsMessengerEXT vk_messenger;
    VkInstance vk_instance;
    VkPhysicalDevice vk_device;
    VkDevice vk_logical;

    VkQueue vk_queue;
    VkQueue vk_present_queue;

    /* store as graphics context structure? */
    VkSwapchainKHR swapchain;
    VkSurfaceKHR surface;
    VkExtent2D swapchain_extent;
    VkFormat swapchain_format;

    VkImage *images;
    VkImageView *image_views;
    uint32_t image_count;

    gpu_queue_indexes queue_indexes;
    uint32_t graphics_up;

    /* hardcoded shaders */
    struct
    {
        shader_object vertex;
        shader_object fragment;
    } triangle_shader;

    framebuffer_list framebuffers;

    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;

    VkViewport viewport;
    VkRect2D scissor;

    /* i can just put this in a struct but this is the quickest to implement anyway */
    commander commanders[APP_FRAMES_INFLIGHT];

    VkSemaphore semaphores_acquired_image[APP_FRAMES_INFLIGHT];
    VkSemaphore semaphores_render_finished[APP_FRAMES_INFLIGHT];
    VkFence fences_in_flight[APP_FRAMES_INFLIGHT];
    uint32_t current_frame;
    uint32_t has_resized;
} project_app;

typedef struct
{
    uint32_t width;
    uint32_t height;
} window_size;

void app_quit();

HWND app_create_window(project_app *app, HINSTANCE instance, int width, int height);
window_size app_window_size(project_app *app);
project_app *app_from_hwnd(HWND window);

void app_graphics_setup(project_app *app);
void app_graphics_cleanup(project_app *app);
void app_graphics_frame(project_app *app);

int app_graphics_create_surface(project_app *app);
int app_graphics_create_swapchain(project_app *app, uint32_t width, uint32_t height);

void app_update_procedure(HWND window, UINT idk, UINT_PTR idk2, DWORD idk3);
void app_handle_resize(project_app *app, uint32_t width, uint32_t height);
UINT_PTR app_setup_loop(HWND window, project_app *app);


#endif
