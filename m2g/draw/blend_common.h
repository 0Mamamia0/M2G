#pragma once

#include "convert.h"

template<blend_mode mode>
inline uint8_t blend_channel(uint8_t dst, uint8_t src, int alpha, int alpha_inv) {
    return 0;
}

template<blend_mode mode>
inline uint8_t blend_channel_alpha(int dst_alpha, int src_alpha, int src_alpha_inv) {
    return 0;
}

template<blend_mode mode>
inline uint8_t blend_channel_premul(uint8_t dst, uint8_t src, int alpha_inv) {
    return 0;
}

template<>
inline uint8_t blend_channel<SRC_OVER>(uint8_t dst, uint8_t src, int alpha, int alpha_inv) {
    return (uint8_t)((src * alpha + dst * alpha_inv) >> 8);
}

template<>
inline uint8_t blend_channel_premul<SRC_OVER>(uint8_t dst, uint8_t src, int alpha_inv) {
    return (uint8_t)(src + ((dst * alpha_inv) >> 8));
}


template<>
inline uint8_t blend_channel_alpha<SRC_OVER>(int dst_alpha, int src_alpha, int src_alpha_inv) {
    return (uint8_t)(src_alpha + (dst_alpha * src_alpha_inv >> 8));
}



template<blend_mode mode, int R, int G, int B, int A>
inline void blend_pixel_rgba(uint8_t* dst, const uint8_t *src) {
    int src_a = src[A];
    int src_a_inv = 0xFF - src_a;
    dst[0] = blend_channel<mode>(dst[0], src[R], src_a, src_a_inv);
    dst[1] = blend_channel<mode>(dst[1], src[G], src_a, src_a_inv);
    dst[2] = blend_channel<mode>(dst[2], src[B],  src_a, src_a_inv);
    dst[3] = blend_channel_alpha<mode>(dst[3], src_a, src_a_inv);
}

template<blend_mode mode, int R, int G, int B, int A>
inline void blend_pixel_rgba_premul(uint8_t* dst, const uint8_t *src) {
    int src_a = src[A];
    int src_a_inv = 0xFF - src_a;
    dst[0] = blend_channel_premul<mode>(dst[0], src[R], src_a_inv);
    dst[1] = blend_channel_premul<mode>(dst[1], src[G], src_a_inv);
    dst[2] = blend_channel_premul<mode>(dst[2], src[B], src_a_inv);
    dst[3] = blend_channel_alpha<mode>(dst[3], src_a, src_a_inv);
}

template<blend_mode mode>
inline void blend_pixel_a8_color_rgb(uint8_t* dst, const uint8_t *src, uint8_t* color) {
    int src_a = src[0];
    int src_a_inv = 0xFF - src_a;

    dst[0] = blend_channel<mode>(dst[0], color[0], src_a,  src_a_inv);
    dst[1] = blend_channel<mode>(dst[1], color[1], src_a,  src_a_inv);
    dst[2] = blend_channel<mode>(dst[2], color[2], src_a,  src_a_inv);
    dst[3] = blend_channel_alpha<mode>(dst[3], src_a, src_a_inv);
    // dst[3] = 0xFF;
}

template<blend_mode mode>
inline void blend_pixel_a8_color_rgba_premul(uint8_t* dst, const uint8_t *src, uint8_t* color) {
    int src_a = src[0];
    int src_a_inv = 0xFF - src_a;

    int r = (color[0] * src_a ) / 0xFF;
    int g = (color[1] * src_a ) / 0xFF;
    int b = (color[2] * src_a ) / 0xFF;

    dst[0] = blend_channel_premul<mode>(dst[0], r, src_a_inv);
    dst[1] = blend_channel_premul<mode>(dst[1], g, src_a_inv);
    dst[2] = blend_channel_premul<mode>(dst[2], b, src_a_inv);
    dst[3] = blend_channel_alpha<mode>(dst[3], src_a, src_a_inv);
}



template<blend_mode mode, pix_order src_order>
inline void blend_pixel(uint8_t* dst, const uint8_t *src) {
    if constexpr (src_order == RGBA) {
        blend_pixel_rgba<mode, 0, 1, 2, 3>(dst, src);
    } else if constexpr (src_order == BGRA) {
        blend_pixel_rgba<mode, 2, 1, 0, 3>(dst, src);
    } else if constexpr (src_order == ARGB) {
        blend_pixel_rgba<mode, 1, 2, 3, 0>(dst, src);
    }
}

template<blend_mode mode, pix_order src_order>
inline void blend_pixel_premul(uint8_t* dst, const uint8_t *src) {
    if constexpr (src_order == RGBA) {
        blend_pixel_rgba_premul<mode, 0, 1, 2, 3>(dst, src);
    } else if constexpr (src_order == BGRA) {
        blend_pixel_rgba_premul<mode, 2, 1, 0, 3>(dst, src);
    } else if constexpr (src_order == ARGB) {
        blend_pixel_rgba_premul<mode, 1, 2, 3, 0>(dst, src);
    }
}





