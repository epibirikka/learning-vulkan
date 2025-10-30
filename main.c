#include <stdio.h>
#include "app.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous_instance, 
                   PSTR cmdline, int cmdshow)
{
    project_app app = {};

    app.hinstance = instance;
    app.window = app_create_window(&app, instance, 800, 600);

    if (app.window == NULL)
    {
        fprintf(stderr, "%s: window failed to create; exiting application\n", __func__);
        return 1;
    }

    SetWindowText(app.window, "rainbow triangle program");

    ShowWindow(app.window, cmdshow);
    app_graphics_setup(&app);

    if (!app.graphics_up)
    {
        fprintf(stderr, "%s: quitting because the graphics weren't setup properly\n", __func__);
        return 1;
    }

    app_setup_loop(app.window, &app);

    MSG msg = {};

    int tick = 0;


    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            /* game tick */

            /* game display */
            app_graphics_frame(&app);
        }
    }

    app_graphics_cleanup(&app);

    return 0;
}
