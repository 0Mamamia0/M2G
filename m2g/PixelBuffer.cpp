﻿//


#include "PixelBuffer.h"

#include <memory>
#include <cstring>
#include "PixelFormat.h"

static void Release(m2g::PixelBuffer* buffer, void* pixels) {
    delete[] static_cast<uint8_t *>(pixels);
}

namespace m2g {
    std::shared_ptr<PixelBuffer> PixelBuffer::allocate(int width, int height, int format) {
        int bpp = PixelFormat::getBytePerPixel(format);
        auto* data = new uint8_t[width * height * bpp];
        return std::make_shared<PixelBuffer>(data, width, height, format, Release);
    }

    std::shared_ptr<PixelBuffer> PixelBuffer::wrap(uint8_t* data, int width, int height, int format, ReleaseFun* fun) {
        return std::make_shared<PixelBuffer>(data, width, height, format, fun);
    }

    std::shared_ptr<PixelBuffer> PixelBuffer::wrap(uint8_t* data, int width, int height, int format, int rowByte,
                                                   PixelBuffer::ReleaseFun* fun) {
        return std::make_shared<PixelBuffer>(data, width, height, format, rowByte, fun);
    }


    PixelBuffer::PixelBuffer(uint8_t* pixels, int width, int height, int format, ReleaseFun* fun)
        : PixelBuffer(pixels, width, height, format, width * PixelFormat::getBytePerPixel(format), fun) {
    }

    PixelBuffer::PixelBuffer(uint8_t* pixels, int width, int height, int format, int rowByte,
                             ReleaseFun* fun)
        : pixel(pixels)
          , fun(fun)
          , width(width)
          , height(height)
          , format(format)
          , bpp(PixelFormat::getBytePerPixel(format))
          , spp(PixelFormat::shiftPerPixel(format))
          , stride(width)
          , rowByte(width * bpp)
          , paddedRowByte(rowByte)
          , size(height * paddedRowByte) {
    }

    PixelBuffer::PixelBuffer(const PixelBuffer& other)
        : PixelBuffer(nullptr, other.width, other.height, other.format, nullptr) {
        auto* data = new uint8_t[size];
        std::memcpy(data, other.pixel, size);
        this->pixel = data;
        this->fun = &Release;
    }


    PixelBuffer::PixelBuffer(PixelBuffer&& other) noexcept
        : PixelBuffer(other.pixel, other.width, other.height, other.format, other.fun) {
        other.pixel = nullptr;
        other.fun = nullptr;
    }


    PixelBuffer::~PixelBuffer() {
        if (this->fun != nullptr) {
            fun(this, pixel);
        }
    }

    int PixelBuffer::getWidth() const {
        return width;
    }

    int PixelBuffer::getHeight() const {
        return height;
    }

    int PixelBuffer::getBytePerPixel() const {
        return bpp;
    }

    int PixelBuffer::getShiftPerPixel() const {
        return spp;
    }

    int PixelBuffer::getFormat() const {
        return format;
    }

    int PixelBuffer::getStride() const {
        return stride;
    }

    int PixelBuffer::getRowBytes() const {
        return rowByte;
    }

    int PixelBuffer::getPaddedRowByte() const {
        return paddedRowByte;
    }

    void PixelBuffer::erase(int val) {
        memset(pixel, val, size);
    }

    bool PixelBuffer::hasAlpha() const {
        switch (format) {
            case GREY_ALPHA:
            case ALPHA_8:
            case RGBA_8888:
                return true;
            case GREY:
            case RGB_888X:
            default:
                return false;
        }
    }

    bool PixelBuffer::isColor() const {
        switch (format) {
            case RGB_888X:
            case RGBA_8888:
                return true;
            case GREY_ALPHA:
            case ALPHA_8:
            case GREY:
            default:
                return false;
        }
    }
}
