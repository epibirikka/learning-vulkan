#ifndef VALIDATION_SUPPORT_H
#define VALIDATION_SUPPORT_H

static const int VALIDATION_LAYERS_ENABLED = 1;

static const char *VALIDATION_LAYERS_LIST[] = {
    "VK_LAYER_KHRONOS_validation",
    NULL
};

#define VALIDATION_LAYERS_COUNT ((sizeof(VALIDATION_LAYERS_LIST)/sizeof(VALIDATION_LAYERS_LIST[0])) - 1)

int graphics_check_validation_support();

#endif
