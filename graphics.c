#include "app.h"
#include "device.h"
#include "logical.h"
#include "debugger.h"
#include "extension.h"
#include "shader.h"
#include "pipeline.h"
#include "framebuffer.h"
#include "command.h"
#include "order.h"

#define VK_USE_PLATFORM_WIN32_KHR
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
    extension_instance_create_info_supplicate(&create_info);

    if (VALIDATION_LAYERS_ENABLED)
    {
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
    app->vk_device = gpu_find_suitable_device(app->surface, devices, device_count);

    free(devices); 
    return app->vk_device != VK_NULL_HANDLE;
}

static int create_swapchain_resources(project_app *app)
{
    app->has_resized = 0;

    framebuffer_create(&app->framebuffers, app->image_count, app->vk_logical, app->render_pass, app->image_views, app->swapchain_extent, app->viewport, app->scissor);
    if (app->framebuffers.count <= 0) return 0;

    for (size_t idx=0; idx<APP_FRAMES_INFLIGHT; ++idx)
    {
        commander_setup(app->commanders + idx, app->vk_device, app->vk_logical, app->surface, &app->framebuffers, app->pipeline);
        if (!(app->commanders + idx)->setup) return 0;

        if (!order_create_semaphore(app->vk_logical, app->semaphores_acquired_image + idx)) return 0;
        if (!order_create_semaphore(app->vk_logical, app->semaphores_render_finished + idx)) return 0;
        if (!order_create_fence(app->vk_logical, app->fences_in_flight + idx, 1)) return 0;

        printf("%s: frame resources idx %zu (semaphores: acquired_image=%p render_finished=%p, fence: in_flight: %p)\n", 
               __func__, idx, app->semaphores_acquired_image[idx], app->semaphores_render_finished[idx],
               app->fences_in_flight[idx]);
    }

    return 1;
}

static int create_pipeline(project_app *app)
{
    if (!pipeline_create_render_pass(app)) return 0;

    shader_object_load_from_file(app->vk_logical, &app->triangle_shader.vertex, "triangle.vert.spv");
    shader_object_load_from_file(app->vk_logical, &app->triangle_shader.fragment, "triangle.frag.spv");

    if (!pipeline_assemble_shaders(app, &app->triangle_shader.vertex, &app->triangle_shader.fragment)) return 0;
    if (!create_swapchain_resources(app)) return 0;

    return 1;
}

static void cleanup_swapchain(project_app *app)
{
    for (size_t idx=0; idx<APP_FRAMES_INFLIGHT; ++idx)
    {
        order_destroy_semaphore(app->vk_logical, app->semaphores_acquired_image + idx);
        order_destroy_semaphore(app->vk_logical, app->semaphores_render_finished + idx);
        order_destroy_fence(app->vk_logical, app->fences_in_flight + idx);
        commander_free(app->commanders + idx);
    }

    if (app->images != NULL)
    {
        printf("%s: freeing %d image views and %d images\n", __func__, 
               app->image_count, app->image_count);

        for (size_t idx=0; idx<app->image_count; ++idx)
        {
            vkDestroyImageView(app->vk_logical, app->image_views[idx], NULL);
        }

        free(app->image_views);
        free(app->images);
        app->images = NULL;
        app->image_views = NULL;
    }

    framebuffer_free(&app->framebuffers);
    vkDestroySwapchainKHR(app->vk_logical, app->swapchain, NULL);
}

