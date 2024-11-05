#pragma once


inline void copyLTR(uint8_t *dst, uint8_t *src, int pixel) {
    memcpy(dst, src, pixel << 2);
}


inline void copyRTL(uint8_t *dst, uint8_t *rgba, int pixel) {
    auto *begin32 = (uint32_t *) dst;
    auto *rgba32 = (uint32_t *) rgba;
    while (pixel-- > 0) {
        *begin32++ = *rgba32--;
    }
}


inline void copyTTB(uint32_t *begin, int width, int step, uint32_t *rgba) {
    while (width-- > 0) {
        *begin++ = *rgba;
        rgba += step;
    }
}

inline void copyBTT(uint32_t *begin, int width, int step, uint32_t *rgba) {
    while (width-- > 0) {
        *begin++ = *rgba;
        rgba -= step;
    }
}


inline void copyRect_(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
                      int width,
                      int height) {
    while (height-- > 0) {
        copyLTR(dst, src, width);
        dst += dst_stride;
        src += src_stride;
    }
}


namespace m2g::piv {
    template<int R, int G, int B, int A>
    void pix_copy(uint8_t *dst, const uint8_t *src, int n) {
        while (n-- > 0) {
            dst[0] = src[R];
            dst[1] = src[G];
            dst[2] = src[B];
            dst[3] = 0xFF;
            dst += 4;
            src += 4;
        }
    }



    template<int src_order>
    void pix_copy_order(uint8_t *dst, uint8_t *src, size_t size) {
        if constexpr(src_order == pix_order::ARGB) {
            pix_copy<1, 2, 3, 0>(dst, src, size);
        } else if constexpr (src_order == pix_order::BGRA) {
            pix_copy<2, 1, 0, 3>(dst, src, size);
        } else if constexpr (src_order == pix_order::RGBA) {
            memcpy(dst, src, size << 2);
        }
    }

    template<int src_order>
    void pix_copy_order(int32_t *dst, uint8_t *src, int n) {
        if constexpr (src_order == pix_order::ARGB) {
            if constexpr (std::endian::native == std::endian::little) {
                pix_copy_order<pix_order::BGRA>((uint8_t*)dst, src, n);
            } else {
                pix_copy_order<pix_order::ARGB>((uint8_t*)dst, src, n);
            }
        }
    }

    template<int src_order>
    void pix_copy_rect_order(uint8_t *dst, uint8_t *src,
                             ptrdiff_t dst_stride, ptrdiff_t src_stride,
                             int width, int height) {
        while (height-- > 0) {
            pix_copy_order<src_order>(dst, src, width);
            dst += dst_stride;
            src += src_stride;
        }
    }
}


inline void copyRect(uint8_t *dst, uint8_t *src,
                     int dst_format, int src_format,
                     ptrdiff_t dst_stride, ptrdiff_t src_stride,
                     int width, int height) {
    if (dst_format != PixelFormatType::RGBA_8888) {
        // Not Impl;
        return;
    }

    switch (src_format) {
        case RGB_888X:
            m2g::piv::blend_rect_order<pix_order::RGBA>(dst, src, dst_stride, src_stride, width, height);
            break;
        case XRGB_8888:
            m2g::piv::blend_rect_order<pix_order::ARGB>(dst, src, dst_stride, src_stride, width, height);
            break;
        case BGRX_8888:
            m2g::piv::blend_rect_order<pix_order::BGRA>(dst, src, dst_stride, src_stride, width, height);
            break;
        default:
            break;
    }
}



