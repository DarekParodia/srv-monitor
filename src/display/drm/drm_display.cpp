#include "drm_display.h"

namespace display::drm
{
    struct DrmResouces
    {
        drmModeRes *res;
        DrmResouces(int fd) { res = drmModeGetResources(fd); }
        ~DrmResouces() { drmModeFreeResources(res); }
    };

    struct DrmConnector
    {
        drmModeConnector *conn;
        DrmConnector(int fd, uint32_t c) { conn = drmModeGetConnector(fd, c); }
        ~DrmConnector() { drmModeFreeConnector(conn); }
    };

    DrmDevice::DrmDevice(int fd,
                         drmModeConnector *conn)
        : _fd(fd), _frontBuf(0), _saved_crtc(nullptr)
    {
        _conn = conn->connector_id;
        memcpy(&_mode, &conn->modes[0], sizeof(_mode));
        _bufs[0].width = conn->modes[0].hdisplay;
        _bufs[0].height = conn->modes[0].vdisplay;
        _bufs[1].width = conn->modes[0].hdisplay;
        _bufs[1].height = conn->modes[0].vdisplay;

        auto enc = drmModeGetEncoder(fd, conn->encoder_id);
        _crtc = enc->crtc_id;
        drmModeFreeEncoder(enc);

        createBuffer(fd, _bufs[0]);
        createBuffer(fd, _bufs[1]);

        _saved_crtc = drmModeGetCrtc(fd, _crtc);
        drmModeSetCrtc(_fd, _crtc, _bufs[0].fb, 0, 0, &_conn, 1, &_mode);
    }

    DrmDevice::~DrmDevice()
    {
        if (!_saved_crtc)
        {
            drmModeSetCrtc(_fd, _saved_crtc->crtc_id, _saved_crtc->buffer_id,
                           _saved_crtc->x, _saved_crtc->y, &_conn, 1,
                           &_saved_crtc->mode);
            drmModeFreeCrtc(_saved_crtc);
            destroyBuffer(_fd, _bufs[1]);
            destroyBuffer(_fd, _bufs[0]);
        }
    }

    DisplayAtrributes DrmDevice::displayAttributes() const
    {
        DisplayAtrributes attr;
        attr.width = _bufs[0].width;
        attr.height = _bufs[0].height;
        // Note: The 32-bit color depth is assumed
        attr.depth = 32;
        return attr;
    }

    void DrmDevice::draw(const void *data, size_t size)
    {
        DrmBuffer *buf = &_bufs[_frontBuf ^ 1];
        std::memcpy(buf->map, data, std::min((size_t)buf->size, size));
        drmModeSetCrtc(_fd, _crtc, buf->fb, 0, 0, &_conn, 1, &_mode);
        _frontBuf ^= 1;
    }

    void DrmDevice::createBuffer(int fd, DrmBuffer &buf)
    {
        struct drm_mode_create_dumb creq;
        struct drm_mode_map_dumb mreq;

        memset(&creq, 0, sizeof(creq));
        creq.width = buf.width;
        creq.height = buf.height;
        creq.bpp = 32;
        drmIoctl(_fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
        buf.stride = creq.pitch;
        buf.size = creq.size;
        buf.handle = creq.handle;

        /* create framebuffer object for the dumb-buffer */
        drmModeAddFB(fd, buf.width, buf.height, 24, 32, buf.stride, buf.handle,
                     &(buf.fb));

        memset(&mreq, 0, sizeof(mreq));
        mreq.handle = buf.handle;
        drmIoctl(_fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);

        buf.map = (uint8_t *)mmap(0, buf.size, PROT_READ | PROT_WRITE, MAP_SHARED,
                                  _fd, mreq.offset);

        memset(buf.map, 0, buf.size);
    }

    void DrmDevice::destroyBuffer(int fd, DrmBuffer &buf)
    {
        struct drm_mode_destroy_dumb dreq;

        munmap(buf.map, buf.size);
        drmModeRmFB(fd, buf.fb);

        memset(&dreq, 0, sizeof(dreq));
        dreq.handle = buf.handle;
        drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    }

    // display

    DrmDisplay::DrmDisplay(Options &options)
    {
        _options = options;
    }

    DrmDisplay::~DrmDisplay()
    {
        for (auto &dev : _dev)
        {
            dev.reset();
        }
        _dev.clear();
        close(_fd);
    }

    int DrmDisplay::displayCount() const
    {
        return _dev.size();
    }

    DisplayAtrributes DrmDisplay::displayAttributes(int displayNum) const
    {
        DisplayAtrributes attr{0, 0, 0};
        if (displayNum >= 0 && displayNum < _dev.size())
        {
            attr = _dev[displayNum]->displayAttributes();
        }
        return attr;
    }

    void DrmDisplay::init()
    {
        logger.info("Initializing DRM display");

        _fd = open(_options.device.c_str(), O_RDWR | O_CLOEXEC);
        if (_fd < 0)
        {
            logger.error("Failed to open device");
            return;
        }

        DrmResouces res(_fd);
        _dev.reserve(res.res->count_crtcs);
        for (int i = 0; i < res.res->count_connectors; ++i)
        {
            DrmConnector conn(_fd, res.res->connectors[i]);
            if ((conn.conn->connection != DRM_MODE_CONNECTED) ||
                (conn.conn->count_modes == 0))
            {
                continue;
            }
            else
            {
                std::shared_ptr<DrmDevice> dev =
                    std::make_shared<DrmDevice>(_fd, conn.conn);
                _dev.push_back(dev);
            }
        }
    }

    void DrmDisplay::draw(int index, const void *buffer, size_t size)
    {
        if (index >= 0 && index < _dev.size())
        {
            _dev[index]->draw(buffer, size);
        }
    }
} // namespace display::drm