static void recreate_swapchain(project_app *app)
{
    window_size wsize = app_window_size(app);

    if (wsize.width <= 0 || wsize.height <= 0 || app->swapchain_extent.width <= 0 || app->swapchain_extent.height <= 0)
    {
        /* don't bother */
        fprintf(stderr, "%s: window must be minimized?\n", __func__);
        return;
    }

    vkDeviceWaitIdle(app->vk_logical);
    cleanup_swapchain(app);

    if (!app_graphics_create_swapchain(app, wsize.width, wsize.height))
    {
        fprintf(stderr, "%s: cannot create swapchain\n", __func__);
        app_quit(); /* bye bye */
        return;
    }

    if (!create_swapchain_resources(app))
    {
        app_quit(); /* bye bye */
        return;
    }
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
    app->vk_logical = VK_NULL_HANDLE;
    app->vk_queue = VK_NULL_HANDLE;
    app->surface = VK_NULL_HANDLE;
    app->images = NULL;
    app->image_views = NULL;
    app->current_frame = 0;
    app->has_resized = 0;

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

    debugger_setup(app);

    if (!app_graphics_create_surface(app))
    {
        fprintf(stderr, "%s: cannot connect surface to window\n", __func__);
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

    gpu_queue_indexes indexes = gpu_find_queue_families(app->vk_device, app->surface);

    /* NOTE: we're just going to crash if the queue families aren't suitable */
    if (!gpu_queue_indexes_complete(&indexes))
    {
        fprintf(stderr, "%s: failed to find suitable queue family "
                        "(graphics=%d, present=%d)\n", __func__, 
                        indexes.graphics, indexes.present);
        return;
    }

    app->queue_indexes = indexes;

    if (logical_create(app) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: cannot create logical device\n", __func__);
        return;
    }

    window_size wsize = app_window_size(app);

    if (!app_graphics_create_swapchain(app, wsize.width, wsize.height))
    {
        fprintf(stderr, "%s: cannot create swapchain\n", __func__);
        return;
    }

    if (!create_pipeline(app))
    {
        fprintf(stderr, "%s: cannot create pipeline\n", __func__);
        return;
    }

    /* TODO: framebuffers, command buffers, blah blah blah */

    printf("%s: graphics set up successfully\n", __func__);
    app->graphics_up = 1;
}

void app_graphics_cleanup(project_app *app)
{
    if (app == NULL)
    {
        return;
    }

    /* the destruction process is IN ORDER, WTF */

    printf("%s: ---- CLEANING UP GRAPHICAL RESOURCES ----\n", __func__);

    printf("%s: waiting for the GPU to finish\n", __func__);

    vkDeviceWaitIdle(app->vk_logical);

    cleanup_swapchain(app);

    vkDestroyRenderPass(app->vk_logical, app->render_pass, NULL);
    vkDestroyPipeline(app->vk_logical, app->pipeline, NULL);
    vkDestroyPipelineLayout(app->vk_logical, app->pipeline_layout, NULL);

    shader_object_free_data(&app->triangle_shader.vertex);
    shader_object_free_data(&app->triangle_shader.fragment);

    vkDestroyDevice(app->vk_logical, NULL);
    vkDestroySurfaceKHR(app->vk_instance, app->surface, NULL);
    vkDestroyInstance(app->vk_instance, NULL);

    printf("%s: ---- CLEANING UP GRAPHICAL RESOURCES FINISHED ----\n", __func__);
}

void app_graphics_frame(project_app *app)
{
    if (app == NULL) return;
    
    window_size wsize = app_window_size(app);
    if (wsize.width <= 0 || wsize.height <= 0 || app->swapchain_extent.width <= 0 || app->swapchain_extent.height <= 0) return;

    size_t frame = app->current_frame++;
    app->current_frame %= APP_FRAMES_INFLIGHT;

    /* ew */
    commander *commander = app->commanders + frame;
    VkFence fence_in_flight = app->fences_in_flight[frame];
    VkSemaphore semaphore_acquired_image = app->semaphores_acquired_image[frame];
    VkSemaphore semaphore_render_finished = app->semaphores_render_finished[frame];

    vkWaitForFences(app->vk_logical, 1, &fence_in_flight, VK_TRUE, UINT64_MAX);

    uint32_t image_index;
    VkResult acquire_result;

    acquire_result = vkAcquireNextImageKHR(app->vk_logical, app->swapchain, UINT64_MAX, semaphore_acquired_image, VK_NULL_HANDLE, &image_index);

    if (app->has_resized || acquire_result == VK_SUBOPTIMAL_KHR || acquire_result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate_swapchain(app);
        return;
    }
    else if (acquire_result != VK_SUCCESS)
    {
        fprintf(stderr, "%s: failed to acquire swap chain image\n", __func__);
        app_quit();
        return;
    }

    /* different from fence_in_flight */
    vkResetFences(app->vk_logical, 1, app->fences_in_flight + frame);

    commander_reset(commander);
    commander_start_recording(commander, image_index);

    if (!commander->recorded)
    {
        /* yeah yeah oh well */
        fprintf(stderr, "%s: commander failed to record, exiting\n", __func__);
        app_quit();
        return;
    }

    /* printf("%s: frame resources idx %zu (semaphores: acquired_image=%p render_finished=%p, fence: in_flight: %p)\n", 
           __func__, frame, semaphore_acquired_image, semaphore_render_finished,
           fence_in_flight); */

    commander_submit(commander, app->vk_queue, semaphore_acquired_image, semaphore_render_finished, fence_in_flight);

    if (!commander->submitted)
    {
        /* yeah yeah oh well */
        fprintf(stderr, "%s: commander failed to submit, aborting\n", __func__);
        app_quit();
        return;
    }

    commander_present(commander, image_index, app->swapchain, app->vk_queue, semaphore_render_finished);
}

void app_handle_resize(project_app *app, uint32_t width, uint32_t height)
{
    if (app == NULL) return;
    app->has_resized = 1;
}
