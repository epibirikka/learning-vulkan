#include "app.h"

#include <windows.h>
#include <stdio.h>

static UINT_PTR IDT_UPDATE = 0x0;

void app_update_procedure(HWND window, UINT idk, UINT_PTR idk2, DWORD idk3)
{
    project_app *app = app_from_hwnd(window);

    if (app == NULL)
    {
        return;
    }
    
    /* TODO */
}

UINT_PTR app_setup_loop(HWND window, project_app *app)
{
    if (window == NULL)
    {
        fprintf(stderr, "%s: window passed null\n", __func__);
        return (UINT_PTR)NULL;
    }

    return SetTimer(window, IDT_UPDATE, 1000, (TIMERPROC)app_update_procedure);
}
