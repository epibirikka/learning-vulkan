#include "debugger.h"
#include "app.h"
#include "validation.h"
#include "extension.h"

#include <vulkan/vulkan.h>
#include <stdio.h>

/* */

static VKAPI_ATTR VkBool32 VKAPI_CALL debugger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagBitsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *user_data
)
{
    fprintf(stderr, "[%s] validation layer: %s\n", __func__, callback_data->pMessage);
    return VK_FALSE;
}

static void debugger_populate_info(VkDebugUtilsMessengerCreateInfoEXT *info)
{
    if (info == NULL)
    {
        return;
    }

    info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    info->pfnUserCallback = debugger_callback;
}


void debugger_setup(project_app *app)
{
    if (app == NULL)
    {
        fprintf(stderr, "%s: app passed null\n", __func__);
        return;
    }

    if (app->vk_instance == NULL)
    {
        fprintf(stderr, "%s: app passed no vkinstance\n", __func__);
        return;
    }

    if (!VALIDATION_LAYERS_ENABLED)
    {
        fprintf(stderr, "%s: validation layers are disabled\n", __func__);
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT create_info;
    debugger_populate_info(&create_info);
    
    if (CreateDebugUtilsMessengerEXT(app->vk_instance, &create_info, NULL, &app->vk_messenger) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: failed to create debug messenger\n", __func__);
    }
}
