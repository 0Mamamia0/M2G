#include <cstring>
#include <algorithm>
#include <cmath>
#include <bit>

#include "draw/draw.h"

#include "color.h"

#include "m2g-def.h"
#include "Image.h"
#include "Font.h"
#include "PixelBuffer.h"
#include "Graphics.h"

namespace {
    using namespace m2g;

    int anchorX(int anchor, int x, int w) {
        switch (anchor & (LEFT | RIGHT | HCENTER)) {
            case RIGHT:
                return x - w;
            case HCENTER:
                return x - (w >> 1);
            case LEFT:
            default:
                return x;
        }
    }


    int anchorY(int anchor, int y, int h) {
        switch (anchor & (TOP | BOTTOM | VCENTER)) {
            case BOTTOM:
                return y - h;
            case VCENTER:
                return y - (h >> 1);
            case TOP:
            default:
                return y;
        }
    }

    int anchorTextX(int anchor, int x, int width, const Font &font) {
        switch (anchor & (LEFT | RIGHT | HCENTER)) {
            case RIGHT:
                return x - width;
            case HCENTER:
                return x - (width >> 1);
            case LEFT:
            default:
                return x;
        }
    }

    int anchorTextY(int anchor, int y, const Font &font) {
        const FontMetrics &metrics = font.getFontMetrics();
        switch (anchor & (TOP | BOTTOM | BASELINE | VCENTER)) {
            case BOTTOM:
                // TODO check
                y -= (int) (metrics.height / 2.f);
            case VCENTER:
                y += (int) ((metrics.ascent + metrics.descent) / 2.f);
            case BASELINE:
                return y;
            case TOP:
            default:
                return y + (int) metrics.ascent;
        }
    }

    bool ValidImage(Image *image, int x, int y, int w, int h) {
        return image != nullptr
               && x >= 0
               && y >= 0
               && w > 0
               && h > 0
               && x + w <= image->getWidth()
               && y + h <= image->getHeight();
    }
}


namespace m2g {
    Graphics::Graphics(Image *image)
            : Graphics(image->getPixelBuffer()) {
    }


    Graphics::Graphics(std::shared_ptr<PixelBuffer> buffer)
            : width(buffer->getWidth()), height(buffer->getHeight()), clip{width, height}, translation{0, 0},
              paintColor{0xFF, 0xFF, 0xFF, 0xFF}, buffer(std::move(buffer)) {
    }

    Graphics::Graphics(Image *image, int width, int height)
            : Graphics(image->getPixelBuffer(), width, height) {
    }


    // TODO Fixed clip to max
    Graphics::Graphics(std::shared_ptr<PixelBuffer> buffer, int width_, int height_)
            : Graphics(std::move(buffer)) {
        setClip(0, 0, width_, height_);
    }


    Graphics::Graphics(const Graphics &other)
            : width(other.width), height(other.height), clip(other.clip), translation(other.translation),
              paintColor(other.paintColor), buffer(other.buffer) {
    }


    Graphics::Graphics(Graphics &&other) noexcept
            : width(other.width), height(other.height), clip(other.clip), translation(other.translation),
              paintColor(other.paintColor), buffer(std::move(other.buffer)) {
    }


    void Graphics::setColor(const Color &color) {
        this->paintColor = color;
    }

    void Graphics::setColor(int color) {
        this->paintColor = color;
    }

    void Graphics::translate(int x, int y) {
        translation.offset(x, y);
    }

    int Graphics::getTranslateX() const {
        return translation.x;
    }

    int Graphics::getTranslateY() const {
        return translation.y;
    }

    void Graphics::setClip(int x, int y, int width_, int height_) {
        // x, y, width, height  int translate space
        if (width_ <= 0 || height_ <= 0) {
            clip.setZero();
            return;
        }

        int x1 = x + translation.x;
        int y1 = y + translation.y;
        int x2 = x1 + width_;
        int y2 = y1 + height_;

        //x1:[0-int_max]
        if (x1 < 0) {
            // if overflow  set it to width , if not just 0;
            x1 = (x > 0 && translation.x > 0) ? this->width : 0;
        }

        //y1:[0-int_max]
        if (y1 < 0) {
            y1 = (y > 0 && translation.y > 0) ? this->height : 0;
        }

        //x2:[0-int_max]
        if (x2 < 0) {
            x2 = (x > 0 && translation.x > 0) ? this->width : 0;
        }

        //y2:[0-int_max]
        if (y2 < 0) {
            y2 = (y > 0 && translation.y > 0) ? this->height : 0;
        }

        if ((x1 >= this->width) || (y1 >= this->height)) {
            clip.setZero();
        } else {
            int cw = std::clamp(x2 - x1, 0, this->width);
            int ch = std::clamp(y2 - y1, 0, this->height);
            clip.setXYWH(x1, y1, cw, ch);
        }
    }

