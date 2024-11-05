#pragma once

#include "PixelFormat.h"


inline void blend1px(uint8_t *dst, const uint8_t *src) {
    int alpha = src[3];

    if (alpha == 0x0) {
        return;
    }

    if (alpha == 0xFF) {
        memcpy(dst, src, 4);
        return;
    }

    int invAlpha = 0xFF - alpha;
    dst[0] = (src[0] * alpha + dst[0] * invAlpha) >> 8;
    dst[1] = (src[1] * alpha + dst[1] * invAlpha) >> 8;
    dst[2] = (src[2] * alpha + dst[2] * invAlpha) >> 8;
    dst[3] = 0xFF;
}


inline void blend_n(uint8_t *dst, const uint8_t *src, int count) {
    while (count-- > 0) {
        blend1px(dst, src);
        dst += 4;
        src += 4;
    }
}


inline void blendA8(uint8_t *dst, uint8_t a8, uint8_t *rgba) {
    if (a8 == 0xFF) {
        memcpy(dst, rgba, 4);
    } else if (a8 != 0x0) {
        int alpha = (int) a8;
        int invAlpha = 0xFF - alpha;

        dst[0] = (uint8_t) ((rgba[0] * alpha + dst[0] * invAlpha) >> 8);
        dst[1] = (uint8_t) ((rgba[1] * alpha + dst[1] * invAlpha) >> 8);
        dst[2] = (uint8_t) ((rgba[2] * alpha + dst[2] * invAlpha) >> 8);
        dst[3] = 0xFF;
    }
}


inline void blendA8_span(uint8_t *dst, const uint8_t *src, uint8_t *rgba, int count) {
    for (int i = 0; i < count; ++i) {
        blendA8(dst, *src, rgba);
        src++;
        dst += 4;
    }
}

static void blendA8_8(uint8_t *dst, const uint8_t *src, uint8_t *rgba) {
    uint64_t s = *(uint64_t *) src;
    if (s == 0xFFFFFFFFFFFFFFFF) {
        uint32_t *d = (uint32_t *) (dst);
        uint32_t color = *(uint32_t *) (rgba);


        *d++ = color;
        *d++ = color;
        *d++ = color;
        *d++ = color;

        *d++ = color;
        *d++ = color;
        *d++ = color;
        *d++ = color;
    } else if (s != 0) {
        blendA8(dst, *src++, rgba);
        blendA8(dst + 4, *src++, rgba);
        blendA8(dst + 8, *src++, rgba);
        blendA8(dst + 12, *src++, rgba);
        blendA8(dst + 16, *src++, rgba);
        blendA8(dst + 20, *src++, rgba);
        blendA8(dst + 24, *src++, rgba);
        blendA8(dst + 28, *src, rgba);

        // blendA8_span(dst, src, rgba, 8);
    }
}


#ifdef __SWITCH__
#include "blend_neon.h"
#else
#include "blend_c.h"
#endif


namespace m2g::piv {
    template<int src_order>
    void blend_rect_order(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
                          int width, int height) {
        while (height-- > 0) {
            pix_blend_order<src_order>(dst, src, width);
            dst += dst_stride;
            src += src_stride;
        }
    }
}



static inline void
blendRect(uint8_t *dst, uint8_t *src, int dst_format, int src_format, ptrdiff_t dst_stride, ptrdiff_t src_stride,
          int width, int height) {
    if (dst_format != PixelFormatType::RGBA_8888) {
        // Not Impl;
        return;
    }
    switch (src_format) {
        case RGBA_8888:
            m2g::piv::blend_rect_order<pix_order::RGBA>(dst, src, dst_stride, src_stride, width, height);
            break;
        case ARGB_8888:
            m2g::piv::blend_rect_order<pix_order::ARGB>(dst, src, dst_stride, src_stride, width, height);
            break;
        case BGRA_8888:
            m2g::piv::blend_rect_order<pix_order::BGRA>(dst, src, dst_stride, src_stride, width, height);
            break;
        default:
            break;
    }
}



