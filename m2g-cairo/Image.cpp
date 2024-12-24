

#include "Image.h"

#include <iostream>
#include <cstring>
#include <algorithm>


#define R(argb) static_cast<uint8_t>((argb >> 16) & 0xFF)
#define G(argb) static_cast<uint8_t>((argb >> 8) & 0xFF)
#define B(argb) static_cast<uint8_t>(argb & 0xFF)
#define A(argb) static_cast<uint8_t>((argb >> 24) & 0xFF)

namespace m2g {
    Image::Image(int width, int height)
        : width_(width)
        , height_(height)
        , channels_(4)
        , mut_(true)
        , data_(nullptr)
        , fun_(nullptr){
        this->surface_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
        this->stride_ = cairo_image_surface_get_stride(surface_);
    }

    // Image::Image(void* data, int width, int height, ReleaseFun fun)
    //     : width_(width)
    //     , height_(height)
    //     , stride_(width * 4)
    //     , mut_(false)
    //     , data_(data)
    //     , fun_(fun){
    //     this->surface_ = cairo_image_surface_create_for_data(
    //         static_cast<unsigned char *>(data),
    //         CAIRO_FORMAT_ARGB32,
    //         width,
    //         height,
    //         stride_);
    //
    // }


    Image::Image(void* data, int width, int height, int channels, ReleaseFun fun)
        : width_(width)
        , height_(height)
        , channels_(channels)
        , mut_(false)
        , data_(data)
        , fun_(fun)
        {

        cairo_format_t format = channels == 3 ? CAIRO_FORMAT_RGB24 : CAIRO_FORMAT_ARGB32;
        this->stride_ = cairo_format_stride_for_width(format, width);
        this->surface_ = cairo_image_surface_create_for_data(
            static_cast<unsigned char *>(data),
            format,
            width,
            height,
            stride_);
    }


    Image::Image(const Image& other)
        : width_(other.width_)
        , height_(other.height_)
        , stride_(other.stride_)
        , mut_(other.mut_)
        , channels_(other.channels_){

        size_t data_size = stride_ * other.height_;
        this->data_ = malloc(data_size);
        this->surface_ = surface_ = cairo_image_surface_create_for_data(
            (unsigned char*)data_,
            CAIRO_FORMAT_ARGB32,
            width_,
            height_,
            stride_);
        this->fun_ = [](Image* image, void* data) {
          free(data);
        };

        std::memcpy(data_, other.getPixels(), height_ * stride_);
    }

    Image::~Image() {
        if(fun_) {
            fun_(this, data_);
        }
        cairo_surface_destroy(surface_);
    }

    int Image::getWidth() const {
        return width_;
    }

    int Image::getHeight() const {
        return height_;
    }

    int Image::getStride() const {
        return stride_;
    }

    int Image::getChannels() const {
        return channels_;
    }

    bool Image::isMutable() const {
        return mut_;
    }

    void Image::setImmutable() {
        this->mut_ = false;
    }

    void* Image::getPixels() const {
        return data_ ? data_ : cairo_image_surface_get_data(surface_);
    }


    void Image::getRGB(int* argb, int dataLength, int offset, int scanLength, int x_, int y_, int width, int height) const {



        auto pixels = static_cast<const uint8_t*>(getPixels());
        while (height --  > 0) {

            if(channels_ == 4) {
                auto* tmp = static_cast<const uint8_t*>(pixels);
                for (int i = 0; i < width; i ++) {
                    int b = tmp[0];
                    int g = tmp[1];
                    int r = tmp[2];
                    int a = tmp[3];

                    if(a == 0) {
                        argb[offset + i] = 0;
                        continue;
                    }
                    double af =  0xFF / a;
                    b = std::clamp<int>(b*af, 0, 0xFF);
                    g = std::clamp<int>(g*af, 0, 0xFF);
                    r = std::clamp<int>(r*af, 0, 0xFF);

                    argb[offset + i] = a << 24 | r << 16  | g << 8 | b;
                    tmp += 4;
                }
            } else if(channels_ == 3) {
                auto* tmp = reinterpret_cast<const uint32_t*>(pixels);
                for (int i = 0; i < width; i ++) {
                    argb[offset + i] = (*tmp++) & 0xFF000000;
                }
            }

            pixels += stride_;
            offset += scanLength;
        }
    }

    cairo_surface_t* Image::getCairoSurface() const {
        return surface_;
    }

    int Image::getFormat() {
        return 6; // BGRA
    }
}