    void Graphics::clipRect(int x, int y, int width_, int height_) {
        if (width_ <= 0 || height_ <= 0) {
            clip.setZero();
            return;
        }

        int x1 = x + translation.x;
        int y1 = y + translation.y;
        int x2 = x1 + width_;
        int y2 = y1 + height_;


        if (x1 < 0) {
            // if overflow  set it to width , if not just 0;
            x1 = (x > 0 && translation.x > 0) ? this->width : 0;
        }

        //y1:[0-int_max]
        if (y1 < 0) {
            y1 = (y > 0 && translation.y > 0) ? this->height : 0;
        }

        //x2:[0-int_max]
        if (x2 < 0) {
            x2 = (x > 0 && translation.x > 0) ? this->width : 0;
        }

        //y2:[0-int_max]
        if (y2 < 0) {
            y2 = (y > 0 && translation.y > 0) ? this->height : 0;
        }

        Rect tmp = {x1, y1, x2, y2};

        if (clip.contain(tmp)) {
            Rect rect = clip.intersect(tmp);
            if (rect.isEmpty()) {
                clip.setZero();
            } else {
                clip.set(rect);
            }
        } else {
            clip.setZero();
        }
    }


    int Graphics::getClipX() const {
        return clip.getX() - translation.x;
    }


    int Graphics::getClipY() const {
        return clip.getY() - translation.y;
    }

    int Graphics::getClipWidth() const {
        return clip.getWidth();
    }

    int Graphics::getClipHeight() const {
        return clip.getHeight();
    }


    void Graphics::drawPixel(int x, int y) {
        *buffer->addr<Color *>(x, y) = paintColor;
    }

    void Graphics::setPixel(int x, int y, Color color) {
        *buffer->addr<Color *>(x, y) = color;
    }


    void Graphics::drawImage(Image *image, int x, int y, int anchor) {
        drawImage(image, 0, 0, image->getWidth(), image->getHeight(), x, y, anchor);
    }


    void Graphics::drawImage(Image *image, int x_src, int y_src, int w_src, int h_src, int x_dst, int y_dst,
                             int anchor) {
        if (!ValidImage(image, x_src, y_src, w_src, h_src)) {
            return;
        }
        translatePoint(x_dst, y_dst, w_src, h_src, anchor);
        PixelBuffer &dst_buffer = *buffer;
        PixelBuffer &src_buffer = image->getPixelBufferRef();
        drawPixels(dst_buffer.addr<uint8_t *>(),
                   src_buffer.addr<uint8_t *>(),
                   dst_buffer.getRowBytes(),
                   src_buffer.getRowBytes(),
                   x_dst, y_dst,
                   Rect::makeXYWH(x_src, y_src, w_src, h_src),
                   clip.getDeviceClipBounds(),
                   dst_buffer.getFormat(),
                   src_buffer.getFormat());
    }


