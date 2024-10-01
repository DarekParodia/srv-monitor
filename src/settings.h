#pragma once

#include <string>
#include <cstdint>
#include <iostream>

// look for comments in .cpp file
namespace settings
{
    extern std::string program_name;
    extern std::string dri_device;
    extern bool verbose;
    extern bool debug;
    extern bool no_confirm;
    extern std::ostream *output_stream;
}