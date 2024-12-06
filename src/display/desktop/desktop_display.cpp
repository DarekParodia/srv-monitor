#define RGFW_IMPLEMENTATION
#define RGFW_BUFFER
#define RGFW_OPENGL
#define RGFW_PRINT_ERRORS

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
        display::desktop::DesktopDisplay *disp = reinterpret_cast<display::desktop::DesktopDisplay *>(win->userPtr);
        disp->setDimensions(r.x, r.y);
        std::cout << "Resized " << r.x << " " << r.y << std::endl;
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
        win = RGFW_createWindow("Srv Monitor", RGFW_RECT(0, 0, this->options.attr.width, this->options.attr.height), RGFW_CENTER | RGFW_NO_RESIZE | RGFW_TRANSPARENT_WINDOW);
        RGFW_setWindowResizeCallback(windowresizefunc);

        win->userPtr = reinterpret_cast<void *>(this);
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
        RGFW_window_checkEvents(win, 100);
    }
    void DesktopDisplay::setDimensions(int width, int height)
    {
        this->options.attr.width = width;
        this->options.attr.height = height;
    }

    bool DesktopDisplay::shouldClose()
    {
        return RGFW_window_shouldClose(win);
    }

    size_t DesktopDisplay::getBufferSize(int index)
    {
        return RGFW_bufferSize.w * RGFW_bufferSize.h * 4;
    }
}