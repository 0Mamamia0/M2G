//
// Created by Admin on 2023/5/2.
//

#ifndef NXJVM_IMAGECODEC_H
#define NXJVM_IMAGECODEC_H


#include <cstdint>

class Image;

class ImageCodec {

public:
    static Image* createEmptyImage(int width, int height);
    static Image* createImage(int width, int height);

    static Image* loadImage(const char* filename);
    static Image* loadImage(uint8_t* data, int offset, int length);
    static Image* createImage(Image *src);
    static Image* createRGBImage(int32_t *argb, int width, int height, bool processAlpha);


};


#endif //NXJVM_IMAGECODEC_H
