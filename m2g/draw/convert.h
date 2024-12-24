#pragma once



namespace m2g::piv {
//    template<int dst_r, int dst_g, int dst_b, int dst_a, int src_r, int src_g, int src_b, int src_a>
    inline void pix_convert_grey8(uint8_t *dst, const uint8_t *src, size_t size) {
        while (size -- > 0) {
            uint8_t a = src[0];
            dst[0] = a;
            dst[1] = a;
            dst[2] = a;
            dst[3] = 0xFF;
            dst += 4;
            src += 1;
        }
    }

    inline void pix_convert_grey_alpha(uint8_t *dst, const uint8_t *src, size_t size) {
        while (size -- > 0) {
            auto grey = (uint16_t)src[0];
            auto a = (uint16_t)src[1];
            auto ia = (uint16_t)(0xFF - a);
            auto value = grey * a;

            dst[0] = (value + dst[0] * ia) >> 8;
            dst[1] = (value + dst[1] * ia) >> 8;
            dst[2] = (value + dst[2] * ia) >> 8;
            dst[3] = 0xFF;
            dst += 4;
            src += 2;
        }
    }


    inline void pix_convert_grey8(uint8_t *dst, const uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height) {
        while (height -- > 0) {
            pix_convert_grey8(dst, src, width);
            dst += dst_stride;
            src += src_stride;
        }
    }

    inline void pix_convert_grey_alpha(uint8_t *dst, const uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height) {
        while (height -- > 0) {
            pix_convert_grey_alpha(dst, src, width);
            dst += dst_stride;
            src += src_stride;
        }
    }
}

inline void convertRect(uint8_t *dst, uint8_t *src,
                     int dst_format, int src_format,
                     ptrdiff_t dst_stride, ptrdiff_t src_stride,
                     int width, int height) {
    if (dst_format != PixelFormatType::RGBA_8888) {
        // Not Impl;
        return;
    }

    switch (src_format) {
        case ALPHA_8:
        case GREY:
            m2g::piv::pix_convert_grey8(dst, src, dst_stride, src_stride, width, height);
            break;
        case GREY_ALPHA:
            m2g::piv::pix_convert_grey_alpha(dst, src, dst_stride, src_stride, width, height);
            break;
        default:
            break;
    }
}