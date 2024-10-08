#pragma once

#include <cstddef>

namespace display
{
    /**
     * @brief Attributes of the display
     *
     * This struct contains the attributes of the display such as width, height, and color depth.
     *
     */
    struct Atrributes
    {
        int width;  /**< Width of the display in pixels */
        int height; /**< Height of the display in pixels  */
        int depth;  /**< Color depth of the display in number of bits */
    };

    class DisplayBase
    {
    public:
        /**
         *
         * @brief Initialize the display
         *
         * This function should be called before any other functions in this class.
         */
        virtual void init() = 0;

        /**
         *
         * @brief Draw a pixel buffer to the display
         *
         * @param index The index of the display to draw to
         * @param buffer The pixel buffer to draw
         * @param size The size of the pixel buffer in bytes
         */
        virtual void draw(int index, const void *buffer, size_t size) = 0;

        /**
         *
         * @brief Get attributes of the display
         * @param index The index of the display to get attributes for
         * @return The attributes of the display
         */
        virtual Atrributes displayAttributes(int index) const = 0;
    };
}