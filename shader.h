#ifndef _GRAPHICS_SHADER_H
#define _GRAPHICS_SHADER_H

/* for this demo, we will only get shaders by file name instead of file data. 
 * because it's the quickest way i can do that */

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

typedef struct
{
    char *data;
    size_t data_size;

    VkDevice logical;
    VkShaderModule module;
} shader_object;

void shader_object_load_from_file(VkDevice logical, shader_object *shader, char *filename);
void shader_object_free_data(shader_object *shader);

#endif
