﻿#include "PixelFormat.h"

int PixelFormat::getBytePerPixel(int format) {
    switch (format) {
        case GREY:
            return 1;
        case GREY_ALPHA:
            return 2;
        case ALPHA_8:
            return 1;
        case RGB_888X:
        case RGBA_8888:
        case ARGB_8888:
        case XRGB_8888:
        case BGRA_8888:
        case BGRX_8888:
            return 4;
        default:
            return 0;
    }
}

int PixelFormat::shiftPerPixel(int format) {
    switch (format) {
        case GREY:
            return 0;
        case GREY_ALPHA:
            return 1;
        case ALPHA_8:
            return 0;
        case RGB_888X:
        case RGBA_8888:
        case ARGB_8888:
        case XRGB_8888:
        case BGRA_8888:
        case BGRX_8888:
            return 2;
        default:
            return 0;
    }
}