    void Graphics::drawRegion(Image *image, int x_src, int y_src, int w_src, int h_src, int transform, int x_dst,
                              int y_dst, int anchor) {
        if (transform == 0) {
            drawImage(image, x_src, y_src, w_src, h_src, x_dst, y_dst, anchor);
        } else {
            int x0 = 0;
            int y0 = 0;


            int x_step = 0;
            int y_step = 0;

            int w_dst, h_dst;
            int w_dst_clip, h_dst_clip;


            if (!ValidImage(image, x_src, y_src, w_src, h_src)) {
                return;
            }

            const Rect bounds = clip.getDeviceClipBounds();
            if (bounds.isEmpty()) {
                return;
            }

            auto &src_pixels = image->getPixelBufferRef();
            int src_stride = src_pixels.getStride();
            int dst_stride = buffer->getStride();

            if (transform & TRANSFORM_INVERTED_AXES) {
                w_dst = h_src;
                h_dst = w_src;
            } else {
                w_dst = w_src;
                h_dst = h_src;
            }


            translatePoint(x_dst, y_dst, w_dst, h_dst, anchor);
            const Rect src_rect = Rect::makeXYWH(x_src, y_src, w_src, h_src);
            const Rect dst_rect = Rect::makeXYWH(x_dst, y_dst, w_dst, h_dst);
            const Rect dst_rect_clip = bounds.intersect(dst_rect);

            if (dst_rect_clip.isEmpty()) {
                return;
            }

            int x_offset = dst_rect_clip.left - x_dst;
            int y_offset = dst_rect_clip.top - y_dst;

            // if(transform & TRANSFORM_X_FLIP) {
            //     x0 = w_src - 1;
            // } else {
            //     x0 = 0;
            // }
            //
            //
            // if(transform & TRANSFORM_Y_FLIP) {
            //     y0 = h_src - 1;
            // } else {
            //     y0 = 0;
            // }

            switch (transform) {
                case TRANS_NONE:
                    x_step = 1;
                    y_step = src_stride;
                    x0 = x_offset;
                    y0 = y_offset;
                    break;
                case TRANS_MIRROR:
                    x_step = -1;
                    y_step = src_stride;
                    x0 = w_src - x_offset - 1;
                    y0 = y_offset;
                    break;
                case TRANS_ROT180:
                    x_step = -1;
                    y_step = -src_stride;
                    x0 = w_src - y_offset - 1;
                    y0 = h_src - x_offset - 1;
                    break;
                case TRANS_MIRROR_ROT180:
                    x_step = 1;
                    y_step = -src_stride;
                    x0 = x_offset;
                    y0 = h_src - y_offset - 1;
                    break;
                case TRANS_ROT90:
                    x_step = -src_stride;
                    y_step = 1;
                    x0 = y_offset;
                    y0 = h_src - x_offset - 1;
                    break;
                case TRANS_ROT270:
                    x_step = src_stride;
                    y_step = -1;
                    x0 = w_src - y_offset - 1;
                    y0 = x_offset;
                    break;
                case TRANS_MIRROR_ROT90:
                    x_step = -src_stride;
                    y_step = -1;
                    x0 = w_src - y_offset - 1;
                    y0 = h_src - x_offset - 1;
                    break;
                case TRANS_MIRROR_ROT270:
                    x_step = src_stride;
                    y_step = 1;
                    x0 = y_offset;
                    y0 = x_offset;
                    break;
                default:
                    return;
            }

            x0 += x_src;
            y0 += y_src;


            int w = dst_rect_clip.getWidth();
            int h = dst_rect_clip.getHeight();

            if (image->getFormat() == RGBA_8888) {
                auto fun = [](uint32_t *dst, uint32_t *src, int w, int x_step) {
                    while (w-- > 0) {
                        blend1px((uint8_t *) dst, (uint8_t *) src);
                        dst += 1;
                        src += x_step;
                    }
                };

                auto *src = src_pixels.addr<uint32_t *>(x0, y0);
                auto *dst = buffer->addr<uint32_t *>(dst_rect_clip.left, dst_rect_clip.top);
                while (h-- > 0) {
                    fun(dst, src, w, x_step);
                    dst += dst_stride;
                    src += y_step;
                }
            } else if (image->getFormat() == RGB_888X) {
                auto fun = [](uint32_t *dst, uint32_t *src, int w, int x_step) {
                    while (w-- > 0) {
                        *dst = *src;
                        dst += 1;
                        src += x_step;
                    }
                };

                auto *src = src_pixels.addr<uint32_t *>(x0, y0);
                auto *dst = buffer->addr<uint32_t *>(dst_rect_clip.left, dst_rect_clip.top);
                while (h-- > 0) {
                    fun(dst, src, w, x_step);
                    dst += dst_stride;
                    src += y_step;
                }
            }
        }
    }


