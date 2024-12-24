#pragma once

#include <cassert>
#include "m2g-def.h"


void premultiply(uint8_t *val, uint8_t a) {
    *val = (uint8_t)((uint16_t)*val * (uint16_t)a * 255);
}

void premultiply(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t a) {
    premultiply(r, a);
    premultiply(g, a);
    premultiply(b, a);
}


void premultiply(uint8_t* rgba, size_t size) {
    while (size-- > 0) {
        premultiply(&rgba[0], &rgba[1], &rgba[2], rgba[3]);
        rgba += 4;
    }
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



void transform(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height, int transform) {

    using copy_opt = void (*)(uint8_t*, uint8_t*, int, ptrdiff_t);
    copy_opt opt = nullptr;

    int dst_width, dst_height;
    int src_x = 0;
    int src_y = 0;
    ptrdiff_t inc;

    using namespace m2g;
    if (transform & TRANSFORM_INVERTED_AXES) {
        dst_width = height;
        dst_height = width;
    }
    else {
        dst_width = width;
        dst_height = height;
    }


    switch (transform) {
        case Transform::TRANS_NONE:
        case Transform::TRANS_MIRROR_ROT270:
            break;
        case Transform::TRANS_ROT90:
        case Transform::TRANS_MIRROR_ROT180:
            src_y = height - 1;
            break;
        case Transform::TRANS_MIRROR_ROT90:
        case Transform::TRANS_ROT180:
            src_x = width - 1;
            src_y = height - 1;
            break;
        case Transform::TRANS_ROT270:
        case Transform::TRANS_MIRROR:
            src_x = width - 1;
            break;

        default:
            return;
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
            return;
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
            return;
    }

    uint8_t* dst_pixels = dst;
    uint8_t* src_pixels = src + (src_y * src_stride) + (src_x * 4);

    int row = dst_height;
    while (row-- > 0) {
        opt(dst_pixels, src_pixels, dst_width, src_stride);
        dst_pixels += dst_stride;
        src_pixels += inc;
    }
}
