#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "settings.h"
#include "utils/logger.h"
#include "display/display.h"
#include "display/drm/drm_display.h"

display::drm::Options drm_options = {
    .device = "/dev/dri/card1",
};

display::DisplayBase *disp = new display::drm::DrmDisplay(drm_options);

void parse_args(int argc, char *argv[])
{
    settings::program_name = std::string(argv[0]);
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);
    logger.info("Hello, World!");
    logger.log("This is a log message");
    disp->init();

    void *data = malloc(1280 * 800 * 4);
    memset(data, 0x3F, 1280 * 800 * 4);
    disp->draw(0, data, 1280 * 800 * 4);

    // delay for 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}