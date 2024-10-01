#include <iostream>
#include <string>

#include "settings.h"
#include "utils/logger.h"

void parse_args(int argc, char *argv[])
{
    settings::program_name = std::string(argv[0]);
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);
    logger.info("Hello, World!");
    logger.log("This is a log message");
    return 0;
}