#pragma once

#include <cstddef>

namespace display
{
    class DisplayBase
    {
    public:
        virtual void init() = 0;
        virtual void draw(int index, const void *buffer, size_t size) = 0;
    };
}