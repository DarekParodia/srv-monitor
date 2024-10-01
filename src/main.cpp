#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "settings.h"
#include "utils/logger.h"
#include "display/display.h"
#include "display/drm/drm_display.h"

display::drm::Options drm_options;
display::DisplayBase *disp;

void parse_args(int argc, char *argv[])
{
    settings::program_name = std::string(argv[0]);
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);
    logger.info("Hello, World!");
    logger.log("This is a log message");

    std::vector<std::string> devices = display::drm::getDrmDevices();
    for (auto &device : devices)
    {
        logger.info("Found device: " + device);
    }
    drm_options.device = devices[0];
    disp = new display::drm::DrmDisplay(drm_options);

    disp->init();
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

    disp->draw(0, data, size);

    // delay for 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}