    void Graphics::drawImage(Image *src, int x_dst, int y_dst, int dst_width, int dst_height, int anchor) {
        if (src == nullptr || dst_width <= 0 || dst_height <= 0) {
            return;
        }

        Rect bounds = clip.getDeviceClipBounds();
        if (bounds.isEmpty()) {
            return;
        }

        int src_width = src->getWidth();
        int src_height = src->getHeight();

        translatePoint(x_dst, y_dst, src_width, src_height, anchor);

        PixelBuffer &dst_buffer = *buffer;
        PixelBuffer &src_buffer = src->getPixelBufferRef();
        drawPixelsScale(dst_buffer.addr<uint8_t *>(), src_buffer.addr<uint8_t *>(), dst_buffer.getRowBytes(),
                        src_buffer.getRowBytes(),
                        Rect::makeXYWH(x_dst, y_dst, dst_width, dst_height), Rect::makeWH(src_width, src_height),
                        bounds, dst_buffer.getFormat(), src_buffer.getFormat());
    }


    inline void Graphics::drawLineV(int x, int y0, int y1) {
        if (clip.clipLineV(x, y0, y1)) {
            int count = y1 - y0;
            count += count < 0 ? -1 : 1;
            auto *begin = buffer->addr<uint8_t *>(x, y0);
            auto stride = buffer->getPaddedRowByte();
            blitLineV(begin, count, stride, paintColor.addr8());
        }
    }

    inline void Graphics::drawLineH(int x0, int x1, int y) {
        if (clip.clipLineH(x0, x1, y)) {
            int count = x1 - x0;
            count += count < 0 ? -1 : 1;
            auto *begin = buffer->addr<uint8_t *>(x0, y);
            blitLineH(begin, count, paintColor.addr8());
        }
    }

    void Graphics::drawLineAA(int x0, int y0, int x1, int y1) {
        int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx - dy, e2, x2; /* error value e_xy */
        int ed = dx + dy == 0 ? 1 : std::sqrt((float) dx * dx + (float) dy * dy);

        for (;;) {
            /* pixel loop */
            drawPixelAlpha(x0, y0, 255 - (255 * abs(err - dx + dy) / ed));
            e2 = err;
            x2 = x0;
            if (2 * e2 >= -dx) {
                /* x step */
                if (x0 == x1) break;
                if (e2 + dy < ed) drawPixelAlpha(x0, y0 + sy, (255 - 255 * (e2 + dy) / ed));
                err -= dy;
                x0 += sx;
            }
            if (2 * e2 <= dy) {
                /* y step */
                if (y0 == y1) break;
                if (dx - e2 < ed) drawPixelAlpha(x2 + sx, y0, (255 - 255 * (dx - e2) / ed));
                err += dx;
                y0 += sy;
            }
        }
    }


    void Graphics::drawLineB(int x0, int y0, int x1, int y1) {
        // d = k;
        // x1 = x0 + 1, d += k
        // y1 = y0, d <= 0.5;  y1 = y0 + 1, d > 0.5;

        // e = d - 0.5
        // e = k - 0.5
        // x1 = x0 + 1, e += k;
        // y1 = y0, d <= 0.5 || d + 0.5 <= 0.5+0.5 || e <= 1 ;   e <= 1;  y1 = y0 + 1; d > 0.5 || d + 0.5 > 0.5 + 0.5 || e > 1

        // if (clip.clipLineB(x0, y0, x1, y1)) {
        //     drawLineAA(x0, y0, x1, y1);
        //     int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        //     int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        //     int err = (dx > dy ? dx : -dy) / 2;
        //
        //     while (x0 != x1 || y0 != y1) {
        //         int e2 = err;
        //         if (e2 > -dx) {
        //             err -= dy;
        //             x0 += sx;
        //         }
        //         if (e2 < dy) {
        //             err += dx;
        //             y0 += sy;
        //         }
        //         drawPixel(x0, y0);
        //     }
        // }


        if (clip.clipLineB(x0, y0, x1, y1)) {
            drawLineAA(x0, y0, x1, y1);
        }
    }


    void Graphics::drawLine(int x0, int y0, int x1, int y1) {
        translatePoint(std::ref(x0), std::ref(y0));
        translatePoint(std::ref(x1), std::ref(y1));

        bool xe = x0 == x1;
        bool ye = y0 == y1;
        if (xe && ye) {
            drawPixel(x0, y0);
        } else if (xe) {
            drawLineV(x0, y0, y1);
        } else if (ye) {
            drawLineH(x0, x1, y0);
        } else {
            drawLineB(x0, y0, x1, y1);
        }
    }


