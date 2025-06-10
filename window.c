#include "app.h"

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>

static LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM word_param, LPARAM long_param);

HWND app_create_window(project_app *app, HINSTANCE instance, int width, int height)
{
    if (app == NULL)
    {
        fprintf(stderr, "%s: app passed null\n", __func__);
    }

    if (instance == NULL)
    {
        fprintf(stderr, "%s: instance passed null\n", __func__);
        return NULL;
    }

    WNDCLASS class = {};
    static const LPCSTR CLASS_NAME = "vulkan_test_program";

    class.hInstance = instance;
    class.lpfnWndProc = window_procedure;
    class.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&class))
    {
        fprintf(stderr, "%s: failed to register class: %p\n", __func__, &class);
        return NULL;
    }

    HWND window = CreateWindowEx(
        0,
        CLASS_NAME, "",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        NULL,
        NULL,
        instance,
        NULL /* keep things simple */
    );

    if (window != NULL)
    {
        SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)app);
    }

    return window;
}

project_app *app_from_hwnd(HWND window)
{
    LONG_PTR ptr = GetWindowLongPtr(window, GWLP_USERDATA);
    return (project_app *)ptr;
}

static LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM word_param, LPARAM long_param)
{
    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_LBUTTONDOWN:
        {
            POINT pt;
            pt.x = GET_X_LPARAM(long_param);
            pt.y = GET_Y_LPARAM(long_param);
            
            printf("%s: lbuttondown @ %ld, %ld\n", __func__, pt.x, pt.y);
        }
            return 0;
    }

    return DefWindowProc(window, message, word_param, long_param);
}
