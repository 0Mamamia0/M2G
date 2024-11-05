
#include "ImageCodec.h"


#include <cstring>
#include <cassert>
#include "stb_image.h"
#include "m2g-def.h"
#include "Image.h"
#include "PixelFormat.h"
#include "PixelBuffer.h"


static int stbChannelsToFormat(int channels) {
    switch (channels) {
        case 1: return GREY;
        case 2: return GREY_ALPHA;
        case 3: return RGB_888X;
        case 4: return RGBA_8888;
        default: return -1;
    }
}

static void STB_ImageReleaseFun(m2g::PixelBuffer* buffer, void* pixel) {
    stbi_image_free(pixel);
}


namespace m2g {
    Image* ImageCodec::loadImage(const char* filename) {
        int width, height, channels;
        int desired_channels = STBI_default;

        //    FILE* file;
        stbi_uc* image_data = nullptr;
        //    errno_t err = fopen_s(&file, filename, "r");

        //    if(err == 0) {
        if (stbi_info(filename, &width, &height, &channels)) {
            if (channels == 3) {
                desired_channels = STBI_rgb_alpha;
            }
            image_data = stbi_load(filename, &width, &height, &channels, desired_channels);
            if (image_data) {
                return new Image(PixelBuffer::wrap(image_data, width, height, stbChannelsToFormat(channels),
                                                   STB_ImageReleaseFun));
            }
        }
        //        fclose(file);
        //    }

        return nullptr;
    }


    Image* ImageCodec::createImage(Image* src) {
        if (!src->isMutable()) {
            auto* result = new Image(src->getPixelBuffer(), false);
            return result;
        }
        std::shared_ptr<PixelBuffer> copy = std::make_shared<PixelBuffer>(src->getPixelBufferRef());
        auto* result = new Image(std::move(copy), false);
        return result;
    }

    Image* ImageCodec::loadImage(uint8_t* data, int offset, int length) {
        int width, height, channels;
        int desired_channels = STBI_default;
        data += offset;

        stbi_uc* image_data = nullptr;
        if (stbi_info_from_memory(data, length, &width, &height, &channels)) {
            if (channels == 3) {
                desired_channels = STBI_rgb_alpha;
            }
            image_data = stbi_load_from_memory(data, length, &width, &height, &channels, desired_channels);

            if (image_data) {
                return new Image(PixelBuffer::wrap(image_data, width, height, stbChannelsToFormat(channels),
                                                   STB_ImageReleaseFun));
            }
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
        catch (const std::bad_alloc&e) {
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


    static void LTR(uint8_t* dst, uint8_t* src, int pixel, ptrdiff_t src_stride) {
        assert(pixel > 0);
        std::memcpy(dst, src, pixel << 2);
    }


    static void RTL(uint8_t* dst, uint8_t* rgba, int pixel, ptrdiff_t src_stride) {
        assert(pixel > 0);
        auto* dst32 = (uint32_t *)dst;
        auto* src32 = (uint32_t *)rgba;
        while (pixel-- > 0) {
            *dst32++ = *src32--;
        }
    }


    static void TTB(uint8_t* dst, uint8_t* src, int pixel, ptrdiff_t src_stride) {
        assert(pixel > 0);
        auto* dst32 = (uint32_t *)dst;
        auto* src32 = (uint32_t *)src;

        ptrdiff_t row_step = src_stride >> 2;
        while (pixel-- > 0) {
            *dst32++ = *src32;
            src32 += row_step;
        }
    }

    static void BTT(uint8_t* dst, uint8_t* src, int pixel, ptrdiff_t src_stride) {
        assert(pixel > 0);
        auto* dst32 = (uint32_t *)dst;
        auto* src32 = (uint32_t *)src;

        ptrdiff_t row_step = src_stride >> 2;
        while (pixel-- > 0) {
            *dst32++ = *src32;
            src32 -= row_step;
        }
    }


    Image* ImageCodec::createImage(Image* src, int x, int y, int width, int height, int transform) {
        if ((transform & ~Transform::TRANSFORM_MASK) != 0) {
            return nullptr;
        }

        int dst_width, dst_height;
        int src_x, src_y;
        ptrdiff_t inc;

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

        using copy_opt = void (*)(uint8_t*, uint8_t*, int, ptrdiff_t);
        copy_opt opt = nullptr;


        switch (transform) {
            case Transform::TRANS_NONE:
                src_x = x;
                src_y = y;
                break;
            case Transform::TRANS_ROT90:
                src_x = x;
                src_y = y + height - 1;
                break;
            case Transform::TRANS_ROT180:
                src_x = x + width - 1;
                src_y = y + height - 1;
                break;
            case Transform::TRANS_ROT270:
                src_x = x + width - 1;
                src_y = y;
                break;
            case Transform::TRANS_MIRROR:
                src_x = x + width - 1;
                src_y = y;
                break;
            case Transform::TRANS_MIRROR_ROT90:
                src_x = x + width - 1;
                src_y = y + height - 1;
                break;
            case Transform::TRANS_MIRROR_ROT180:
                src_x = x;
                src_y = y + height - 1;
                break;
            case Transform::TRANS_MIRROR_ROT270:
                src_x = x;
                src_y = y;
                break;
            default:
                return nullptr;
        }


        switch (transform) {
            case Transform::TRANS_NONE:
            case Transform::TRANS_MIRROR_ROT180:
                opt = LTR;
                break;
            case Transform::TRANS_MIRROR:
            case Transform::TRANS_ROT180:
                opt = RTL;
                break;
            case Transform::TRANS_MIRROR_ROT270:
            case Transform::TRANS_ROT270:
                opt = TTB;
                break;
            case Transform::TRANS_ROT90:
            case Transform::TRANS_MIRROR_ROT90:
                opt = BTT;
                break;
            default:
                return nullptr;
        }


        switch (transform) {
            case Transform::TRANS_NONE:
            case Transform::TRANS_MIRROR:
                inc = src_stride;
                break;
            case Transform::TRANS_ROT180:
            case Transform::TRANS_MIRROR_ROT180:
                inc = -src_stride;
                break;
            case Transform::TRANS_ROT90:
            case Transform::TRANS_MIRROR_ROT270:
                inc = 4;
                break;
            case Transform::TRANS_MIRROR_ROT90:
            case Transform::TRANS_ROT270:
                inc = -4;
                break;
            default:
                return nullptr;
        }


        auto* dst_pixels = dst_buffer.addr<uint8_t *>();
        auto* src_pixels = src_buffer.addr<uint8_t *>(src_x, src_y);

        while (dst_height-- > 0) {
            opt(dst_pixels, src_pixels, dst_width, src_stride);
            dst_pixels += dst_stride;
            src_pixels += inc;
        }
        return image;
    }
}