    void Graphics::drawPoint(int x, int y) {
        drawPoint({x, y});
    }

    void Graphics::drawPoint(Point point) {
        Rect clip_ = clip.getDeviceClipBounds();
        if (clip_.contain(point)) {
            drawPixel(point.x, point.y);
        }
    }

    void Graphics::clear(int color) {
        this->clear(Color{color});
    }


    void Graphics::clear(const Color &color) {
        auto *pixel = buffer->addr<uint8_t *>();
        ptrdiff_t stride_ = buffer->getRowBytes();

        if (stride_ == width * 4) {
            blitH(pixel, width * height, (uint8_t *) &color);
        } else {
            blitRect(pixel, width, height, stride_, (uint8_t *) &color);
        }
    }


    void Graphics::drawRect(int x, int y, int w, int h) {

        if (w < 0 || h < 0)
            return;
        if (w == 0 && h == 0) {
            drawPoint(x, y);
            return;
        }

        translatePoint(x, y);
        int x1 = x + w;
        int y1 = y + h;

        drawLineH(x, x1, y);
        drawLineV(x, y, y1);

        if (w != 0) {
            drawLineV(x1, y, y1);
        }

        if (h != 0) {
            drawLineH(x, x1, y1);
        }
    }

    void Graphics::drawRect(const Rect &rect) {
        drawRect(rect.left, rect.top, rect.getWidth(), rect.getHeight());
    }

    void Graphics::fillRect(int x, int y, int w, int h) {
        fillRect({x, y, x + w, y + h});
    }


    void Graphics::fillRect(const Rect &rect) {

        if (rect.isEmpty())
            return;

        Rect clip_ = clip.getDeviceClipBounds();
        if (clip_.isEmpty())
            return;

        Rect dst = rect.offset(translation.x, translation.y);
        if (dst.isEmpty()) {
            return;
        }

        Rect bound = clip_.intersect(dst);
        if (bound.isEmpty()) {
            return;
        }

        int x = bound.left;
        int y = bound.top;
        int w = bound.getWidth();
        int h = bound.getHeight();

        auto *pixel = buffer->addr<uint8_t *>(x, y);
        ptrdiff_t stride_ = buffer->getRowBytes();

        blitRect(pixel, w, h, stride_, (uint8_t *) &paintColor);
    }

    void Graphics::drawCircle(int centerX, int centerY, int r) {
        int xm = centerX;
        int ym = centerY;
        int x = -r, y = 0; /* II. quadrant from bottom left to top right */
        int i, x2, e2, err = 2 - 2 * r; /* error of 1.step */
        r = 1 - err;
        do {
            i = 255 * abs(err - 2 * (x + y) - 2) / r; /* get blend value of pixel */
            drawPixelAlpha(xm - x, ym + y, 255 - i); /*   I. Quadrant */
            drawPixelAlpha(xm - y, ym - x, 255 - i); /*  II. Quadrant */
            drawPixelAlpha(xm + x, ym - y, 255 - i); /* III. Quadrant */
            drawPixelAlpha(xm + y, ym + x, 255 - i); /*  IV. Quadrant */
            e2 = err;
            x2 = x; /* remember values */
            if (err + y > 0) {
                /* x step */
                i = 255 * (err - 2 * x - 1) / r; /* outward pixel */
                if (i < 256) {
                    drawPixelAlpha(xm - x, ym + y + 1, 255 - i);
                    drawPixelAlpha(xm - y - 1, ym - x, 255 - i);
                    drawPixelAlpha(xm + x, ym - y - 1, 255 - i);
                    drawPixelAlpha(xm + y + 1, ym + x, 255 - i);
                }
                err += ++x * 2 + 1;
            }
            if (e2 + x2 <= 0) {
                /* y step */
                i = 255 * (2 * y + 3 - e2) / r; /* inward pixel */
                if (i < 256) {
                    drawPixelAlpha(xm - x2 - 1, ym + y, 255 - i);
                    drawPixelAlpha(xm - y, ym - x2 - 1, 255 - i);
                    drawPixelAlpha(xm + x2 + 1, ym - y, 255 - i);
                    drawPixelAlpha(xm + y, ym + x2 + 1, 255 - i);
                }
                err += ++y * 2 + 1;
            }
        } while (x < 0);



//        auto fun = [this, centerX, centerY](int ox, int oy){
//            drawPoint(centerX + ox, centerY + oy);
//            drawPoint(centerX + ox, centerY - oy);
//            drawPoint(centerX - ox, centerY + oy);
//            drawPoint(centerX - ox, centerY - oy);
//
//            drawPoint(centerX + oy, centerY + ox);
//            drawPoint(centerX + oy, centerY - ox);
//            drawPoint(centerX - oy, centerY + ox);
//            drawPoint(centerX - oy, centerY - ox);
//        };
//        circleRun<false>(r, fun);
    }

