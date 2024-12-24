#include "STB_Decoder.h"


#include "stb_image.h"
#include "ImageFormat.h"


namespace m2g {
    static int stbChannelsToFormat(int channels) {
        switch (channels) {
            case 1: return GREY;
            case 2: return GREY_ALPHA;
            case 3: return RGB_888X;
            case 4: return RGBA_8888;
            default: return -1;
        }
    }

    static void STB_PixelReleaseFun(PixelBuffer* buffer, void* pixel) {
        stbi_image_free(pixel);
    }


    std::shared_ptr<PixelBuffer> STB_Decoder::decode(uint8_t* data, int length) {
        int width, height, channels;
        int desired_channels = STBI_default;

        stbi_uc* image_data = nullptr;
        if (!stbi_info_from_memory(data, length, &width, &height, &channels)) {
            return nullptr;
        }

        if (channels == STBI_rgb) {
            desired_channels = STBI_rgb_alpha;
        }
        image_data = stbi_load_from_memory(data, length, &width, &height, &channels, desired_channels);
        if (!image_data) {
            return nullptr;
        }

        return PixelBuffer::wrap(image_data, width, height, stbChannelsToFormat(channels), STB_PixelReleaseFun);
    }

    std::shared_ptr<PixelBuffer> STB_Decoder::decode(const char *filename) {
        int width, height, channels;
        int desired_channels = STBI_default;

        stbi_uc* image_data = nullptr;
        if (!stbi_info(filename, &width, &height, &channels)) {
            return nullptr;
        }

        if (channels == STBI_rgb) {
            desired_channels = STBI_rgb_alpha;
        }
        image_data = stbi_load(filename, &width, &height, &channels, desired_channels);
        if (image_data == nullptr) {
            return nullptr;
        }

        return PixelBuffer::wrap(image_data, width, height, stbChannelsToFormat(channels), STB_PixelReleaseFun);
    }

}

