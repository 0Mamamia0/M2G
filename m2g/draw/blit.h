#pragma once

#include <cstdlib>
#include <cstddef>
#include "fill.h"
#include "blend.h"

namespace m2g {

    template<bool flip>
    inline void blit_h(uint8_t* __restrict begin, int count, uint8_t* __restrict rgba) {
        uint8_t alpha = rgba[3];
        if (alpha == 0)
            return;
        if (alpha == 0xFF) {
            fill_h<flip>(begin, count, rgba);
        } else {
            blend_h_color<flip>(begin, count, rgba);
        }

    }

    template<bool flip>
    inline void blit_v(uint8_t* __restrict begin, int count, ptrdiff_t stride, uint8_t* __restrict rgba) {
        uint8_t alpha = rgba[3];
        if (alpha == 0)
            return;
        if (alpha == 0xFF) {
            fill_v<flip>(begin, count, stride, rgba);
        } else {
            blend_v_color<flip>(begin, count, stride, rgba);
        }
    }

    inline void blit_line_h(uint8_t* __restrict begin, int count,  uint8_t* __restrict rgba) {
        if (count < 0) {
            blit_h<true>(begin, -count, rgba);
        }  else {
            blit_h<false>(begin, count, rgba);
        }
    }

    inline void blit_line_v(uint8_t* __restrict begin, int count, ptrdiff_t stride, uint8_t* __restrict rgba) {
        if (count < 0) {
            blit_v<true>(begin, -count, stride, rgba);
        }  else {
            blit_v<false>(begin, count, stride, rgba);
        }
    }



    inline void blit_rect(uint8_t* __restrict begin, int width, int height, ptrdiff_t stride, uint8_t* __restrict rgba) {
        uint8_t alpha = rgba[3];
        if (alpha == 0)
            return;
        if (alpha == 0xFF) {
            fill_rect(begin, width, height,  stride, rgba);
        } else {
            blend_rect_color(begin, width, height, stride, rgba);
        }
    }
}