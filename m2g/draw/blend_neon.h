#pragma once

#include "arm_neon.h"
#include "pix_def.h"
#include "blend.h"
#include "string.h"
#include "assert.h"
#define R 0
#define G 1
#define B 2
#define A 3

// int16_t src = cast<int16_t>(src);
// int16_t dst = cast<int16_t>(dst);
// int16_t alpha = cast<int16_t>(alpha);
// dst += (src - dst) * alpha / 0xFF;
// return cast<uint8_t>(dst);


static uint8x8_t alpha_blend_channel_neon(uint8x8_t src, uint8x8_t dst, uint16x8_t alpha) {
    int16x8_t src_wide, dst_wide;
    src_wide = vreinterpretq_s16_u16(vmovl_u8(src));
    dst_wide = vreinterpretq_s16_u16(vmovl_u8(dst));
    src_wide = (src_wide - dst_wide) * vreinterpretq_s16_u16(alpha);
    dst_wide += vshrq_n_s16(src_wide, 8);
    return vmovn_u16(vreinterpretq_u16_s16(dst_wide));
}




template<int __R, int __G, int __B, int __A>
void blend_rgba_neon_order(uint8_t* dst, const uint8_t* src, size_t size) {
    // assert(size % 32 == 0);
    for (int i = 0; i < size; i += 8) {
        uint8x8x4_t vdst = vld4_u8(dst);
        uint8x8x4_t vsrc = vld4_u8(src);
        uint16x8_t alpha = vmovl_u8(vsrc.val[__A]);
        vdst.val[R] = alpha_blend_channel_neon(vsrc.val[__R], vdst.val[R], alpha);
        vdst.val[G] = alpha_blend_channel_neon(vsrc.val[__G], vdst.val[G], alpha);
        vdst.val[B] = alpha_blend_channel_neon(vsrc.val[__B], vdst.val[B], alpha);
        vdst.val[A] = vdup_n_u8(0xFF);;
        vst4_u8(dst, vdst);

        src += 32;
        dst += 32;
    }
}

inline void blend_a8_neon(uint8_t* dst, uint8_t* a8, uint8_t* rgba, int size) {
    uint8x8_t vred   = vdup_n_u8(rgba[0]);
    uint8x8_t vgreen = vdup_n_u8(rgba[1]);
    uint8x8_t vblue  = vdup_n_u8(rgba[2]);
    uint8x8_t valpha_full = vdup_n_u8(0xFF);

    for (int i = 0; i < size; i += 8) {
        uint8x8x4_t vdst = vld4_u8(dst);
        uint16x8_t valpha = vmovl_u8(vld1_u8(a8));

        vdst.val[0] = alpha_blend_channel_neon(vred, vdst.val[0], valpha);
        vdst.val[1] = alpha_blend_channel_neon(vgreen, vdst.val[1], valpha);
        vdst.val[2] = alpha_blend_channel_neon(vblue, vdst.val[2], valpha);
        vdst.val[3] = vdup_n_u8(0xFF);
        vst4_u8(dst, vdst);

        a8  += 8;
        dst += 32;
    }
}

static inline void blend_r8g8b8a8_neon(uint8_t* dst, uint8_t* src, int size) {
    blend_rgba_neon_order<0, 1, 2, 3>(dst, src, size);
}

template<int order>
static void blend_rgba_neon(uint8_t* dst, const uint8_t* src, int count) {
    if constexpr (order == pix_order::ARGB) {
        blend_rgba_neon_order<1, 2, 3, 0>(dst, src, count);
    } else if constexpr (order == pix_order::BGRA) {
        blend_rgba_neon_order<2, 1, 0, 3>(dst, src, count);
    } else if constexpr (order == pix_order::RGBA) {
        blend_rgba_neon_order<0, 1, 2, 3>(dst, src, count);
    }
}

static void blend_(uint8_t* dst, uint8_t* src, int count) {
    int num = count % 8;
    int offset = count - num;
    blend_rgba_neon<pix_order::RGBA>(dst, src, offset);
    dst += (offset << 2);
    src += (offset << 2);
    blend_n(dst, src, num);
}


inline void blendA8(uint8_t* dst, uint8_t* src, uint8_t* rgba, int count) {

    int e_c = count / 8;
    int r_c = count % 8;

    int offset = count - r_c;
    blend_a8_neon(dst, src, rgba, offset);

    dst += offset << 2;
    src += offset;
    blendA8_span(dst, src, rgba, r_c);
}


#undef A
#undef R
#undef G
#undef B


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

        int num = n % 8;
        int count = n - num;
        blend_rgba_neon<src_order>(dst, src, count);
        dst += (count << 2);
        src += (count << 2);

        if constexpr (src_order == pix_order::RGBA) {
            pix_blend<0, 1, 2, 3>(dst, src, num);
        } else if constexpr (src_order == pix_order::BGRA) {
            pix_blend<2, 1, 0, 3>(dst, src, num);
        } else if constexpr (src_order == pix_order::ARGB) {
            pix_blend<1, 2, 3, 0>(dst, src, num);
        } else {
            static_assert(false, "unknown order");
        }
    }

}

