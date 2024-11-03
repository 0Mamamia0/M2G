

#include <cstdint>
#include "draw.h"
#include "Image.h"
#include "Graphics.h"
#include "PixelBuffer.h"

namespace m2g {
    Image::Image(std::shared_ptr<PixelBuffer> pixel)
        : Image(std::move(pixel), false) {

    }

    Image::Image(std::shared_ptr<PixelBuffer> pixel, bool isMutable)
            : width(pixel->getWidth())
            , height(pixel->getHeight())
            , mut(isMutable)
            ,pixel(std::move(pixel)){

    }




    int Image::getHeight() const {
        return pixel->getHeight();
    }

    int Image::getWidth() const {
        return pixel->getWidth();
    }

    bool Image::isMutable() const {
        return mut;
    }

    void Image::setImmutable() {
        this->mut = false;
    }

    Image::~Image() = default;

    void Image::getRGB(int* argb, int dataLength, int offset, int scanLength, int x_, int y_, int width_, int height_) const {
        int image_width = getWidth();
        auto* rgba = pixel->addr<Color*>(x_, y_);
        while (height_ > 0) {
            piv::pix_copy<piv::pix_order::ARGB>(argb + offset, (uint8_t*)rgba, width_);
            auto* tmp = rgba;
            for (int i = 0; i < width_; i ++) {
                int value = 0;
                value |= static_cast<int>(tmp->a) << 24;
                value |= static_cast<int>(tmp->r) << 16;
                value |= static_cast<int>(tmp->g) << 8;
                value |= static_cast<int>(tmp->b);
                argb[offset + i] = value;
                tmp ++;
            }
            rgba += image_width;
            offset += scanLength;
            height_--;
        }
    }

    PixelBuffer& Image::getPixelBufferRef() const{
        return *pixel;
    }


    std::shared_ptr<PixelBuffer> Image::getPixelBuffer() const {
        return pixel;
    }


    int Image::getFormat() const {
        return pixel->getFormat();
    }

    bool Image::hasAlpha() const {
        return pixel->hasAlpha();
    }

    bool Image::isColor() const {
        return pixel->isColor();
    }

    void* Image::getPixels() const {
        return pixel->addr<void*>();
    }
}






