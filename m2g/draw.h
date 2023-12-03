//
// Created by Admin on 2023/10/16.
//

#ifndef __DRAW_H__
#define __DRAW_H__


#include "color.h"
#include "Rect.h"
#include "PixelFormat.h"

static inline void blitV(uint8_t* begin, int height, ptrdiff_t stride, uint8_t* rgba) {
    while (height-- > 0) {
        memcpy(begin, rgba, 4);
        begin += stride;
    }
}

inline void blitH(uint8_t* begin, int width, uint8_t* rgba) {
    while (width -- > 0) {
        memcpy(begin, rgba, 4);
        begin += 4;
    }
}

inline void blitRect(uint8_t* begin, int width, int height, ptrdiff_t stride, uint8_t* rgba) {
    while (height-- > 0) {
        blitH(begin, width, rgba);
        begin += stride;
    }
}




inline void copyLTR(uint8_t* begin, int width, uint8_t* rgba) {
    memcpy(begin, rgba, width * 4);
}

inline void copyRTL(uint8_t* begin, int width, uint8_t* rgba) {
    uint32_t* begin32 = (uint32_t*)begin;
    uint32_t* rgba32 = (uint32_t*)rgba;
    while (width -- > 0) {
        *begin32++ = *rgba32--;
    }
}



//static inline void copyTTB(uint32_t* begin, int width, int step, uint32_t* rgba) {
//    while (width -- > 0) {
//        *begin++ = *rgba;
//        rgba += step;
//    }
//}
//
//static inline void copyBTT(uint32_t* begin, int width, int step, uint32_t* rgba) {
//    while (width-- > 0) {
//        *begin++ = *rgba;
//        rgba -= step;
//    }
//}


inline void copyRect(uint8_t* dst, uint8_t* src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height) {
    while (height-- > 0) {
        copyLTR(dst, width, src);
        dst += dst_stride;
        src += src_stride;
    }
}

inline void copyRectXF(uint8_t* dst, uint8_t* src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height) {
    while (height-- > 0) {
        copyRTL(dst, width, src);
        dst += dst_stride;
        src += src_stride;
    }
}

inline void copyRectYF(uint8_t* dst, uint8_t* src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height) {
    src = src + (src_stride * (height - 1));
    while (height-- > 0) {
        copyLTR(dst, width, src);
        dst += dst_stride;
        src -= src_stride;
    }
}

inline void copyRectXYF(uint8_t* dst, uint8_t* src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height) {
    src = src + (src_stride * (height - 1));
    while (height-- > 0) {
        copyRTL(dst, width, src);
        dst += dst_stride;
        src -= src_stride;
    }
}




inline void copyArea(uint8_t* dst,ptrdiff_t stride, int format, const Rect& dst_area, const Rect& src_area) {
    assert(dst_area.getWidth() == src_area.getWidth());
    assert(dst_area.getHeight() == src_area.getHeight());
    assert(dst_area.getWidth() > 0 && dst_area.getHeight() > 0);

    int copy_width = dst_area.getWidth();
    int copy_height = dst_area.getHeight();


    if(src_area.contain(dst_area)) {


        int bpp = PixelFormat::getBytePerPixel(format);
        const int src_offset = (src_area.top * stride) + (src_area.left * bpp);
        const int dst_offset = (dst_area.top * stride) + (dst_area.left * bpp);


        if(src_area.top == dst_area.top) {
            for (int row = 0; row < copy_height; row++) {
                uint8_t* src_ptr = dst + src_offset + (row * stride);
                uint8_t* dst_ptr = dst + dst_offset + (row * stride);
                memmove(dst_ptr, src_ptr, copy_width * bpp);
            }
        } else if(src_area.top < dst_area.top) {
            for (int row = copy_height - 1; row >= 0; row--)  {
                uint8_t* src_ptr = dst + src_offset + (row * stride);
                uint8_t* dst_ptr = dst + dst_offset + (row * stride);
                memcpy(dst_ptr, src_ptr, copy_width * bpp);
            }
        } else {
            for (int row = 0; row < copy_height; row++) {
                uint8_t* src_ptr = dst + src_offset + (row * stride);
                uint8_t* dst_ptr = dst + dst_offset + (row * stride);
                memcpy(dst_ptr, src_ptr, copy_width * bpp);
            }
        }


    } else {
        int bpp = PixelFormat::getBytePerPixel(format);
        const int src_offset = (src_area.top * stride) + (src_area.left * bpp);
        const int dst_offset = (dst_area.top * stride) + (dst_area.left * bpp);

        for (int row = 0; row < copy_height; row++) {
            uint8_t* src_ptr = dst + src_offset + (row * stride);
            uint8_t* dst_ptr = dst + dst_offset + (row * stride);
            memcpy(dst_ptr, src_ptr, copy_width * bpp);
        }
    }

}



