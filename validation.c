#include "app.h"
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include "validation.h"

static int has_layer_property(const char *match_name, VkLayerProperties *properties, size_t count)
{
    if (match_name == NULL || properties == NULL || count <= 0)
    {
        fprintf(stderr, "%s: match_name: %p, properties: %p, count: %lld\n", __func__, match_name, properties, count);
        return 0;
    }

    /*
    for (size_t index=0; index<count; ++index)
    {
        printf("property: %s\n", properties[index].layerName);
    }
    */

    for (size_t index=0; index<count; ++index)
    {
        if (!strcmp(match_name, properties[index].layerName))
        {
            printf("%s: found it\n", __func__);
            return 1;
        }
    }

    return 0;
}

int graphics_check_validation_support()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties *properties = calloc(layer_count, sizeof(VkLayerProperties));

    if (properties == NULL)
    {
        fprintf(stderr, "%s: looks like we already ran out of memory :D (supposed to have %d layers)\n", __func__, layer_count);
        return 0;
    }

    vkEnumerateInstanceLayerProperties(&layer_count, properties);

    int result = 0;

    for (const char **string=VALIDATION_LAYERS_LIST; *string != NULL; string++)
    {
        printf("%s testing: %s\n", __func__, *string);

        if (has_layer_property(*string, properties, layer_count))
        {
            result = 1;
            goto done;
        }
    }

done:
    free(properties);
    return result;
}