    void Graphics::fillCircle(int centerX, int centerY, int r) {
        auto fun = [this, centerX, centerY](int ox, int oy){
            drawLineH(centerX - ox, centerX + ox, centerY + oy);
            drawLineH(centerX - ox, centerX + ox, centerY - oy);
        };
        circleRun<true>(r, fun);
    }

    void Graphics::copyArea(int x_src, int y_src, int width_, int height_, int x_dst, int y_dst, int anchor) {
        translatePoint(x_dst, y_dst, width_, height_, anchor);

        Rect bounds = clip.getDeviceClipBounds();
        if (bounds.isEmpty())
            return;

        Rect src_area = Rect::makeXYWH(x_src, y_src, width_, height_);
        if (src_area.isEmpty() || src_area.left < 0 || src_area.top < 0 || src_area.right > width || src_area.bottom >
                                                                                                     height)
            return;

        Rect dst_area = Rect::makeXYWH(x_dst, y_dst, width_, height_);
        dst_area = bounds.intersect(dst_area);
        if (dst_area.isEmpty()) {
            return;
        }

        int cx = dst_area.left - x_dst;
        int cy = dst_area.top - y_dst;
        int cw = dst_area.getWidth();
        int ch = dst_area.getHeight();
        src_area = Rect::makeXYWH(x_src + cx, y_src + cy, cw, ch);


        auto *pixel = buffer->addr<uint8_t *>();
        ptrdiff_t stride_ = buffer->getRowBytes();
        ::copyArea(pixel, stride_, dst_area, src_area);
    }

    void Graphics::drawPixelAlpha(int x, int y, int alpha) {
        auto *pixels = buffer->addr<uint8_t *>(x, y);
        blendA8(pixels, alpha, (uint8_t *) &paintColor);
    }


    void Graphics::drawChar(char c, int x, int y, int anchor, const Font &font) {
        auto *pixel = buffer->addr<uint8_t *>();
        ptrdiff_t stride = buffer->getRowBytes();
        int format = buffer->getFormat();
        translatePoint(x, y);
        x = anchorTextX(anchor, x, font.charWidth(c), font);
        y = anchorTextY(anchor, y, font);

        draw_char(pixel, stride, format, c, x, y, clip.getDeviceClipBounds(), font, paintColor.ToARGB());
    }

    void Graphics::drawString(const std::string &str, int x, int y, int anchor, const Font &font) {
        this->drawString(str.c_str(), str.length(), x, y, anchor, font);
    }


    void Graphics::drawString(const char *str, int x, int y, int anchor, const Font &font) {
        if (str != nullptr) {
            this->drawString(str, strlen(str), x, y, anchor, font);
        }
    }


    void Graphics::drawString(const char *str, size_t len, int x, int y, int anchor, const Font &font) {
        if (str == nullptr) {
            return;
        }

        Rect bounds = clip.getDeviceClipBounds();
        if (bounds.isEmpty()) {
            return;
        }

        int x_offset = x;
        int y_offset = y;

        translatePoint(x_offset, y_offset);
        x_offset = anchorTextX(anchor, x_offset, font.charsWidth(str, (int) len), font);
        y_offset = anchorTextY(anchor, y_offset, font);


        draw_string(buffer->addr<uint8_t *>(), buffer->getRowBytes(), buffer->getFormat(), str, len, x_offset, y_offset,
                    bounds, font,
                    paintColor.ToARGB());
    }

