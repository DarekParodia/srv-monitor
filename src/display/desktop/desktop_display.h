#pragma once

#include "display/display.h"

#include <string>
#include <GLFW/glfw3.h>

namespace display::desktop
{
    typedef struct Options
    {
        display::Atrributes attr;
        bool vsync;
        std::string title;
    } Options;

    class DesktopDisplay : public display::DisplayBase
    {
    public:
        DesktopDisplay(Options &options);
        ~DesktopDisplay();
        void init() override;
        void draw(int index, const void *buffer, size_t size) override;
        Atrributes displayAttributes(int index) const override;
        void update() override;
        void show();
        void hide();
        static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
        bool shouldClose() override;

    private:
        Options options;
        GLFWwindow *window;
    };
}