#pragma once
#include <bit>
#include <cstring>
#include "color.h"
#include "pix_def.h"
#include "blit.h"
#include "blend.h"
#include "copy.h"
#include "run.h"
#include "draw_text.h"
#include "draw_image.h"




namespace m2g {





    namespace piv {


        template<bool flip>
        void line_copy(uint8_t *dst, uint8_t *src, int pixel) {
            if (!flip) {
                memcpy(dst, src, pixel << 2);
            } else {
                auto *dst32 = (uint32_t *) dst;
                auto *src32 = (uint32_t *) src + pixel;
                while (pixel-- > 0) {
                    *dst32++ = *--src32;
                }
            }
        }


        template<bool x_flip, bool y_flip>
        void rect_copy(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height) {
            if (y_flip) {
                src = src + (height - 1) * src_stride;
                src_stride = -src_stride;
            }

            while (height-- > 0) {
                line_copy<x_flip>(dst, src, width);
                dst += dst_stride;
                src += src_stride;
            }
        }


        inline void line_move(uint8_t *dst, uint8_t *src, int pixel) {
            memmove(dst, src, pixel << 2);
        }

        inline void rect_move(uint8_t *dst, uint8_t *src, ptrdiff_t stride, int width, int height) {
            while (height-- > 0) {
                line_move(dst, src, width);
                dst += stride;
                src += stride;
            }
        }
    }


    inline void copyArea(uint8_t *pixels, ptrdiff_t stride, const m2g::Rect &dst_area, const m2g::Rect &src_area) {
        // assert(dst_area.getWidth() == src_area.getWidth());
        // assert(dst_area.getHeight() == src_area.getHeight());
        // assert(dst_area.getWidth() > 0 && dst_area.getHeight() > 0);

        int width = dst_area.getWidth();
        int height = dst_area.getHeight();
        ptrdiff_t src_offset = (src_area.top * stride) + (src_area.left * 4);
        ptrdiff_t dst_offset = (dst_area.top * stride) + (dst_area.left * 4);
        uint8_t *dst = pixels + dst_offset;
        uint8_t *src = pixels + src_offset;


        if (src_area.contain(dst_area)) {
            if (src_area.top == dst_area.top) {
                piv::rect_move(dst, src, stride, width, height);
            } else if (src_area.top < dst_area.top) {
                dst = dst + (height - 1) * stride;
                src = src + (height - 1) * stride;
                stride = -stride;
                piv::rect_copy<false, false>(dst, src, stride, stride, width, height);
            } else {
                piv::rect_copy<false, false>(dst, src, stride, stride, width, height);
            }
        } else {
            piv::rect_copy<false, false>(dst, src, stride, stride, width, height);
        }
    }








    //static inline void blendRect(uint32_t* dst, uint32_t* src, int step,  int xInc, int yInc, int width, int height) {
    //    while (height-- > 0) {
    //        auto dstTemp = reinterpret_cast<Color*>(dst);
    //        auto srcTemp = reinterpret_cast<Color*>(src);
    //        auto tmpW = width;
    //
    //        while (tmpW-- > 0) {
    //            blend(dstTemp++, srcTemp);
    //            srcTemp += xInc;
    //        }
    //
    //        src += yInc;
    //        dst += step;
    //    }
    //}
    //
    //static inline void blendRectYFlip(uint32_t* dst, uint32_t* src, int dst_step, int src_step, int width, int height) {
    //    while (height-- > 0) {
    //        auto dstTemp = reinterpret_cast<Color*>(dst);
    //        auto srcTemp = reinterpret_cast<Color*>(src);
    //        auto tmpW = width;
    //
    //        while (tmpW-- > 0) {
    //            blend(dstTemp++, srcTemp --);
    //        }
    //        dst += dst_step;
    //        src += src_step;
    //    }
    //}
}


