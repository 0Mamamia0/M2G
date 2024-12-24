#pragma once

#include "Rect.h"
#include "color.h"
#include "PixelFormat.h"

#include "copy.h"
#include "blend.h"
#include "convert.h"

namespace m2g {


    static void drawPixels(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
                           int width, int height, int dst_format, int src_format) {
        if (dst_format != PixelFormatType::RGBA_8888) {
            // Not Impl;
            return;
        }
        switch (src_format) {
            case ALPHA_8:
            case GREY:
            case GREY_ALPHA:
                convertRect(dst, src, dst_format, src_format, dst_stride, src_stride, width, height);
                break;
            case RGB_888X:
            case XRGB_8888:
            case BGRX_8888:
                copyRect(dst, src, dst_format, src_format, dst_stride, src_stride, width, height);
                break;
            case RGBA_8888:
            case ARGB_8888:
            case BGRA_8888:
                blendRect(dst, src, dst_format, src_format, dst_stride, src_stride, width, height);
                break;

            default:
                break;
        }
    }


    static void drawPixels(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
                           int x, int y, const Rect &src_crop, int dst_format, int src_format) {
    }


    static void drawRGB(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride, int src_format,
                        int dataLength, int offset, int scanLength,
                        int x, int y, int width, int height, const Rect &clip,
                        bool processAlpha) {
    }

    static void drawPixels(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
                           int x, int y, const Rect &src_rect, const Rect &clip, int dst_format, int src_format) {
        if (clip.isEmpty() || src_rect.isEmpty())
            return;

        Rect dst_rect = Rect::makeXYWH(x, y, src_rect.getWidth(), src_rect.getHeight());
        Rect dst_rect_clip = dst_rect.intersect(clip);

        if (dst_rect_clip.isEmpty())
            return;

        int x0 = dst_rect_clip.left - x + src_rect.left;
        int y0 = dst_rect_clip.top - y + src_rect.top;
        int x1 = dst_rect_clip.right - x + src_rect.left;
        int y1 = dst_rect_clip.bottom - y + src_rect.top;

        uint8_t *dst_pixels = dst + (dst_rect_clip.top * dst_stride) + (
                dst_rect_clip.left * PixelFormat::getBytePerPixel(dst_format));
        uint8_t *src_pixels = src + (y0 * src_stride) + x0 * PixelFormat::getBytePerPixel(src_format);
        drawPixels(dst_pixels, src_pixels, dst_stride, src_stride, x1 - x0, y1 - y0, dst_format, src_format);
    }


    static void drawPixelsScaleRGB888X(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
                                       const Rect &region, float ratioX, float ratioY) {
        int srcY = -1;
        uint8_t *dst_row = dst + (dst_stride * region.top) + (region.left << 2);
        size_t row_bytes = region.getWidth() << 2;


        auto fun = [](uint8_t *dst, uint8_t *src, int l, int r, float ratio) {
            int srcX = -1;
            for (int x = l; x < r; ++x) {
                int sx = (int) ((x + 0.5f) * ratio);
                if (sx != srcX) {
                    srcX = sx;
                    *(uint32_t *) dst = *(uint32_t *) (src + (srcX << 2));
                } else {
                    *(uint32_t *) dst = *((uint32_t *) dst - 1);
                }
                dst += 4;
            }
        };


        for (int y = region.top; y < region.bottom; ++y) {
            int sy = (int) ((y + 0.5f) * ratioY);
            if (sy == srcY) {
                memcpy(dst_row, dst_row - dst_stride, row_bytes);
            } else {
                srcY = sy;
                fun(dst_row, src + (sy * src_stride), region.left, region.right, ratioX);
            }
            dst_row += dst_stride;
        }
    }

