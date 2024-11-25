#define RGFW_IMPLEMENTATION
#define RGFW_BUFFER
#define RGFW_OPENGL

#include <iostream>
#include <functional>
#include <RGFW.h>

#include "desktop_display.h"

namespace display::desktop
{
    // sadly window has to be static because i cannot use callbacks with pointers to this object :(
    RGFW_window *win;

    // idk this kind of doesn't work (RGFW's fault)
    void windowresizefunc(RGFW_window *window, RGFW_rect r)
    {
        if (win != window)
            return;
        // std::cout << "Resized " << r.x << " " << r.y << std::endl;
    }

    DesktopDisplay::DesktopDisplay(Options &options)
    {
        this->options = options;
    }

    DesktopDisplay::~DesktopDisplay()
    {
        RGFW_window_close(win);
    }

    void DesktopDisplay::init()
    {
        win = RGFW_createWindow("Srv Monitor", RGFW_RECT(0, 0, this->options.attr.width, this->options.attr.height), RGFW_CENTER | RGFW_TRANSPARENT_WINDOW);
        RGFW_setWindowResizeCallback(windowresizefunc);

        this->options.attr.height = win->r.h;
    }

    void DesktopDisplay::draw(int index, const void *buffer, size_t size)
    {
        memcpy(win->buffer, buffer, size);
    }

    Atrributes DesktopDisplay::displayAttributes(int index) const
    {
        return this->options.attr;
    }

    void DesktopDisplay::update()
    {
        RGFW_window_setGPURender(win, 0);
        RGFW_window_swapBuffers(win);
        RGFW_window_checkFPS(win, 60);
    }

    bool DesktopDisplay::shouldClose()
    {
        while (RGFW_window_checkEvent(win))
        {
            if (win->event.type == RGFW_quit || RGFW_isPressed(win, RGFW_Escape))
            {
                return true;
            }
        }
        return false;
    }
}