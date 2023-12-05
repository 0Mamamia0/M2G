//
// Created by Admin on 2023/5/22.
//

#include "Image.h"

#include <stdint.h>
#include "Graphics.h"
#include "PixelBuffer.h"



Image::Image(std::shared_ptr<PixelBuffer> pixel)
        : Image(pixel, false) {

}

Image::Image(std::shared_ptr<PixelBuffer> pixel, bool isMutable)
        : pixel(pixel)
        , width(pixel->getWidth())
        , height(pixel->getHeight())
        , mut(isMutable){

}


Graphics* Image::getGraphics() {
    if(isMutable()) {
        return new Graphics(pixel);
    }
    return nullptr;
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

Image::~Image() {

}

void Image::getRGB(int* argb, int dataLength, int offset, int scanLength, int x_, int y_, int width_, int height_) {
    int image_width = getWidth();
    auto* rgba = pixel->addr<Color*>(x_, y_);
    while (height_ > 0) {
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

PixelBuffer& Image::getPixelBufferRef() {
    return *pixel.get();
}


std::shared_ptr<PixelBuffer> Image::getPixelBuffer() {
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




