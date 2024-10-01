#include "settings.h"

namespace settings
{
    std::string program_name = "srvmonitor";
    std::string dri_device = "/dev/dri/card1";
    bool verbose = true;
    bool debug = true;
    bool no_confirm = false;
    std::ostream *output_stream = &std::cout;
}