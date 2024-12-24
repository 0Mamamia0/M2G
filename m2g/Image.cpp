

#include <cstdint>
#include <span>
#include "draw/draw.h"
#include "Image.h"
#include "Graphics.h"
#include "PixelBuffer.h"

namespace m2g {
    Image::Image(std::shared_ptr<PixelBuffer> pixel)
        : Image(std::move(pixel), false) {

    }

    Image::Image(std::shared_ptr<PixelBuffer> pixel, bool isMutable)
            : width_(pixel->getWidth())
            , height_(pixel->getHeight())
            , mut_(isMutable)
            , pixel_(std::move(pixel)){
    }

    int Image::getHeight() const {
        return height_;
    }

    int Image::getWidth() const {
        return width_;
    }

    bool Image::isMutable() const {
        return mut_;
    }

    void Image::setImmutable() {
        this->mut_ = false;
    }

    Image::~Image() = default;

    void Image::getRGB(int* argb, int dataLength, int offset, int scanLength, int x, int y, int width, int height) const {
//        int image_width = getWidth();
        int format = getFormat();
        int stride = pixel_->getRowBytes();

        if(format == PixelFormatType::RGB_888X || format == PixelFormatType::RGBA_8888) {
            auto* rgba = pixel_->addr<Color*>(x, y);
            piv::pix_copy_rect_order<pix_order::ARGB, pix_order::RGBA>((uint32_t*)argb + offset, (uint8_t*)rgba, scanLength * 4, stride, width, height);
        } else if(format == PixelFormatType::GREY) {
            auto* src = pixel_->addr<uint8_t*>(x, y);
            auto* dst = argb + offset;
            while (height-- > 0) {
                for (int i = 0; i < width; ++i) {
                    uint8_t gray = src[i];
                    dst[i] = 0xFF << 24 | gray << 16 | gray << 8 | gray;
                }
                src += stride;
                dst += scanLength;
            }

        } else if(format == PixelFormatType::GREY_ALPHA) {
            auto* src = pixel_->addr<uint8_t*>(x, y);
            auto* dst = argb + offset;
            while (height-- > 0) {
                for (int i = 0; i < width; ++i) {
                    uint8_t gray = src[i * 2];
                    uint8_t alpha = src[i * 2 + 1];
                    dst[i] = alpha << 24 | gray << 16 | gray << 8 | gray;
                }
                src += stride;
                dst += scanLength;
            }
        }


    }

    PixelBuffer& Image::getPixelBufferRef() const {
        return *pixel_;
    }


    std::shared_ptr<PixelBuffer> Image::getPixelBuffer() const {
        return pixel_;
    }

    int Image::getFormat() const {
        return pixel_->getFormat();
    }

    bool Image::hasAlpha() const {
        return pixel_->hasAlpha();
    }

    bool Image::isColor() const {
        return pixel_->isColor();
    }

    void* Image::getPixels() const {
        return pixel_->addr<void*>();
    }
}






