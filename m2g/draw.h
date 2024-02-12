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


inline void copyLTR(uint8_t* dst, uint8_t* src, int pixel) {
    memcpy(dst, src, pixel << 2);
}



inline void copyRTL(uint8_t* dst, uint8_t* rgba, int pixel) {
    uint32_t* begin32 = (uint32_t*)dst;
    uint32_t* rgba32 = (uint32_t*)rgba;
    while (pixel -- > 0) {
        *begin32++ = *rgba32--;
    }
}



inline void copyTTB(uint32_t* begin, int width, int step, uint32_t* rgba) {
    while (width -- > 0) {
        *begin++ = *rgba;
        rgba += step;
    }
}

inline void copyBTT(uint32_t* begin, int width, int step, uint32_t* rgba) {
    while (width-- > 0) {
        *begin++ = *rgba;
        rgba -= step;
    }
}


inline void copyRect(uint8_t* dst, uint8_t* src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height) {
    while (height-- > 0) {
        copyLTR(dst, src, width);
        dst += dst_stride;
        src += src_stride;
    }
}



namespace piv {

    template<bool flip>
    void line_copy(uint8_t* dst, uint8_t* src, int pixel) {
        if(!flip) {
            memcpy(dst, src, pixel << 2);
        } else {
            uint32_t* dst32 = (uint32_t*)dst;
            uint32_t* src32 = (uint32_t*)src + pixel;
            while (pixel -- > 0) {
                *dst32++ = *--src32;
            }
        }
    }

    void line_move(uint8_t* dst, uint8_t* src, int pixel) {
        memmove(dst, src, pixel << 2);
    }

    template<bool x_flip, bool y_flip>
    void rect_copy(uint8_t* dst, uint8_t* src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int width, int height) {
        if(y_flip) {
            src = src + (height - 1) * src_stride;
            src_stride = -src_stride;
        }

        while (height-- > 0) {
            line_copy<x_flip>(dst, src, width);
            dst += dst_stride;
            src += src_stride;
        }
    }




    void rect_move(uint8_t* dst, uint8_t* src, ptrdiff_t stride, int width, int height) {
//        if(flip) {
//            ptrdiff_t offset = (height - 1) * stride;
//            dst = dst + offset;
//            src = src + offset;
//            stride = -stride;
//        }

        while (height-- > 0) {
            line_move(dst, src, width);
            dst += stride;
            src += stride;
        }
    }



}










inline void copyArea(uint8_t* pixels, ptrdiff_t stride, const Rect& dst_area, const Rect& src_area) {
    assert(dst_area.getWidth() == src_area.getWidth());
    assert(dst_area.getHeight() == src_area.getHeight());
    assert(dst_area.getWidth() > 0 && dst_area.getHeight() > 0);

    int width = dst_area.getWidth();
    int height = dst_area.getHeight();
    int src_offset = (src_area.top * stride) + (src_area.left * 4);
    int dst_offset = (dst_area.top * stride) + (dst_area.left * 4);
    uint8_t* dst = pixels + dst_offset;
    uint8_t* src = pixels + src_offset;


    if(src_area.contain(dst_area)) {
        if(src_area.top == dst_area.top) {
           piv::rect_move(dst, src, stride, width, height);
        } else if(src_area.top < dst_area.top) {
            dst = dst + (height - 1) * stride;
            src = src + (height - 1) * stride;
            stride = -stride;
            piv::rect_copy<false, false>(dst, src, stride, stride,  width, height);
        } else {
            piv::rect_copy<false, false>(dst, src, stride, stride, width, height);
        }
    } else {
        piv::rect_copy<false, false>(dst, src, stride, stride, width,height);
    }

}



inline void blend1px(void* dst_, void* src_) {
    uint8_t * dst = (uint8_t*) dst_;
    uint8_t * src = (uint8_t*) src_;


    int alpha = src[3];

    if(alpha == 0x0) {
        return;
    }

    if(alpha == 0xFF) {
        memcpy(dst, src, 4);
        return;
    }

    int invAlpha = 0xFF - alpha;
    dst[0] = (src[0] * alpha + dst[0] * invAlpha) >> 8;
    dst[1] = (src[1] * alpha + dst[1] * invAlpha) >> 8;
    dst[2] = (src[2] * alpha + dst[2] * invAlpha) >> 8;
    dst[3] = 0xFF;
}



inline void blend_n(void* dst_, void* src_, int count) {
    uint32_t* dst = (uint32_t*) dst_;
    uint32_t* src = (uint32_t*) src_;

    if(count > 0) {
        int time = count / 8;
        int c = count % 8;
        switch (c) {
            do {
                case 8:  blend1px(dst ++, src ++);  // 0
                case 7:  blend1px(dst ++, src ++);
                case 6:  blend1px(dst ++, src ++);
                case 5:  blend1px(dst ++, src ++);
                case 4:  blend1px(dst ++, src ++);
                case 3:  blend1px(dst ++, src ++);
                case 2:  blend1px(dst ++, src ++);
                case 1:  blend1px(dst ++, src ++);
                default:;
            } while (time -- > 0);
        }
    }


//    for (int i = 0 ; i  < count; i++ ) {
//        blend1px(dst, src);
//        dst += 4;
//        src += 4;
//    }
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

static void blendA8_8(uint8_t* dst, const uint8_t* src, uint8_t* rgba) {
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
        blendA8(dst,      *src ++, rgba);
        blendA8(dst + 4,  *src ++, rgba);
        blendA8(dst + 8,  *src ++, rgba);
        blendA8(dst + 12, *src ++, rgba);
        blendA8(dst + 16, *src ++, rgba);
        blendA8(dst + 20, *src ++, rgba);
        blendA8(dst + 24, *src ++, rgba);
        blendA8(dst + 28, *src, rgba);

        // blendA8_span(dst, src, rgba, 8);
    }
}


inline void blendA8(uint8_t* dst, uint8_t* src, uint8_t* rgba, int count) {
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
#include "arm_neon.h"
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

    blend_n(dst, src, num);
}


#else
inline void blend(uint8_t* dst, uint8_t* src, int count) {
    blend_n(dst, src, count);
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
