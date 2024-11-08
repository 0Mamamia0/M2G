#pragma once

#include <cstdint>
//template<int order>
//void blend_rect_order(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
//                      int width, int height) {
//    while (height-- > 0) {
//        if constexpr (order == pix_order::RGBA) {
//            m2g::piv::pix_blend<0, 1, 2, 3>(dst, src, width);
//        } else if constexpr (order == pix_order::BGRA) {
//            m2g::piv::pix_blend<3, 2, 1, 0>(dst, src, width);
//        } else if constexpr (order == pix_order::ARGB) {
//            m2g::piv::pix_blend<1, 2, 3, 0>(dst, src, width);
//        } else {
//            static_assert(false, "unknown order");
//        }
//        dst += dst_stride;
//        src += src_stride;
//    }
//}
//
inline void blendA8(uint8_t *dst, uint8_t *src, uint8_t *rgba, int count) {
    int e_c = count / 8;
    int r_c = count % 8;
    for (int i = 0; i < e_c; ++i) {
        blendA8_8(dst, src, rgba);
        src += 8;
        dst += 32;
    }
    blendA8_span(dst, src, rgba, r_c);
}
//
//
//inline void blend(uint8_t *dst, uint8_t *src, int count) {
//    blend_n(dst, src, count);
//}


namespace m2g::piv {
    template<int R, int G, int B, int A>
    void pix_blend(uint8_t *dst, const uint8_t *src, int n) {
        uint8_t rgba[4];
        while (n-- > 0) {
            rgba[0] = src[R];
            rgba[1] = src[G];
            rgba[2] = src[B];
            rgba[3] = src[A];
            blend1px(dst, rgba);
            dst += 4;
            src += 4;
        }
    }



    template<int src_order>
    void pix_blend_order(uint8_t *dst, const uint8_t *src, int n) {
        if constexpr (src_order == pix_order::RGBA) {
            pix_blend<0, 1, 2, 3>(dst, src, n);
        } else if constexpr (src_order == pix_order::BGRA) {
            pix_blend<2, 1, 0, 3>(dst, src, n);
        } else if constexpr (src_order == pix_order::ARGB) {
            pix_blend<1, 2, 3, 0>(dst, src, n);
        }
    }

}