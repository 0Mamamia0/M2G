

#include "ImageCodec.h"

#include <iostream>
#include <cassert>
#include <cstring>
#include <algorithm>

#include "m2g-def.h"
#include "Image.h"
#include "stb_image.h"

namespace m2g {


    Image* ImageCodec::createEmptyImage(int width, int height, bool isMutable) {
        return nullptr;
    }

    Image* ImageCodec::createImage(int width, int height) {
        Image* image = new Image(width, height);
        void* pixels = image->getPixels();
        int stride = image->getStride();
        while (height -- > 0) {
            memset(pixels, 0xFF, stride);
        }
        return image;
    }

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
                return new Image(image_data, width, height, channels, [](Image* image, void* data) {
                    stbi_image_free(data);
                });
            }
        }
        //        fclose(file);
        //    }

        return nullptr;


        return nullptr;
    }

    Image* ImageCodec::loadImage(uint8_t* data, int offset, int length) {

        int width, height, channels;
        int desired_channels = STBI_default;
        data += offset;

        stbi_uc* image_data = nullptr;


        if (!stbi_info_from_memory(data, length, &width, &height, &channels)) {
            return nullptr;
        }

        if (channels == STBI_rgb) {
            desired_channels = STBI_rgb_alpha;
        }
        image_data = stbi_load_from_memory(data, length, &width, &height, &channels, desired_channels);

        if(image_data == nullptr)
            return nullptr;
        if(channels != 4 && channels != 3) {
            std::cerr << "Not Impl yet" << std::endl;
            return nullptr;
        }
        if(channels == 4) {
            stbi_uc* color = image_data;
            for (int i = 0; i < width * height; i ++) {
                uint8_t r = color[0];
                uint8_t g = color[1];
                uint8_t b = color[2];
                uint8_t a = color[3];

                float af = a / 0xFF;
                b = std::clamp<int16_t>((b & 0xFF) * af, 0, 0xFF);
                g = std::clamp<int16_t>((g & 0xFF) * af, 0, 0xFF);
                r = std::clamp<int16_t>((r & 0xFF) * af, 0, 0xFF);

                color[0] = b ;
                color[1] = g;
                color[2] = r;
                color[3] = a;
                color += 4;
            }
        } else if(channels == 3) {
            stbi_uc* color = image_data;
            for (int i = 0; i < width * height; i ++) {
                uint8_t r = color[0];
                uint8_t g = color[1];
                uint8_t b = color[2];
                uint8_t a = color[3];

                float af = a / 0xFF;
                color[0] = b * af;
                color[1] = g * af;
                color[2] = r * af;
                color[3] = 0xFF;
                color += 4;
            }
        }

        if(channels == 4 || channels == 3) {

            return new Image(image_data, width, height, channels,  [](Image* image, void* data) {
               stbi_image_free(data);
            });
        }

        return nullptr;
    }

    Image* ImageCodec::createImage(Image* src) {
        // if (!src->isMutable()) {
        //     Image* copy = new Image(*src);
        //     return result;
        // }

        if(src->isMutable()) {
            Image* image  = new Image(*src);
            image->setImmutable();
            return image;
        }

        return nullptr;
    }

    Image* ImageCodec::createRGBImage(int32_t* argbData, int width, int height, bool processAlpha) {

        try {
            int pixelsCount = width * height;
            uint8_t* pixels = (uint8_t*)malloc(pixelsCount * 4);

            uint8_t* tmp = pixels;

            if(processAlpha) {
                while (pixelsCount-- > 0) {
                    uint32_t argb = *argbData++;

                    uint8_t a = (argb >> 24) & 0xFF;
                    float af = a / 0xFF;

                    *tmp++ = std::clamp<uint8_t>((argb & 0xFF) * af, 0, 0xFF);
                    *tmp++ = std::clamp<uint8_t>((argb >> 8 & 0xFF) * af, 0, 0xFF);
                    *tmp++ = std::clamp<uint8_t>((argb >> 16 & 0xFF) * af, 0, 0xFF);
                    *tmp++ = a;

                }
            } else {
                while (pixelsCount-- > 0) {
                    uint32_t argb = *argbData++;
                    *tmp++ = argb & 0xFF;
                    *tmp++ = argb >> 8 & 0xFF;
                    *tmp++ = argb >> 16 & 0xFF;
                    *tmp++ = 0xFF;

                }
            }


            return new Image(pixels, width, height, processAlpha ? 4 : 3, [](Image* image, void* data) {
                free(data);
            });
        } catch (const std::bad_alloc& e) {
            return nullptr;
        }

    }



    static void LTR(uint8_t* dst, uint8_t* src, int pixel, ptrdiff_t src_stride) {
        assert(pixel > 0);
        std::memcpy(dst, src, pixel << 2);
    }


    static void RTL(uint8_t* dst, uint8_t* rgba, int pixel, ptrdiff_t src_stride) {
        assert(pixel > 0);
        uint32_t* dst32 = (uint32_t *)dst;
        uint32_t* src32 = (uint32_t *)rgba;
        while (pixel-- > 0) {
            *dst32++ = *src32--;
        }
    }


    static void TTB(uint8_t* dst, uint8_t* src, int pixel, ptrdiff_t src_stride) {
        assert(pixel > 0);
        uint32_t* dst32 = (uint32_t *)dst;
        uint32_t* src32 = (uint32_t *)src;

        ptrdiff_t row_step = src_stride >> 2;
        while (pixel-- > 0) {
            *dst32++ = *src32;
            src32 += row_step;
        }
    }

    static void BTT(uint8_t* dst, uint8_t* src, int pixel, ptrdiff_t src_stride) {
        assert(pixel > 0);
        uint32_t* dst32 = (uint32_t *)dst;
        uint32_t* src32 = (uint32_t *)src;

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


        auto src_surface = src->getCairoSurface();
        auto* src_data =  cairo_image_surface_get_data(src_surface);
        auto* dst_data = (unsigned char*) malloc(dst_width * dst_height * 4);

        if(dst_data == nullptr) {
            return nullptr;
        }

        int dst_stride = dst_width * 4;
        int src_stride = src->getStride();

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

        uint8_t* dst_pixels = dst_data;
        uint8_t* src_pixels = src_data + (src_y * src_stride) + (src_x * 4);

        int row = dst_height;
        while (row-- > 0) {
            opt(dst_pixels, src_pixels, dst_width, src_stride);
            dst_pixels += dst_stride;
            src_pixels += inc;
        }


        return new Image(dst_data, dst_width, dst_height, src->getChannels(), [](Image*,void* data) {
            free(data);
        });
    }
}
