//
// Created by Admin on 2023/5/22.
//

#ifndef NXJVM_IMAGE_H
#define NXJVM_IMAGE_H

#include <stdint.h>
#include "Graphics.h"


class Graphics;
class PixelBuffer;



class Image {






public:

    Image(std::shared_ptr<PixelBuffer> pixel);
    Image(std::shared_ptr<PixelBuffer> pixel, bool isMutable);
    ~Image();

    int getFormat() const;
    int getWidth() const;
    int getHeight() const;
    bool isMutable() const;
    bool hasAlpha() const;
    bool isColor() const;

    Graphics *getGraphics();

    void setImmutable();

    void getRGB(int *data, int dataLength, int offset, int scanLength, int x_, int y_, int width_, int height_);


    PixelBuffer& getPixelBufferRef();

public:

//    uint8_t* data;

//    int channels;


private:
    std::shared_ptr<PixelBuffer> pixel;
    int width;
    int height;
    bool mut;
};


#endif //NXJVM_IMAGE_H
