#pragma once

#include "cairo/cairo.h"


namespace m2g{

    class Image {
        using ReleaseFun =  void(Image*, void*);
    public:
        Image(int width, int height);
        Image(void* data, int width, int height, int channels, ReleaseFun fun);
        Image(const Image& other);
        ~Image();

        int getWidth() const;
        int getHeight() const;
        int getStride() const;
        int getChannels() const;
        bool isMutable() const;
        void setImmutable();
        void* getPixels() const;
        void getRGB(int* argb, int dataLength, int offset, int scanLength, int x_, int y_, int width_,
                    int height_) const;

        cairo_surface_t* getCairoSurface() const;



        int getFormat();

    private:
        int width_;
        int height_;
        int stride_;
        int channels_;
        bool mut_;
        void* data_;
        ReleaseFun* fun_;
        cairo_surface_t* surface_;
    };
}