template<blend_mode mode, pix_order src_order>
inline void blend_pixel_check(uint8_t *dst, const uint8_t *src) {
    uint16_t alpha = src[3];
    if (alpha == 0xFF) {
        memcpy(dst, src, 4);
        return;
    }
    if (alpha == 0x0) {
        return;
    }
    blend_pixel<mode, src_order>(dst, src);
}


// template<blend_mode mode>
// inline void blend_pixel_a8_premul(uint8_t *dst, const uint8_t *src, uint8_t a8) {
//     const int alpha = a8;
//     const int alpha_inv = 0xFF - alpha;
//     // blend_pixel_<mode, RGBA>(dst, src, alpha, alpha_inv);
// }
//
// template<blend_mode mode>
// inline void blend_pixel_a8_check(uint8_t *dst, const uint8_t *src, uint8_t a8) {
//     if (a8 == 0) {
//         return;
//     }
//     if (a8 == 0xFF) {
//         memcpy(dst, src, 4);
//     }
//     blend_pixel_a8<mode, RGBA>(dst, src);
// }


template<int num>
inline void fill_num(uint8_t *dst, const uint8_t *src) {
    for (int i = 0; i < num; ++i) {
        memcpy(dst, src, 4);
        dst += 4;
    }
}

template<blend_mode mode, pix_order src_order>
inline void blend_n(uint8_t *dst, const uint8_t *src, int count) {
    while (count-- > 0) {
        blend_pixel_check<mode, src_order>(dst, src);
        dst += 4;
        src += 4;
    }
}


template<blend_mode mode>
inline void blend_n_color_premul(uint8_t *dst, const uint8_t *color,int count, ptrdiff_t stride) {
    while (count -- > 0) {
        blend_pixel_premul<mode, RGBA>(dst, color);
        dst += stride;
    }
}

template<int N>
inline void blend_a8_num_color_rgb(uint8_t* dst, const uint8_t* src, uint8_t* rgba) {
    static_assert((N & (N - 1)) == 0, "N must be power of 2");
    if constexpr (N == 1) {
        blend_pixel_a8_color_rgb<SRC_OVER>(dst, src, rgba);
    } else {
        using T = std::conditional_t<
            (N <= 2),  uint16_t,
            std::conditional_t<
                (N <= 4),  uint32_t,
                uint64_t
            >
        >;

        T s;
        memcpy(&s, src, sizeof(T));

        if (s == 0)
            return;

        if (s == static_cast<T>(-1)) {
            fill_num<N>(dst, rgba);
        } else {
            constexpr int half = N / 2;
            blend_a8_num_color_rgb<half>(dst, src, rgba);
            blend_a8_num_color_rgb<half>(dst + 4 * half, src + half, rgba);
        }
    }
}


template<int N>
inline void blend_a8_num_color_rgba(uint8_t* dst, const uint8_t* src, uint8_t* rgba) {
    static_assert((N & (N - 1)) == 0, "N must be power of 2");
    if constexpr (N == 1) {
        blend_pixel_a8_color_rgba_premul<SRC_OVER>(dst,  src, rgba);
    } else {
        using T = std::conditional_t<
            (N <= 2),  uint16_t,
            std::conditional_t<
                (N <= 4),  uint32_t,
                uint64_t
            >
        >;

        T s;
        memcpy(&s, src, sizeof(T));

        if (s == 0)
            return;

        if (s == static_cast<T>(-1)) {
            blend_n_color_premul<SRC_OVER>(dst, rgba, N, 4);
        } else {
            constexpr int half = N / 2;
            blend_a8_num_color_rgba<half>(dst, src, rgba);
            blend_a8_num_color_rgba<half>(dst + 4 * half, src + half, rgba);
        }
    }
}



template<int N>
inline void blend_a8_num(uint8_t* dst, const uint8_t* src, uint8_t* rgba) {
    uint8_t a_color = rgba[3];
    if (a_color == 0)
        return;
    if (a_color == 0xFF) {
        blend_a8_num_color_rgb<N>(dst, src, rgba);
    } else {
        uint8_t color[4];
        premultiply_pixel<RGBA>(rgba, color);
        blend_a8_num_color_rgba<N>(dst, src, color);
    }
}


template<blend_mode mode>
inline void blend_a8_n(uint8_t *dst, const uint8_t *src, uint8_t *rgba, int count) {
    while (count >= 8) {
        blend_a8_num<8>(dst, src, rgba);
        dst += 32;   // 8 像素 * 4 字节
        src += 8;
        count -= 8;
    }
    if (count >= 4) {
        blend_a8_num<4>(dst, src, rgba);
        dst += 16;
        src += 4;
        count -= 4;
    }
    if (count >= 2) {
        blend_a8_num<2>(dst, src, rgba);
        dst += 8;
        src += 2;
        count -= 2;
    }
    if (count > 0) {
        blend_a8_num<1>(dst, src, rgba);
    }
}