#include "app.h"

#include <windows.h>

window_size app_window_size(project_app *app)
{
    window_size size = {};

    if (app == NULL) return size;
    if (app->window == NULL) return size;

    RECT window_rect = {};
    GetWindowRect(app->window, &window_rect);

    size.width = window_rect.right - window_rect.left;
    size.height = window_rect.bottom - window_rect.top;
    return size;
}

void app_quit()
{
    PostQuitMessage(0);
}
