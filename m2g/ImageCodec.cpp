//
// Created by Admin on 2023/5/2.
//

#include "ImageCodec.h"

#include "stdio.h"
#include "stb_image.h"
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

static void STB_ImageReleaseFun(PixelBuffer* buffer, void* pixel) {
    stbi_image_free(pixel);
}


Image* ImageCodec::loadImage(const char *filename) {
    int width, height, channels;
    int desired_channels = STBI_default;

//    FILE* file;
    stbi_uc* image_data = nullptr;
//    errno_t err = fopen_s(&file, filename, "r");

//    if(err == 0) {
        if(stbi_info(filename, &width, &height, &channels)) {
            if(channels == 3) {
                desired_channels  = STBI_rgb_alpha;
            }
            image_data = stbi_load(filename, &width, &height, &channels, desired_channels);
            if(image_data) {
                return new Image(PixelBuffer::wrap(image_data, width, height, stbChannelsToFormat(channels), STB_ImageReleaseFun));
            }
        }
//        fclose(file);
//    }

    return nullptr;
}



Image *ImageCodec::createImage(Image* src) {
//    if(src->get == 3 || src->channels == 4) {
//        Image* image = Image::createImage(src->width, src->height);
//        memcpy(image->data, src->data, src->width * src->height * src->channels);
//    }
//    return Image::createImage(src->width, src->height);
    return nullptr;
}

Image *ImageCodec::loadImage(uint8_t *data, int offset, int length) {
    int width, height, channels;
    int desired_channels = STBI_default;
    data += offset;

    stbi_uc* image_data = nullptr;
    if(stbi_info_from_memory(data, length, &width, &height, &channels)) {
        if(channels == 3) {
            desired_channels  = STBI_rgb_alpha;
        }
        image_data = stbi_load_from_memory(data, length, &width, &height, &channels, desired_channels);

        if(image_data) {
            return new Image(PixelBuffer::wrap(image_data, width, height, stbChannelsToFormat(channels), STB_ImageReleaseFun));
        }
    }
    return nullptr;
}

Image *ImageCodec::createRGBImage(int32_t *argbData, int width, int height, bool processAlpha) {

    Image* image = createEmptyImage(width, height);

    if(image) {
        PixelBuffer& pixel = image->getPixelBufferRef();
        uint8_t* pixels =  pixel.addr<uint8_t*>();

        int pixelsCount = width * height;
        while (pixelsCount-- > 0) {
            int32_t argb = *argbData++;
            *pixels++ = argb >> 16 &0xFF;
            *pixels++ = argb >> 8  &0xFF;
            *pixels++ = argb       &0xFF;
            if(!processAlpha) {
                *pixels++ = 0xFF;
            } else {
                *pixels++ = argb >> 24 &0xFF;
            }
        }
    }
    return image;
}

Image *ImageCodec::createEmptyImage(int width, int height) {
    if(width <= 0 || height <= 0) {
        return nullptr;
    }
    auto pixel = PixelBuffer::allocate(width, height, PixelFormatType::RGBA_8888);
    return pixel == nullptr ? nullptr : new Image(std::move(pixel), true);
}

Image *ImageCodec::createImage(int width, int height) {
    Image* image = createEmptyImage(width, height);
    if(image != nullptr) {
        PixelBuffer& pixel = image->getPixelBufferRef();
        pixel.erase(0xFF);
    }
    return image;

}