    void Graphics::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
        drawLine(x0, y0, x1, y1);
        drawLine(x0, y0, x2, y2);
        drawLine(x1, y1, x2, y2);
    }


    void Graphics::fillTriangleA(int x0, int y0, int x1, int x2, int y_base) {
        int dx1 = x1 - x0;
        int dx2 = x2 - x0;
        int dy = abs(y_base - y0);

        int sy = y_base > y0 ? 1 : -1;
        float k1 = (float) dy / (float) dx1;
        float k2 = (float) dy / (float) dx2;
        float sx1 = -1 / k1;
        float sx2 = -1 / k2;
        auto f1 = (float) x1;
        auto f2 = (float) x2;

        while (y_base != y0) {
            f1 += sx1;
            f2 += sx2;

            y0 += sy;
            drawLineH((int) roundf(f1), (int) roundf(f2), y0);
        }
    }


    void Graphics::fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
        translatePoint(x0, y0);
        translatePoint(x1, y1);
        translatePoint(x2, y2);

        if (y1 < y0) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        if (y2 < y0) {
            std::swap(x0, x2);
            std::swap(y0, y2);
        }
        if (y2 < y1) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        if (x0 == x1 && x1 == x2) {
            return;
        }

        if (y0 == y2) {
            return;
        } else if (y0 == y1) {
            fillTriangleA(x2, y2, x0, x1, y0);
        } else if (y1 == y2) {
            fillTriangleA(x0, y0, x1, x2, y1);
        } else {
            int mid_x = static_cast<int>(x0 + (static_cast<double>(y1 - y0) / static_cast<double>(y2 - y0)) * (
                    x2 - x0));
            int mid_y = y1;

            fillTriangleA(x0, y0, x1, mid_x, mid_y);
            fillTriangleA(x2, y2, x1, mid_x, mid_y);
        }
    }


    void Graphics::translatePoint(int &x, int &y) const {
        x += translation.x;
        y += translation.y;
    }

    void Graphics::translatePoint(int &x, int &y, int width_, int height_, int anchor) {
        translatePoint(x, y);
        x = anchorX(anchor, x, width_);
        y = anchorY(anchor, y, height_);
    }

    int Graphics::getColor() {
        return paintColor.ToARGB();
    }


    void Graphics::drawArc(int x_, int y_, int w, int h, int startAngle, int arcAngle) {


//        translatePoint(x_, y_);

//        float radiusX = (float) w / 2.f;
//        float radiusY = (float) h / 2.f;
//        float centerX = (float) x_ + radiusX;
//        float centerY = (float) y_ + radiusY;
//        auto cx = static_cast<int>(std::roundf(centerX));
//        auto cy = static_cast<int>(std::roundf(centerY));
//        auto rx = static_cast<int>(std::roundf(radiusX));
//        auto ry = static_cast<int>(std::roundf(radiusY));

//        auto fun = [this](int cx, int cy, int x, int y) {
//            this->drawPoint(cx + x, cy + y);
//            this->drawPoint(cx - x, cy + y);
//            this->drawPoint(cx + x, cy - y);
//            this->drawPoint(cx - x, cy - y);
//        };
//        arcRun<false>(cx, cy, rx, ry, fun);
    }

    void Graphics::fillArc(int x_, int y_, int w, int h, int startAngle, int arcAngle) {
//        translatePoint(x_, y_);

//        float radiusX = (float) w / 2.f;
//        float radiusY = (float) h / 2.f;
//        float centerX = (float) x_ + radiusX;
//        float centerY = (float) y_ + radiusY;
//        auto cx = static_cast<int>(std::roundf(centerX));
//        auto cy = static_cast<int>(std::roundf(centerY));
//        auto rx = static_cast<int>(std::roundf(radiusX));
//        auto ry = static_cast<int>(std::roundf(radiusY));

        auto fun = [this](int cx, int cy, int x, int y) {
            this->drawLineH(cx - x, cx + x, cy - y);
            this->drawLineH(cx - x, cx + x, cy + y);
        };
//        arcRun<false>(cx, cy, rx, ry, fun);
    }

    int Graphics::save() {
        states.emplace(
                translation,
                clip.getClipBounds(),
                paintColor
        );
        return states.size() - 1;
    }

    bool Graphics::restore() {
        return this->restoreToCount(states.size() - 1);
    }


    bool Graphics::restoreToCount(int count) {
        if (count < 0 || count >= states.size()) {
            return false;
        }
        while (states.size() > (count + 1)) {
            states.pop();
        }
        auto &state = states.top();
        this->translation = state.translate;
        this->clip.set(state.clip);
        this->paintColor = state.paintColor;
        states.pop();
        return true;
    }


    void Graphics::drawRGB(int *rgbData, int dataLength, int offset, int scanLength,
                           int x, int y, int width_, int height_,
                           bool processAlpha) {

        Rect clip_ = clip.getDeviceClipBounds();
        if (clip_.isEmpty() || width <= 0 || height <= 0) {
            return;
        }
        translatePoint(x, y);
        Rect dst_rect_clip = clip_.intersect(Rect::makeXYWH(x, y, width_, height_));
        if (dst_rect_clip.isEmpty()) {
            return;
        }

        int src_format = 0;


        if constexpr (std::endian::native == std::endian::little) {
            src_format = processAlpha ? BGRA_8888 : BGRX_8888;
        } else if constexpr (std::endian::native == std::endian::big) {
            src_format = processAlpha ? ARGB_8888 : XRGB_8888;
        }


        int bpp = PixelFormat::getBytePerPixel(src_format);
        rgbData += offset;

        int x0 = dst_rect_clip.left - x;
        int y0 = dst_rect_clip.top - y;

        auto *dst_pixels = buffer->addr<uint8_t *>(dst_rect_clip.left, dst_rect_clip.top);
        auto *src_pixels = reinterpret_cast<uint8_t *>(rgbData) + (y0 * scanLength) + x0 * bpp;
        drawPixels(dst_pixels, src_pixels, buffer->getRowBytes(), scanLength * bpp, width_ - x0, height_ - y0,
                   RGBA_8888, src_format);
    }


    void Graphics::drawRoundRect(int x, int y, int w, int h, int arcWidth, int arcHeight) {
        arcWidth = std::clamp(arcWidth, 0, w);
        arcHeight = std::clamp(arcHeight, 0, h);


        if (w <= 1 || h <= 1) {
            if (w < 0 || h < 0) {
                return;
            } else if (w == 0 && h == 0) {
                drawPoint(x, y);
            } else {
                drawLine(x, y, x + width, y + height);
            }
        } else {
            if (arcWidth > 0 || arcHeight > 0) {
                int rx = (arcWidth / 2);
                int ry = (arcHeight / 2);
                int tx1 = x + rx;
                int tx2 = x + w - rx;
                int ty1 = y + ry;
                int ty2 = y + h - ry;

                auto fun = [this, tx1, tx2, ty1, ty2](int ox, int oy) {
                    this->drawPoint(tx1 - ox, ty1 - oy);
                    this->drawPoint(tx2 + ox, ty1 - oy);
                    this->drawPoint(tx1 - ox, ty2 + oy);
                    this->drawPoint(tx2 + ox, ty2 + oy);
                };
                arcRun<false>(rx, ry, fun);

                this->drawLineH(tx1, tx2, y);
                this->drawLineH(tx2, tx2, y + h);
                this->drawLineV(x, ty1, ty2);
                this->drawLineV(x + w, ty1, ty2);
            } else {
                drawRect(x, y, w, h);
            }
        }
    }

    void Graphics::fillRoundRect(int x, int y, int w, int h, int arcWidth, int arcHeight) {

        arcWidth = std::clamp(arcWidth, 0, w);
        arcHeight = std::clamp(arcHeight, 0, h);

        if (w <= 1 || h <= 1) {
            if (w == 1 && h == 0) {
                drawPoint(x, y);
            }
        } else {
            if (arcWidth > 0 || arcHeight > 0) {

                int rx = (arcWidth / 2);
                int ry = (arcHeight / 2);
                int tx1 = x + rx;
                int tx2 = x + w - rx;
                int ty1 = y + ry;
                int ty2 = y + h - ry;

//                auto fun = [this, tx1, tx2, ty1, ty2](int ox, int oy) {
//                    this->drawLineH(tx1 - ox, tx2 + ox, ty1 - oy);
//                    this->drawLineH(tx1 - ox, tx2 + ox, ty2 + oy);
//                };
//                arcRun<true>(rx, ry, fun);

                fillRect(x, ty1, w, ty2 - ty1);
            } else {
                fillRect(x, y, w, h);
            }
        }

    }

    void Graphics::drawEllipse(int x, int y, int radiusX, int radiusY) {
    }

    void Graphics::fillEllipse(int x, int y, int radiusX, int radiusY) {
    }
}
