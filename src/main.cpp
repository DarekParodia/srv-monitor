#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "settings.h"
#include "utils/logger.h"
#include "utils/env.h"
#include "display/display.h"
#include "display/drm/drm_display.h"
#include "display/desktop/desktop_display.h"

display::drm::Options drm_options;
display::desktop::Options desktop_options;
display::DisplayBase *disp;

void init_drm()
{
    logger.info("initializing drm display");
    std::vector<std::string> devices = display::drm::getDrmDevices();
    for (auto &device : devices)
    {
        logger.info("Found device: " + device);
    }
    drm_options.device = devices[0];
    disp = new display::drm::DrmDisplay(drm_options);
    disp->init();
}

void init_desktop()
{
    logger.info("Initializing desktop display");
    desktop_options.attr = display::Atrributes(600, 400, 24);
    desktop_options.vsync = true;
    desktop_options.title = std::string("Srv Monitor");
    display::desktop::DesktopDisplay *desktop_disp = new display::desktop::DesktopDisplay(desktop_options);

    desktop_disp->init();
    desktop_disp->show();

    disp = desktop_disp;
}

void parse_args(int argc, char *argv[])
{
    settings::program_name = std::string(argv[0]);
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);
    logger.info("Hello, World!");
    logger.log("This is a log message");

    if (utils::isDesktop())
        init_desktop();
    else
        init_drm();

    display::Atrributes attr = disp->displayAttributes(0);
    size_t size = attr.width * attr.height * (attr.depth / 8);

    void *data = malloc(size);

    for (int i = 0; i < size; i += 4)
    {
        ((uint8_t *)data)[i] = (uint8_t)(((double)i / (double)(size)) * 0xff); // blue
        ((uint8_t *)data)[i + 1] = 0;                                          // green
        ((uint8_t *)data)[i + 2] = 0;                                          // red
        ((uint8_t *)data)[i + 3] = 0xff;                                       // alpha
    }

    while (!disp->shouldClose())
    {
        disp->draw(0, data, size);
        disp->update();
    }

    // delay for 5 seconds
    // std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}