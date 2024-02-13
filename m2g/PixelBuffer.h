#ifndef M2G_PIXELBUFFER_H
#define M2G_PIXELBUFFER_H

#include <cstdint>
#include <memory>

namespace m2g {
    class PixelBuffer {
        using ReleaseFun =  void(PixelBuffer*, void*);

    public:
        static std::shared_ptr<PixelBuffer> allocate(int width, int height, int format);
        static std::shared_ptr<PixelBuffer> wrap(uint8_t *data, int width, int height, int format, ReleaseFun* fun);
        static std::shared_ptr<PixelBuffer> wrap(uint8_t *data, int width, int height, int format, int rowByte, ReleaseFun* fun);

        PixelBuffer(const PixelBuffer &other);
        PixelBuffer(PixelBuffer &&other) noexcept;
        PixelBuffer(uint8_t* pixels, int width, int height, int format, ReleaseFun* fun);
        PixelBuffer(uint8_t* pixels, int width, int height, int format, int rowBytes, ReleaseFun* fun);
        ~PixelBuffer();

        int getWidth() const;
        int getHeight() const;
        int getFormat() const;
        int getBytePerPixel() const;
        int getShiftPerPixel() const;
        int getStride() const;
        int getRowBytes() const;
        int getPaddedRowByte() const;
        void erase(int i);
        bool hasAlpha() const;
        bool isColor() const;


        template<typename T>
        T addr() {
            return reinterpret_cast<T>(pixel);
        }

        template<typename T>
        T addr(int x, int y) {
            return reinterpret_cast<T>(pixel + (y * paddedRowByte) + (x << spp));
        }


    private:
        uint8_t *pixel;
        ReleaseFun *fun;
        int width;
        int height;
        int format;
        int bpp;
        int spp;
        int stride;
        int rowByte;
        int paddedRowByte;
        size_t size;
    };
}




#endif //M2G_PIXELBUFFER_H
