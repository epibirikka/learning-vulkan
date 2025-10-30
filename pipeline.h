#ifndef _GRAPHICS_PIPELINE_H
#define _GRAPHICS_PIPELINE_H

/* technical debt: our app struct is congested
 * so these are just functions */

/* the rainbow triangle pipeline */

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "shader.h"
#include "app.h"

int pipeline_create_render_pass(project_app *app);
int pipeline_assemble_shaders(project_app *app, shader_object *vertex, shader_object *fragment);

#endif
