#pragma once

#include "display/display.h"
#include "utils/logger.h"
#include "settings.h"

#include <memory>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <cstring>
#include <sys/mman.h>
#include <xf86drm.h>
#include <fcntl.h>
#include <unistd.h>

namespace display::drm
{
    struct DisplayAtrributes
    {
        // Width of the display in pixels
        int width;
        // Height of the display in pixels
        int height;
        // Color depth of the display in number of bits
        int depth;
    };

    struct DrmBuffer
    {
        uint32_t width;
        uint32_t height;
        uint32_t stride;
        uint32_t size;
        uint32_t handle;
        uint8_t *map;
        uint32_t fb;
    };

    struct Options
    {
        std::string device;
    };

    class DrmDevice
    {
    public:
        DrmDevice(int fd, drmModeConnector *conn);
        ~DrmDevice();

        DisplayAtrributes displayAttributes() const;
        void draw(const void *data, size_t size);

    private:
        int _fd;
        unsigned int _frontBuf;
        DrmBuffer _bufs[2];

        drmModeModeInfo _mode;
        uint32_t _conn;
        uint32_t _crtc;
        drmModeCrtc *_saved_crtc;

        void createBuffer(int fd, DrmBuffer &buf);
        void destroyBuffer(int fd, DrmBuffer &buf);
    };

    class DrmDisplay : public display::DisplayBase
    {
    public:
        DrmDisplay(Options &options);
        ~DrmDisplay();
        void init() override;
        void draw(int index, const void *buffer, size_t size) override;
        int displayCount() const;
        DisplayAtrributes displayAttributes(int displayNum) const;

    private:
        int _fd;
        std::vector<std::shared_ptr<DrmDevice>> _dev;
        Options _options;
    };
}