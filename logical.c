#include "logical.h"
#include "validation.h"
#include "extension.h"

#include <stdio.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <stdio.h>

VkResult logical_create(project_app *app)
{
    if (app == NULL) return VK_INCOMPLETE;
    if (!gpu_queue_indexes_complete(&app->queue_indexes)) return VK_INCOMPLETE;

    float _priorities[1] = {1.0f};

    /* overcomplicated bunch of shit */

    uint32_t indexes[2] = {app->queue_indexes.graphics, app->queue_indexes.present};
    VkDeviceQueueCreateInfo q_infos[2] = {};
    int count = 0;

    /* I think we will only need two of these ^^^ */

    for (count=0; count<2; ++count)
    {
        VkDeviceQueueCreateInfo q_info = {};
        q_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        q_info.queueFamilyIndex = indexes[count];
        q_info.queueCount = 1;
        q_info.pQueuePriorities = _priorities;

        q_infos[count] = q_info;

        if (indexes[0] == indexes[1])
        {
            count++; /* fencepost instincts */
            break;
        }
    }

    VkPhysicalDeviceFeatures features = {};
    /* TODO: come back to this later*/


    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = q_infos;
    create_info.queueCreateInfoCount = count;
    create_info.pEnabledFeatures = &features;

    extension_device_create_info_supplicate(&create_info);

    if (VALIDATION_LAYERS_ENABLED)
    {
        create_info.enabledLayerCount = VALIDATION_LAYERS_COUNT;
        create_info.ppEnabledLayerNames = VALIDATION_LAYERS_LIST;
        printf("%s: validation layers enabled\n", __func__);
    }
    else
    {
        printf("%s: validation layers disabled\n", __func__);
        create_info.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(app->vk_device, &create_info, NULL, &(app->vk_logical));

    if (result == VK_SUCCESS) 
    {
        vkGetDeviceQueue(app->vk_logical, indexes[0], 0, &app->vk_queue);
        vkGetDeviceQueue(app->vk_logical, indexes[1], 0, &app->vk_present_queue);
        printf("%s: app->vk_queue and app->vk_present_queue is set\n", __func__);
    }

    return result;
}