inline void blend1px(uint8_t* dst, uint8_t* src) {
    int alpha = src[3];
    if(alpha == 0xFF) {
        memcpy(dst, src, 4);
    } else if(alpha != 0x0){
        int invAlpha = 0xFF - alpha;
        dst[0] = (src[0] * alpha + dst[0] * invAlpha) >> 8;
        dst[1] = (src[1] * alpha + dst[1] * invAlpha) >> 8;
        dst[2] = (src[2] * alpha + dst[2] * invAlpha) >> 8;
        dst[3] = 0xFF;
    }
}



inline void blend_span(uint8_t* dst, uint8_t* src, int count) {
    for (int i = 0 ; i  < count; i++ ) {
        blend1px(dst, src);
        dst += 4;
        src += 4;
    }
}




inline void blendA8(uint8_t* dst, uint8_t a8, uint8_t* rgba) {
    if (a8 == 0xFF) {
        memcpy(dst, rgba, 4);
    } else if (a8 != 0x0) {
        int alpha = (int)a8;
        int invAlpha = 0xFF - alpha;

        dst[0] = (uint8_t)((rgba[0] * alpha + dst[0] * invAlpha) >> 8);
        dst[1] = (uint8_t)((rgba[1] * alpha + dst[1] * invAlpha) >> 8);
        dst[2] = (uint8_t)((rgba[2] * alpha + dst[2] * invAlpha) >> 8);
        dst[3] = 0xFF;
    }
}


inline void blendA8_span(uint8_t* dst, const uint8_t* src, uint8_t* rgba, int count) {
    for (int i = 0; i < count; ++i) {
        blendA8(dst, *src, rgba);
        src ++ ;
        dst += 4;
    }
}

void blendA8_8(uint8_t* dst, const uint8_t* src, uint8_t* rgba) {
    uint64_t s = *(uint64_t*)src;
    if(s == 0xFFFFFFFFFFFFFFFF) {
        uint32_t* d = (uint32_t*)(dst);
        uint32_t color = *(uint32_t*)(rgba);

        *d++ = color;
        *d++ = color;
        *d++ = color;
        *d++ = color;

        *d++ = color;
        *d++ = color;
        *d++ = color;
        *d++ = color;

    } else if(s != 0) {
        blendA8_span(dst, src, rgba, 8);
    }
}


void blendA8(uint8_t* dst, uint8_t* src, uint8_t* rgba, int count) {
    int e_c = count / 8;
    int r_c = count % 8;
    for (int i = 0; i < e_c; ++i) {
        blendA8_8(dst, src, rgba);
        src += 8;
        dst += 32;
    }
    blendA8_span(dst, src, rgba, r_c);
}






#ifdef __SWITCH__
static inline uint8x8_t blend_32_neon(uint8x8_t src, uint8x8_t dst, uint16x8_t alpha) {
    int16x8_t src_wide, dst_wide;

    // int16_t src = cast<int16_t>(src);
    // int16_t dst = cast<int16_t>(dst);
    // int16_t alpha = cast<int16_t>(alpha);
    // dst += (src - dst) * alpha / 0xFF;
    // return cast<uint8_t>(dst);


    src_wide = vreinterpretq_s16_u16(vmovl_u8(src));
    dst_wide = vreinterpretq_s16_u16(vmovl_u8(dst));

    src_wide = (src_wide - dst_wide) * vreinterpretq_s16_u16(alpha);
    dst_wide += vshrq_n_s16(src_wide, 8);

    return vmovn_u16(vreinterpretq_u16_s16(dst_wide));
}


static inline void blend_rgba_neon(uint8_t* dst, uint8_t* src, int size) {
    uint8x8x4_t vdst;
    uint8x8x4_t vsrc;

    for (int i = 0; i < size; i += 8) {
        vdst = vld4_u8(dst);
        vsrc = vld4_u8(src);

        uint16x8_t a = vmovl_u8(vsrc.val[3]);
        vdst.val[0] = blend_32_neon(vsrc.val[0], vdst.val[0], a);
        vdst.val[1] = blend_32_neon(vsrc.val[1], vdst.val[1], a);
        vdst.val[2] = blend_32_neon(vsrc.val[2], vdst.val[2], a);
        vdst.val[3] = vdup_n_u8(0xFF);;
        vst4_u8(dst, vdst);

        src += 32;
        dst += 32;
    }
}


static inline void blend(uint8_t* dst, uint8_t* src, int count) {
    int num = count % 8;
    int offset = count - num;

    blend_rgba_neon(dst, src, offset);
    dst += (offset << 2);
    src += (offset << 2);

    blend_span(dst, src, num);
}


#else
inline void blend(uint8_t* dst, uint8_t* src, int count) {
    blend_span(dst, src, count);
}

#endif



static inline void blendRect(uint8_t* dst, uint8_t* src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height) {
    while (height-- > 0) {
        blend(dst, src, width);
        dst += dst_stride;
        src += src_stride;
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







#endif //__DRAW_H__
