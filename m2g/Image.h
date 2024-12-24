#pragma once

#include <cstdint>
#include <memory>

namespace m2g {
    class Graphics;

    class PixelBuffer;

    class Image {
    public:
        explicit Image(std::shared_ptr<PixelBuffer> pixel);

        Image(std::shared_ptr<PixelBuffer> pixel, bool isMutable);

        Image(const Image &other) = delete;

        Image(Image &&other) noexcept = delete;

        ~Image();

        void setImmutable();

        int getFormat() const;

        int getWidth() const;

        int getHeight() const;

        bool isMutable() const;

        bool hasAlpha() const;

        bool isColor() const;

        void *getPixels() const;

        PixelBuffer &getPixelBufferRef() const;

        std::shared_ptr<PixelBuffer> getPixelBuffer() const;

        void
        getRGB(int *data, int dataLength, int offset, int scanLength, int x_, int y_, int width_, int height_) const;

    private:
        int width_;
        int height_;
        bool mut_;
        std::shared_ptr<PixelBuffer> pixel_;
    };
}