#pragma once

#include "display/display.h"

#include <string>

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
        bool shouldClose() override;

    private:
        Options options;
    };
}