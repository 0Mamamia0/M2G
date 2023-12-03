//
// Created by Admin on 2023/5/30.
//

#ifndef LEARNSFML_PIXELFORMAT_H
#define LEARNSFML_PIXELFORMAT_H

enum PixelFormatType {
    GREY       = 0,
    GREY_ALPHA = 1,
    ALPHA_8    = 2,
    RGB_888X   = 3,
    RGBA_8888  = 4,
} ;


class PixelFormat {

public:
    static int getBytePerPixel(int  format);

    static int shiftPerPixel(int format);
};


#endif //LEARNSFML_PIXELFORMAT_H