    static void drawPixelsScaleRGBA8888(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
                                        const Rect &region, float ratioX, float ratioY) {
        uint8_t *dst_row = dst + (dst_stride * region.top) + (region.left << 2);

        auto fun = [](uint8_t *dst, uint8_t *src, int l, int r, float ratio) {
            for (int x = l; x < r; ++x) {
                int sx = (int) ((x + 0.5f) * ratio);
                blend1px(dst, src + (sx << 2));
                dst += 4;
            }
        };

        for (int y = region.top; y < region.bottom; ++y) {
            int sy = (int) ((y + 0.5f) * ratioY);
            fun(dst_row, src + (sy * src_stride), region.left, region.right, ratioX);
            dst_row += dst_stride;
        }
    }


    static void drawPixelsScale(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
                                const Rect &region, float ratioX, float ratioY, int dst_format, int src_format) {
        if (dst_format != PixelFormatType::RGBA_8888) {
            // Not Impl;
            return;
        }

        switch (src_format) {
            case PixelFormatType::RGB_888X:
                drawPixelsScaleRGB888X(dst, src, dst_stride, src_stride, region, ratioX, ratioY);
                break;
            case PixelFormatType::RGBA_8888:
                drawPixelsScaleRGBA8888(dst, src, dst_stride, src_stride, region, ratioX, ratioY);
                break;
            case PixelFormatType::ALPHA_8:
            case PixelFormatType::GREY:
            case PixelFormatType::GREY_ALPHA:
            default:
                break;
        }
    }


    static void drawPixelsScale(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
                                const Rect &dst_rect, const Rect &src_rect, const Rect &clip, int dst_format,
                                int src_format) {
        if (clip.isEmpty() || src_rect.isEmpty() || dst_rect.isEmpty())
            return;
        Rect region = clip.intersect(dst_rect);

        if (region.isEmpty())
            return;


        int dst_width = dst_rect.getWidth();
        int dst_height = dst_rect.getHeight();
        int src_width = src_rect.getWidth();
        int src_height = src_rect.getHeight();

        float ratioX = (float) src_width / (float) dst_width;
        float ratioY = (float) src_height / (float) dst_height;


        region.left -= dst_rect.left;
        region.top -= dst_rect.top;
        region.right -= dst_rect.left;
        region.bottom -= dst_rect.top;

        dst += dst_rect.top * dst_stride + (dst_rect.left * PixelFormat::getBytePerPixel(dst_format));
        src += src_rect.top * src_stride + (src_rect.left * PixelFormat::getBytePerPixel(src_format));


        drawPixelsScale(dst, src, dst_stride, src_stride, region, ratioX, ratioY, dst_format, src_format);
    }


    static void drawPixelsA8_ARGB(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
                                  int x, int y, int width, int height, int dst_format, int color) {
        if (dst_format != PixelFormatType::RGBA_8888) {
            // Not Impl;
            return;
        }

        Color paintColor(color);
        uint8_t *dst_row = dst + (y * dst_stride) + (x << 2);
        while (height-- > 0) {
            blendA8(dst_row, src, paintColor.addr8(), width);
            src += src_stride;
            dst_row += dst_stride;
        }
    }

    static void drawPixelsA8_ARGB(uint8_t *dst, uint8_t *src, ptrdiff_t dst_stride, ptrdiff_t src_stride,
                                  int x, int y, int width, int height, int dst_format, int color, const Rect &clip) {
        if (dst_format != PixelFormatType::RGBA_8888) {
            // Not Impl;
            return;
        }


        Rect dst_rect = Rect::makeXYWH(x, y, width, height);
        Rect clip_rect = clip.intersect(dst_rect);
        if (clip_rect.isEmpty())
            return;


        Color paintColor(color);
        int x1 = clip_rect.left - x;
        int y1 = clip_rect.top - y;
        int w = clip_rect.getWidth();
        int h = clip_rect.getHeight();

        uint8_t *dst_row = dst + (clip_rect.top * dst_stride) + (clip_rect.left << 2);
        uint8_t *src_row = src + (y1 * src_stride) + x1;

        while (h-- > 0) {
            blendA8(dst_row, src_row, paintColor.addr8(), w);
            src_row += src_stride;
            dst_row += dst_stride;
        }
    }
}



