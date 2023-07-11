//
// Created by Admin on 2024/2/20.
//

#ifndef CAIROIMAGECODEC_H
#define CAIROIMAGECODEC_H



#include <cstdint>





namespace m2g {

    class Image;

    class ImageCodec {
    public:
        static Image* createEmptyImage(int width, int height, bool isMutable);
        static Image* createImage(int width, int height);
        static Image* loadImage(const char* filename);
        static Image* loadImage(uint8_t* data, int offset, int length);
        static Image* createImage(Image *src);
        static Image* createRGBImage(int32_t *argb, int width, int height, bool processAlpha);
        static Image* createImage(Image* src, int x, int y, int width, int height, int transform);
    };
}



#endif //CAIROIMAGECODEC_H
