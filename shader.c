#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

static void create_itself(shader_object *shader, VkDevice logical)
{
    if (shader == NULL) return;
    
    shader->logical = logical;

    if (shader->data == NULL)
    {
        fprintf(stderr, "%s: (%p)->data not allocated\n", __func__, shader);
        return;
    }

    if (shader->data_size <= 0)
    {
        fprintf(stderr, "%s: (%p)->data_size <= 0\n", __func__, shader);
        return;
    }

    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = shader->data_size;
    create_info.pCode = (uint32_t *)(shader->data);

    if (vkCreateShaderModule(logical, &create_info, NULL, &shader->module) != VK_SUCCESS)
    {
        fprintf(stderr, "%s: failed to create shader module for %p\n", __func__, shader);
    }

    fprintf(stdout, "%s: compiled shader module (%p) from shader %p\n", __func__, &shader->module, shader);
}

void shader_object_free_data(shader_object *shader)
{
    if (shader == NULL) return;

    if (shader->data != NULL)
    {
        printf("%s: %p freeing data\n", __func__, shader);
        free(shader->data);
        shader->data = NULL;
    }

    shader->data_size = 0;

    if (shader->module != VK_NULL_HANDLE)
    {
        printf("%s: %p freeing module\n", __func__, shader);
        vkDestroyShaderModule(shader->logical, shader->module, NULL);
    }

    shader->module = VK_NULL_HANDLE;
}

void shader_object_load_from_file(VkDevice logical, shader_object *shader, char *filename)
{
    if (shader == NULL) return;
    if (filename == NULL) return;

    shader->module = VK_NULL_HANDLE;

    FILE *fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        fprintf(stderr, "%s: failed to open file '%s' for shader %p\n", __func__, filename, shader);
        return;
    };

    /* haphazardly get file size */
    size_t size = 0;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    shader->data = calloc(size, sizeof(char));
    shader->data_size = size;

    if (shader->data == NULL)
    {
        fprintf(stderr, "%s: '%s' failed to allocate for shader file data\n", __func__, filename);
        fclose(fp);
        return;
    }

    size_t read = fread(shader->data, sizeof(char), shader->data_size, fp);

    if (read < shader->data_size)
    {
        fprintf(stderr, "%s: '%s' failed to completely read shader file\n", __func__, filename);
        fclose(fp);
        return;
    }

    fclose(fp);

    create_itself(shader, logical);
}
