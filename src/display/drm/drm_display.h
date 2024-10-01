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
#include <vector>

namespace display::drm
{
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

    /**
     * Options for DrmDisplay class
     */
    struct Options
    {
        std::string device; /**< Path to the DRM device */
    };

    class DrmDevice
    {
    public:
        DrmDevice(int fd, drmModeConnector *conn);
        ~DrmDevice();

        Atrributes displayAttributes() const;
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

    /**
     * @brief DrmDisplay class
     *
     * This class is used to display content on a DRM device
     */
    class DrmDisplay : public display::DisplayBase
    {
    public:
        DrmDisplay(Options &options);
        ~DrmDisplay();
        void init() override;
        void draw(int index, const void *buffer, size_t size) override;
        int displayCount() const;
        Atrributes displayAttributes(int index) const override;

    private:
        int _fd;
        std::vector<std::shared_ptr<DrmDevice>> _dev;
        Options _options;
    };

    /**
     * Get a list of DRM devices
     *
     * @return Vector of DRM devices path strings
     */
    std::vector<std::string> getDrmDevices();
}