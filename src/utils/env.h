#pragma once
#include <cstdlib>

namespace utils
{
    std::string getEnv(const char *var)
    {
        char *val = std::getenv(var);
        return (val == nullptr ? "" : val);
    }

    bool isDesktop()
    {
        std::string sessionType = getEnv("XDG_SESSION_TYPE");
        return !sessionType.empty();
    }
}