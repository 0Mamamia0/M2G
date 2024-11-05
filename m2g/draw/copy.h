#pragma once

#define RGBA_ORDER 0,1,2,3
#define ARGB_ORDER 1,2,3,0
#define BGRA_ORDER 2,1,0,3


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

    template<int dst_r, int dst_g, int dst_b, int dst_a, int src_r, int src_g, int src_b, int src_a>
    void pix_copy(uint8_t *dst, const uint8_t *src, size_t size) {
        while (size -- > 0) {
            dst[dst_r] = src[src_r];
            dst[dst_g] = src[src_g];
            dst[dst_b] = src[src_b];
            dst[dst_a] = src[src_a];
            dst += 4;
            src += 4;
        }
    }



    template<int R, int G, int B, int A>
    void pix_copy_no_alpha(uint8_t *dst, const uint8_t *src, size_t size) {
        while (size -- > 0) {
            dst[0] = src[R];
            dst[1] = src[G];
            dst[2] = src[B];
            dst[3] = 0xFF;
            dst += 4;
            src += 4;
        }
    }



    template<int src_order>
    void pix_copy_order_no_alpha(uint8_t *dst, uint8_t *src, size_t size) {
        if constexpr(src_order == pix_order::ARGB) {
            pix_copy_no_alpha<1, 2, 3, 0>(dst, src, size);
        } else if constexpr (src_order == pix_order::BGRA) {
            pix_copy_no_alpha<2, 1, 0, 3>(dst, src, size);
        } else if constexpr (src_order == pix_order::RGBA) {
            memcpy(dst, src, size << 2);
        }
    }


    template<int dst_order, int src_order>
    void pix_copy_order(uint8_t *dst, uint8_t *src, size_t size) {

        if constexpr(src_order == dst_order) {
            memcpy(dst, src, size << 2);
        } else if constexpr (dst_order == RGBA){
            if constexpr (src_order == pix_order::BGRA) {
                pix_copy<RGBA_ORDER, BGRA_ORDER>(dst, src, size);
            } else if constexpr (src_order == pix_order::ARGB) {
                pix_copy<RGBA_ORDER, ARGB_ORDER>(dst, src, size);
            }
        } else if constexpr (dst_order == ARGB) {
            if constexpr (src_order == pix_order::BGRA) {
                pix_copy<ARGB_ORDER, BGRA_ORDER>(dst, src, size);
            } else if constexpr (src_order == pix_order::RGBA) {
                pix_copy<ARGB_ORDER, RGBA_ORDER>(dst, src, size);
            }
        } else if constexpr (dst_order == pix_order::BGRA) {
            if constexpr(src_order == pix_order::ARGB) {
                pix_copy<BGRA_ORDER, ARGB_ORDER>(dst, src, size);
            } if constexpr (src_order == pix_order::RGBA) {
                pix_copy<BGRA_ORDER, RGBA_ORDER>(dst, src, size);
            }
        }
    }



    template<int src_order>
    void pix_copy_order_no_alpha(int32_t *dst, uint8_t *src, int n) {
        if constexpr (src_order == pix_order::ARGB) {
            if constexpr (std::endian::native == std::endian::little) {
                pix_copy_order_no_alpha<pix_order::BGRA>((uint8_t*)dst, src, n);
            } else {
                pix_copy_order_no_alpha<pix_order::ARGB>((uint8_t*)dst, src, n);
            }
        }
    }

    template<int src_order>
    void pix_copy_rect_order_no_alpha(uint8_t *dst, uint8_t *src,
                             ptrdiff_t dst_stride, ptrdiff_t src_stride,
                             int width, int height) {
        while (height-- > 0) {
            pix_copy_order_no_alpha<src_order>(dst, src, width);
            dst += dst_stride;
            src += src_stride;
        }
    }

    template<int dst_order, int src_order>
    void pix_copy_rect_order(uint8_t *dst, uint8_t *src,
                             ptrdiff_t dst_stride, ptrdiff_t src_stride,
                             int width, int height) {
        while (height-- > 0) {
            pix_copy_order<dst_order, src_order>(dst, src, width);
            dst += dst_stride;
            src += src_stride;
        }

    }


    template<int dst_order, int src_order>
    void pix_copy_rect_order(uint32_t *dst, uint8_t *src,
                             ptrdiff_t dst_stride, ptrdiff_t src_stride,
                             int width, int height) {

        if constexpr (std::endian::native == std::endian::little) {
            if constexpr(dst_order == pix_order::BGRA) {
                pix_copy_rect_order<pix_order::ARGB, src_order>((uint8_t*)dst, src, dst_stride, src_stride, width, height);
            } else if constexpr (dst_order == pix_order::ARGB) {
                pix_copy_rect_order<pix_order::BGRA, src_order>((uint8_t*)dst, src, dst_stride, src_stride, width, height);
            } else if constexpr (dst_order == pix_order::RGBA) {

            }
        } else {
            pix_copy_rect_order<dst_order, src_order>((uint8_t*)dst, src, dst_stride, src_stride, width, height);
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
            m2g::piv::pix_copy_rect_order_no_alpha<pix_order::RGBA>(dst, src, dst_stride, src_stride, width, height);
            break;
        case XRGB_8888:
            m2g::piv::pix_copy_rect_order_no_alpha<pix_order::ARGB>(dst, src, dst_stride, src_stride, width, height);
            break;
        case BGRX_8888:
            m2g::piv::pix_copy_rect_order_no_alpha<pix_order::BGRA>(dst, src, dst_stride, src_stride, width, height);
            break;
        default:
            break;
    }
}


#undef RGBA_ORDER
#undef ARGB_ORDER
#undef BGRA_ORDER


