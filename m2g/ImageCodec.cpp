
#include "ImageCodec.h"

#include "stb_image.h"
#include "m2g-def.h"
#include "Image.h"
#include "PixelFormat.h"
#include "PixelBuffer.h"
#include "pixel.h"
#include "STB_Decoder.h"





namespace m2g {
    Image* ImageCodec::loadImage(const char* filename) {
        auto pixel = STB_Decoder::decode(filename);
        if (pixel) {
            return new Image(pixel, false);
        }
        return nullptr;
    }


    Image* ImageCodec::createImage(Image* src) {
        if (!src->isMutable()) {
            auto* result = new Image(src->getPixelBuffer(), false);
            return result;
        }

        auto& src_pixel = src->getPixelBufferRef();

        int width = src->getWidth();
        int height = src->getHeight();
        int format = src->getFormat();

        auto dst_pixel = PixelBuffer::allocate(width, height, format);
        src_pixel.copyTo(*dst_pixel);

        auto* result = new Image(std::move(dst_pixel), false);
        return result;
    }

    Image* ImageCodec::loadImage(uint8_t* data, int offset, int length) {
        auto pixel = STB_Decoder::decode(data + offset, length);
        if (pixel) {
            return new Image(pixel, false);
        }
        return nullptr;
    }

    Image* ImageCodec::createRGBImage(int32_t* argbData, int width, int height, bool processAlpha) {
//        Image* image = createEmptyImage(width, height, false);
        try {

            int format = processAlpha ? PixelFormatType::RGBA_8888 : PixelFormatType::RGB_888X;
            auto pixel = PixelBuffer::allocate(width, height, format);
            auto* pixels = pixel->addr<uint8_t *>();
            int pixelsCount = width * height;
            while (pixelsCount-- > 0) {
                int32_t argb = *argbData++;
                *pixels++ = argb >> 16 & 0xFF;
                *pixels++ = argb >> 8 & 0xFF;
                *pixels++ = argb & 0xFF;
                if (!processAlpha) {
                    *pixels++ = 0xFF;
                }
                else {
                    *pixels++ = argb >> 24 & 0xFF;
                }
            }
            return new Image(std::move(pixel), false);
        }
        catch (const std::bad_alloc& e) {
            return nullptr;
        }
    }

    Image* ImageCodec::createEmptyImage(int width, int height, bool isMutable) {
        if (width <= 0 || height <= 0) {
            return nullptr;
        }
        try {
            auto pixel = PixelBuffer::allocate(width, height, PixelFormatType::RGBA_8888);
            return new Image(std::move(pixel), isMutable);
        }
        catch (const std::bad_alloc&e) {
            return nullptr;
        }
    }

    Image* ImageCodec::createImage(int width, int height) {
        Image* image = createEmptyImage(width, height, true);
        if (image != nullptr) {
            PixelBuffer &pixel = image->getPixelBufferRef();
            pixel.erase(0xFF);
        }
        return image;
    }



    Image* ImageCodec::createImage(Image* src, int x, int y, int width, int height, int transform) {
        if ((transform & ~Transform::TRANSFORM_MASK) != 0) {
            return nullptr;
        }

        int dst_width, dst_height;

        if (transform & TRANSFORM_INVERTED_AXES) {
            dst_width = height;
            dst_height = width;
        }
        else {
            dst_width = width;
            dst_height = height;
        }

        Image* image = createEmptyImage(dst_width, dst_height, false);
        if (image == nullptr) {
            return nullptr;
        }

        auto &dst_buffer = image->getPixelBufferRef();
        auto &src_buffer = src->getPixelBufferRef();
        int dst_stride = dst_buffer.getRowBytes();
        int src_stride = src_buffer.getRowBytes();
        auto* dst_pixels = dst_buffer.addr<uint8_t *>();
        auto* src_pixels = src_buffer.addr<uint8_t *>(x, y);

        ::transform(dst_pixels, src_pixels, dst_stride, src_stride, width, height, transform);

        return image;

    }
}
