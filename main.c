#include <stdio.h>

#include "app.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous_instance, 
                   PSTR cmdline, int cmdshow)
{
    project_app app = {};

    app.window = app_create_window(&app, instance, 800, 600);

    if (app.window == NULL)
    {
        fprintf(stderr, "%s: window failed to create; exiting application\n", __func__);
        return 1;
    }

    ShowWindow(app.window, cmdshow);
    app_graphics_setup(&app);
    app_setup_loop(app.window, &app);

    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    app_graphics_cleanup(&app);

    return 0;
}
