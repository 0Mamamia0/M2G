#pragma once

#include "pix_def.h"
#include "PixelFormat.h"
#include "blend_common.h"
#ifdef __SWITCH__
#include "blend_neon.h"
#else
#include "blend_c.h"
#endif

template<bool flip>
inline void blend_h_color(uint8_t* __restrict begin, int count, uint8_t* __restrict rgba) {
    uint8_t color[4];
    premultiply_pixel<RGBA>(rgba, color);

    if constexpr (flip) {
        uint8_t* start = begin - 4 * (count - 1);
        blend_n_color_premul<SRC_OVER>(start, color, count, 4);
    } else {
        blend_n_color_premul<SRC_OVER>(begin, color,  count, 4);
    }
}

template<bool flip>
inline void blend_v_color(uint8_t* __restrict begin, int count, ptrdiff_t stride, uint8_t* __restrict rgba) {
    uint8_t color[4];
    premultiply_pixel<RGBA>(rgba, color);

    if constexpr (flip) {
        uint8_t* start = begin - (count - 1) * stride;
        blend_n_color_premul<SRC_OVER>(start, color, count, stride);
    } else {
        blend_n_color_premul<SRC_OVER>(begin, color, count, stride);
    }
}


inline void blend_rect_color(uint8_t* __restrict begin, int width, int height, ptrdiff_t stride, uint8_t* __restrict rgba) {
    uint8_t color[4];
    premultiply_pixel<RGBA>(rgba, color);

    if (stride == width * 4) {
        blend_n_color_premul<SRC_OVER>(begin, color, width * height, 4);
    } else {
        while (height-- > 0) {
            blend_n_color_premul<SRC_OVER>(begin, color,  width, 4);
            begin += stride;
        }
    }
}



namespace m2g::piv {
    template<pix_order src_order>
    void blend_rect_order(uint8_t *dst, const uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height) {
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
    if (dst_format != RGBA_8888) {
        return;
    }
    switch (src_format) {
        case RGBA_8888:
            m2g::piv::blend_rect_order<RGBA>(dst, src, dst_stride, src_stride, width, height);
            break;
        case ARGB_8888:
            m2g::piv::blend_rect_order<ARGB>(dst, src, dst_stride, src_stride, width, height);
            break;
        case BGRA_8888:
            m2g::piv::blend_rect_order<BGRA>(dst, src, dst_stride, src_stride, width, height);
            break;
        default:
            break;
    }
}



