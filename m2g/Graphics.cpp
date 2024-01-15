
#include <cstring>
#include <algorithm>
#include <cmath>
#include "draw.h"
#include "draw_image.h"
#include "draw_text.h"
#include "color.h"
#include "m2g.h"
#include "m2g-def.h"
#include "Graphics.h"


static inline int anchorX(int anchor, int x, int w) {
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


static inline int anchorY(int anchor, int y, int h) {
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

static inline int anchorTextX(int anchor, int x, int width, const Font& font) {
    switch (anchor & (LEFT | RIGHT | HCENTER)) {
        case RIGHT:
            return x - width;
        case HCENTER:
            return  x - std::round(width / 2.f);
        case LEFT:
        default:
            return x;
    }
}

static inline int anchorTextY(int anchor, int y, const Font& font) {
    const FontMetrics metrics = font.getFontMetrics();
    switch (anchor & (TOP | BOTTOM | BASELINE | VCENTER)) {
        case BOTTOM:
            return y + metrics.descent - metrics.lineGap;
        case BASELINE:
            return y;
        case VCENTER:
            return y + (metrics.ascent - std::round(metrics.height / 2));
        case TOP:
        default:
            return y + metrics.ascent;
    }
}



bool ValidImage(Image* image, int x, int y, int w, int h) {
    return image!= nullptr
           && x >= 0
           && y >= 0
           && w > 0
           && h > 0
           && x + w <= image->getWidth()
           && y + h <= image->getHeight();
}


Graphics::Graphics(std::shared_ptr<PixelBuffer> buffer)
        : buffer(buffer)
        , width(buffer->getWidth())
        , height(buffer->getHeight())
        , clip {width, height}
        , translation {0, 0}
        , paintColor {0xFF, 0x0, 0x0, 0xFF} {
}

Graphics::Graphics(std::shared_ptr<PixelBuffer> buffer, int width_, int height_)
        : Graphics(buffer) {
    setClip(0, 0, width_, height_);
}


Graphics::Graphics(const Graphics &other)
    : buffer(other.buffer)
    , width(other.width)
    , height(other.height)
    , clip(other.clip)
    , translation(other.translation)
    , paintColor (other.paintColor){
}


Graphics::Graphics(Graphics&& other) noexcept
    : buffer(std::move(other.buffer))
    , width(other.width)
    , height(other.height)
    , clip(other.clip)
    , translation(other.translation)
    , paintColor (other.paintColor) {
}


void Graphics::setColor(const Color& color) {
    paintColor = color;
}

void Graphics::setColor(int color) {
    paintColor = color;
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
    if(width_ <= 0 || height_ <= 0) {
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


    if(width_ <= 0 || height_ <= 0) {
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

    if(clip.contain(tmp)) {
        Rect rect = clip.intersect(tmp);
        if(rect.isEmpty()) {
            clip.setZero();
        } else {
            clip.set(rect);
        }
    } else {
        clip.setZero();
    }

}


int Graphics::getClipX() {
    return clip.getX() - translation.x;
}


int Graphics::getClipY() {
    return clip.getY() - translation.y;
}



int Graphics::getClipWidth() {
    return clip.getWidth();
}

int Graphics::getClipHeight() {
    return clip.getHeight();
}


inline void Graphics::drawPixel(int x, int y) {
    *buffer->addr<Color*>(x, y) = paintColor ;
}

void Graphics::setPixel(int x, int y, Color color) {
    *buffer->addr<Color*>(x, y) = color ;
}


void Graphics::drawImage(Image* image, int x, int y, int anchor) {
    drawImage(image, 0, 0, image->getWidth(), image->getHeight(), x, y, anchor);
}



void Graphics::drawImage(Image* image, int x_src, int y_src, int w_src, int h_src, int x_dst, int y_dst, int anchor) {
    if(!ValidImage(image, x_src, y_src, w_src, h_src)) {
        return;
    }
    translatePoint(x_dst, y_dst, w_src, h_src, anchor);
    PixelBuffer& dst_buffer = *buffer;
    PixelBuffer& src_buffer = image->getPixelBufferRef();

    drawPixels(dst_buffer.addr<uint8_t*>(), src_buffer.addr<uint8_t*>(), dst_buffer.getRowBytes(), src_buffer.getRowBytes(),
               x_dst, y_dst,
               Rect::makeXYWH(x_src, y_src, w_src, h_src), clip.getDeviceClipBounds(), dst_buffer.getFormat(), src_buffer.getFormat());
}


void drawRegion() {
    // uint8_t* src_offset = src_pixels.addr<uint8_t*>(offsetX, offsetY);
    // uint8_t* dst_offset = buffer->addr<uint8_t*>(dstRectClip.left, dstRectClip.top);

    // ptrdiff_t src_stride = src_pixels.getRowBytes();
    // ptrdiff_t dst_stride = buffer->getRowBytes();

    // switch (transform) {
    //     case TRANS_MIRROR:
    //         copyRectXF(dst_offset, src_offset, dst_stride, src_stride, width_, height_);
    //         break;
    //     case TRANS_ROT180:
    //         copyRectXYF(dst_offset, src_offset, dst_stride, src_stride, width_, height_);
    //         break;
    //     case TRANS_MIRROR_ROT180:
    //         src_offset = src_pixels.addr<uint8_t*>(x_src + offsetX, y_src);
    //         copyRectYF(dst_offset, src_offset, dst_stride, src_stride, width_, height_ - offsetY);
    //         break;
    //
    // }
}

void Graphics::drawRegion(Image* image, int x_src, int y_src, int w_src, int h_src, int transform, int x_dst, int y_dst, int anchor) {
    if(transform == 0) {
        drawImage(image, x_src, y_src, w_src, h_src, x_dst, y_dst);
    } else {
        int x0 = 0;
        int y0 = 0;


        int x_step = 0;
        int y_step = 0;

        int w_dst, h_dst;
        int w_dst_clip, h_dst_clip;


        if(!ValidImage(image, x_src, y_src, w_src, h_src)) {
            return;
        }

        const Rect bounds = clip.getDeviceClipBounds();
        if(bounds.isEmpty() ) {
            return;
        }

        auto& src_pixels = image->getPixelBufferRef();
        int src_stride = src_pixels.getStride();
        int dst_stride = buffer->getStride();

        if(transform & TRANSFORM_INVERTED_AXES) {
            w_dst  = h_src;
            h_dst = w_src;
        } else {
            w_dst  = w_src;
            h_dst = h_src;
        }


        translatePoint(x_dst, y_dst);
        const Rect src_rect = Rect::makeXYWH(x_src, y_src, w_src, h_src);
        const Rect dst_rect = Rect::makeXYWH(x_dst, y_dst, w_dst, h_dst);
        const Rect dst_rect_clip = bounds.intersect(dst_rect);

        if(dst_rect_clip.isEmpty()) {
            return;
        }

        int x_offset = dst_rect_clip.left - x_dst;
        int y_offset = dst_rect_clip.top - y_dst;

        if(transform & TRANSFORM_X_FLIP) {
            x0 = w_src - 1;
        } else {
            x0 = 0;
        }


        if(transform & TRANSFORM_Y_FLIP) {
            y0 = h_src - 1;
        } else {
            y0 = 0;
        }

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
                y0 = h_src - x_offset -  1;
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

        if(image->getFormat() == RGBA_8888) {
            auto fun = [] (uint32_t* dst, uint32_t* src, int w, int x_step) {
                while(w -- >0) {
                    blend1px((uint8_t*)dst, (uint8_t*)src);
                    dst += 1;
                    src += x_step;
                }
            };

            uint32_t* src = src_pixels.addr<uint32_t*>(x0, y0);
            uint32_t* dst = buffer->addr<uint32_t*>(dst_rect_clip.left, dst_rect_clip.top);
            while (h -- > 0) {
                fun(dst, src, w, x_step);
                dst += dst_stride;
                src += y_step;
            }

        } else if (image->getFormat() == RGB_888X) {
            auto fun = [] (uint32_t* dst, uint32_t* src, int w, int x_step) {
                while(w -- >0) {
                    *dst = *src;
                    dst += 1;
                    src += x_step;
                }
            };

            uint32_t* src = src_pixels.addr<uint32_t*>(x0, y0);
            uint32_t* dst = buffer->addr<uint32_t*>(dst_rect_clip.left, dst_rect_clip.top);
            while (h -- > 0) {
                fun(dst, src, w, x_step);
                dst += dst_stride;
                src += y_step;
            }
        }
    }
}


// 内存越界

void Graphics::drawImage(Image* src, int x_dst,int y_dst, int dst_width, int dst_height, int anchor) {
    if(src == nullptr || dst_width <= 0 || dst_height <= 0) {
        return;
    }

    Rect bounds = clip.getDeviceClipBounds();
    if(bounds.isEmpty() ) {
        return;
    }

    int src_width = src->getWidth();
    int src_height = src->getHeight();

    translatePoint(x_dst, y_dst, src_width, src_height, anchor);

    PixelBuffer& dst_buffer = *buffer;
    PixelBuffer& src_buffer = src->getPixelBufferRef();
    drawPixelsScale(dst_buffer.addr<uint8_t*>(), src_buffer.addr<uint8_t*>(), dst_buffer.getRowBytes(), src_buffer.getRowBytes(),
    Rect::makeXYWH(x_dst, y_dst, dst_width, dst_height), Rect::makeWH(src_width, src_height), bounds, dst_buffer.getFormat(), src_buffer.getFormat());

}




inline void Graphics::drawLineV(int x, int y0, int y1) {
    if(clip.clipLineV(x, y0, y1)) {
        if(y0 == y1) {
            drawPixel(x, y0);
            return;
        } else if(y0 > y1){
            std::swap(y0, y1);
        }
        do {
            drawPixel(x, y0);
            y0 ++;
        } while (y0 != y1);
    }
}

inline void Graphics::drawLineH(int x0, int x1, int y) {
    if(clip.clipLineH(x0, x1, y)) {
        if(x0 == x1) {
            drawPixel(x0, x1);
            return;
        } else if(x0 > x1){
            std::swap(x0, x1);
        }
        auto* start = buffer->addr<uint8_t*>(x0, y);
        blitH(start, x1 - x0 + 1, (uint8_t*)&paintColor);
    }
}

void Graphics::drawLineAA(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = dx-dy, e2, x2;                       /* error value e_xy */
    int ed = dx+dy == 0 ? 1 : std::sqrt((float)dx*dx+(float)dy*dy);

    for ( ; ; ){                                         /* pixel loop */
        drawPixelAlpha(x0,y0, 255 - (255*abs(err-dx+dy)/ed));
        e2 = err; x2 = x0;
        if (2*e2 >= -dx) {                                    /* x step */
            if (x0 == x1) break;
            if (e2+dy < ed) drawPixelAlpha(x0,y0+sy, (255 - 255*(e2+dy)/ed));
            err -= dy; x0 += sx;
        }
        if (2*e2 <= dy) {                                     /* y step */
            if (y0 == y1) break;
            if (dx-e2 < ed) drawPixelAlpha(x2+sx,y0, (255 - 255*(dx-e2)/ed));
            err += dx; y0 += sy;
        }
    }
}



void Graphics::drawLineB(int x0, int y0, int x1, int y1) {
    if(clip.clipLineB(x0, y0, x1, y1)) {
        drawLineAA(x0, y0, x1, y1);
    }
}



void Graphics::drawRect(int x, int y, int w, int h) {

    // TODO FIXME

    if(clip.isEmpty() || w < 0 || h < 0)
        return;
    translatePoint(x, y);



    int x1 = x + w;
    int y1 = y + h;

    if(w == 0 && h == 0) {
        drawPoint(x, y);
        return;
    }

    drawLineH(x, x1, y);
    drawLineV(x, y, y1);

    if(w != 0) {
        drawLineV(x1, y, y1);
    }

    if(h != 0) {
        drawLineH(x, x1, y1);
    }


}


void Graphics::drawLine(int x0, int y0, int x1, int y1) {

    translatePoint(std::ref(x0), std::ref(y0));
    translatePoint(std::ref(x1), std::ref(y1));

    bool xe = x0 == x1;
    bool ye = y0 == y1;
    if(xe && ye) {
        drawPoint(x0, y0);
    } else if(xe){
        drawLineV(x0, y0, y1);
    } else if(ye) {
        drawLineH(x0, x1, y0);
    } else {
        drawLineB(x0, y0, x1, y1);
    }
}

void Graphics::drawPoint(int x, int y) {
    drawPoint({x, y});
}

void Graphics::drawPoint(Point point) {
    if(clip.contain(point)) {
        drawPixel(point.x, point.y);
    }
}



void Graphics::clear(Color color) {
    auto* pixel = buffer->addr<uint8_t*>();
    ptrdiff_t stride_ = buffer->getRowBytes();

    if(stride_ == width * 4) {
        blitH(pixel, width * height , (uint8_t*)&color);
    } else {
        blitRect(pixel, width, height, stride_, (uint8_t*)&color);
    }
}

void Graphics::drawRect(const Rect &rect) {
    drawRect(rect.left, rect.top, rect.getWidth(), rect.getHeight());
}

void Graphics::fillRect(int x, int y, int w, int h) {
    if(clip.isEmpty() || w <= 0 || h <= 0)
        return;

    translatePoint(x, y);

    if(w == 1) {
        drawLineV(x, y, y + h - 1);
        return;
    } else if(h == 1) {
        drawLineH(x, x + w - 1, y);
        return;
    }
    fillRect({x, y, x + w, y + h});
}


void Graphics::fillRect(const Rect &rect) {

    if(rect.isEmpty())
        return;

    Rect clipRect = clip.getDeviceClipBounds();
    if(clipRect.isEmpty())
        return;

    Rect bound =  clipRect.intersect(rect);
    if(bound.isEmpty()) {
        return;
    }

    int x = bound.left;
    int y = bound.top;
    int w = bound.getWidth();
    int h = bound.getHeight();

    auto* pixel = buffer->addr<uint8_t *>(x, y);
    ptrdiff_t stride_ = buffer->getRowBytes();

    blitRect(pixel, w, h, stride_, (uint8_t*)&paintColor);

}

void Graphics::drawCircle(int centerX, int centerY, int r) {

    int xm = centerX;
    int ym = centerY;
    int x = -r, y = 0;           /* II. quadrant from bottom left to top right */
    int i, x2, e2, err = 2-2*r;                             /* error of 1.step */
    r = 1-err;
    do {
        i = 255*abs(err-2*(x+y)-2)/r;               /* get blend value of pixel */
        drawPixelAlpha(xm-x, ym+y, 255-i);                             /*   I. Quadrant */
        drawPixelAlpha(xm-y, ym-x, 255-i);                             /*  II. Quadrant */
        drawPixelAlpha(xm+x, ym-y, 255-i);                             /* III. Quadrant */
        drawPixelAlpha(xm+y, ym+x, 255-i);                             /*  IV. Quadrant */
        e2 = err; x2 = x;                                    /* remember values */
        if (err+y > 0) {                                              /* x step */
            i = 255*(err-2*x-1)/r;                              /* outward pixel */
            if (i < 256) {
                drawPixelAlpha(xm-x, ym+y+1, 255-i);
                drawPixelAlpha(xm-y-1, ym-x, 255-i);
                drawPixelAlpha(xm+x, ym-y-1, 255-i);
                drawPixelAlpha(xm+y+1, ym+x, 255-i);
            }
            err += ++x*2+1;
        }
        if (e2+x2 <= 0) {                                             /* y step */
            i = 255*(2*y+3-e2)/r;                                /* inward pixel */
            if (i < 256) {
                drawPixelAlpha(xm-x2-1, ym+y, 255-i);
                drawPixelAlpha(xm-y, ym-x2-1, 255-i);
                drawPixelAlpha(xm+x2+1, ym-y, 255-i);
                drawPixelAlpha(xm+y, ym+x2+1, 255-i);
            }
            err += ++y*2+1;
        }
    } while (x < 0);


//    int y = r;
//    int p = 1 - r;
//    double lastFadeAmount = 0;
//    double fadeAmount = 0;
//    const int32_t maxOpaque = paintColor.a;
////    const int32_t noAlphaColor = color & 0x00FFFFFF;
//    for (int x = 0; x < y; x++) {
//        if (p < 0) {
//            p += 2 * x + 1;
//        } else {
//            y -= 1;
//            p += 2 * (x - y) + 1;
//        }
//
//        drawPixel(centerX + x, centerY + y);
//        drawPixel(centerX + y, centerY + x);
//        drawPixel(centerX + y, centerY - x);
//        drawPixel(centerX + x, centerY - y);
//
//        drawPixel(centerX - x, centerY + y);
//        drawPixel(centerX - y, centerY + x);
//        drawPixel(centerX - x, centerY - y);
//        drawPixel(centerX - y, centerY - x);
////        double height_ = sqrt(r * r - x * x);
////        fadeAmount = (double)maxOpaque * (1.0 - (ceil(height_) - height_));
//////        if (fadeAmount > lastFadeAmount)
//////            y--;
////        lastFadeAmount = fadeAmount;
////        paintColor.a = 0x54;
//
//
//    }

}

void Graphics::fillCircle(int centerX, int centerY, int r) {
    int y = r;
    int p = 1 - r;
    for (int x = 0; x < y; x++) {
        if (p < 0) {
            // 中点在园内
            p += 2 * x + 1;
        } else {
            // 中点在圆外或圆上
            y -= 1;
            p += 2 * (x - y) + 1;
            drawLineH(centerX - x, centerX + x, centerY + y);
            drawLineH(centerX - x, centerX + x, centerY - y);
        }
        drawLineH(centerX - y, centerX + y, centerY + x);
        drawLineH(centerX - y, centerX + y, centerY - x);
    }
}

void Graphics::copyArea(int x_src, int y_src, int width_, int height_, int x_dst, int y_dst, int anchor) {


    translatePoint(x_dst, y_dst, width_, height_, anchor);

    Rect bounds = clip.getDeviceClipBounds();
    if(bounds.isEmpty())
        return;

    Rect src_area = Rect::makeXYWH(x_src, y_src, width_, height_);
    if(src_area.isEmpty() || src_area.left < 0 || src_area.top < 0 || src_area.right > width || src_area.bottom > height)
        return;

    Rect dst_area = Rect::makeXYWH(x_dst, y_dst, width_, height_);
    dst_area = bounds.intersect(dst_area);
    if(dst_area.isEmpty()) {
        return;
    }

    int cx = dst_area.left - x_dst;
    int cy = dst_area.top - y_dst;
    int cw = dst_area.getWidth();
    int ch = dst_area.getHeight();

    src_area = Rect::makeXYWH(x_src + cx, y_src + cy, cw, ch);


    auto* pixel = buffer->addr<uint8_t *>();
    ptrdiff_t stride_ = buffer->getRowBytes();

    ::copyArea(pixel, stride_, buffer->getFormat(), dst_area, src_area);

}

void Graphics::drawPixelAlpha(int x, int y, int alpha) {
    uint8_t* pixels = buffer->addr<uint8_t*>(x, y);
    blendA8(pixels, alpha, (uint8_t*)&paintColor);
}


void Graphics::drawChar(char c, int x, int y, int anchor, const Font& font) {
    auto* pixel = buffer->addr<uint8_t*>();
    ptrdiff_t stride = buffer->getRowBytes();
    int format = buffer->getFormat();
    translatePoint(x, y);
    x = anchorTextX(anchor, x, font.charWidth(c), font);
    y = anchorTextY(anchor, y, font);
    draw_char(pixel, stride, format, c, x, y, font, paintColor.ToARGB());
}

void Graphics::drawString(const std::string& str, int x, int y, int anchor, const Font &font) {
    Rect bounds = clip.getDeviceClipBounds();
    if(bounds.isEmpty()) {
        return;
    }

    int len = str.length();
    int x_offset = x;
    int y_offset = y;

    translatePoint(x_offset, y_offset);
    x_offset = anchorTextX(anchor, x_offset, font.charsWidth(str.c_str(), len), font);
    y_offset = anchorTextY(anchor, y_offset, font);


    draw_string(buffer->addr<uint8_t*>(), buffer->getRowBytes(), buffer->getFormat(), str.c_str(), len, x_offset, y_offset, bounds, font,
                paintColor.ToARGB());

}


void Graphics::drawString(const char *str, int x, int y, int anchor, const Font& font) {

    Rect bounds = clip.getDeviceClipBounds();
    if(bounds.isEmpty()) {
        return;
    }

    int len = strlen(str);
    int x_offset = x;
    int y_offset = y;

    translatePoint(x_offset, y_offset);
    x_offset = anchorTextX(anchor, x_offset, font.charsWidth(str, len), font);
    y_offset = anchorTextY(anchor, y_offset, font);


    draw_string(buffer->addr<uint8_t*>(), buffer->getRowBytes(), buffer->getFormat(), str,len, x_offset, y_offset, bounds, font,
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


    float k1 = dy / (float)dx1;
    float k2 = dy / (float)dx2;
    int sy = 1;
    float sx1 = 1 / k1;
    float sx2 = 1 / k2;
    float f1 = x0;
    float f2 = x0;

    while (y0  != y_base) {
        f1 += sx1;
        f2 += sx2;

        y0 += sy;
        drawLineH(round(f1), round(f2), y0);
    }
}



void Graphics::fillTriangleV(int x0, int y0, int x1, int x2, int y_base) {
    int dx1 = x1 - x0;
    int dx2 = x2 - x0;
    int dy = abs(y_base - y0);


    float k1 = dy / (float)dx1;
    float k2 = dy / (float)dx2;
    int sy = 1;
    float sx1 = -1 / k1;
    float sx2 = -1 / k2;
    float f1 = x1;
    float f2 = x2;

    while (y_base != y0) {
        f1 += sx1;
        f2 += sx2;

        y_base += sy;
        drawLineH(roundf(f1), roundf(f2), y_base);
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

    if(x0 == x1 && x1 == x2) {
        return;
    }

    if (y0 == y2) {
        return;
    } else if(y0 == y1) {
        fillTriangleV(x2, y2, x0, x1, y0);
    } else if(y1 == y2) {
        fillTriangleA(x0, y0, x1, x2, y1);
    } else {
        int mid_x = static_cast<int>(x0 + (static_cast<double>(y1 - y0) / static_cast<double>(y2 - y0)) * (x2 - x0));
        int mid_y = y1;

        fillTriangleA(x0, y0, x1, mid_x, mid_y);
        fillTriangleV(x2, y2, x1, mid_x, mid_y);
    }


}




void Graphics::translatePoint(int& x, int& y) {
    x += translation.x;
    y += translation.y;
}

void Graphics::translatePoint(int &x, int &y,  int width_, int height_, int anchor) {
    translatePoint(x, y);
    x = anchorX(anchor, x, width_);
    y = anchorY(anchor, y, height_);
}

int Graphics::getColor() {
    return paintColor.ToARGB();
}

static void ellipsePlotPoints(Graphics* graphics, double centerX, double centerY, int x, int y) {
    graphics->drawPixel(centerX + x, centerY + y);
    // 对称点
    graphics->drawPixel(centerX - x, centerY + y);

    graphics->drawPixel(centerX + x, centerY - y);
    graphics->drawPixel(centerX - x, centerY - y);


//    graphics->drawLine(centerX + x, centerY + y, centerX - x, centerY + y);
//    graphics->drawLine(centerX + x, centerY - y, centerX - x, centerY - y);
}


void Graphics::drawArc(int x_, int y_, int w, int h, int startAngle, int arcAngle) {
    translatePoint(x_, y_);

    int radiusX = w / 2;
    int radiusY = h / 2;
    double centerX = x_ + radiusX;
    double centerY = y_ + radiusY;

    int rx2 = radiusX * radiusX;
    int ry2 = radiusY * radiusY;
    int p;

    int x = 0, y = radiusY; // (区域1)初始点
    int px = 0, py = 2 * rx2 * y; // 像素计算终止条件项

    // 绘制椭圆起始点
    ellipsePlotPoints(this,centerX, centerY, x, y);

    // 区域1
    p = round(ry2 - rx2 * radiusY + 0.25 * rx2);
    while (px < py) {
        x++;
        px += 2 * ry2; // 通过累加来计算 2ry^2 x[k+1], 而不是每次都用乘法
        if (p < 0) {
            p += ry2 + px;

        }
        else {
            y--;
            py -= 2 * rx2;
            p += ry2 + px - py;
            ellipsePlotPoints(this, centerX, centerY, x, y);
        }

    }

    // 区域2
    p = round(ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 *
                                                                      ry2);
    while (y > 0) {
        y--;
        py -= 2 * rx2;
        if (p > 0) {
            p += rx2 - py;
        }
        else {
            x++;
            px += 2 * ry2;
            p += rx2 - py + px;
        }
        ellipsePlotPoints(this, centerX, centerY, x, y);
    }






//    int radiusX = w / 2;
//    int radiusY = h / 2;
//    double centerX = x + radiusX;
//    double centerY = y + radiusY;
//
//    for (int angle = startAngle; angle < startAngle + arcAngle; ++angle) {
//        double radians = angle * M_PI / 180.0;
//        int xPos = static_cast<int>(centerX + radiusX * cos(radians));
//        int yPos = static_cast<int>(centerY + radiusY * sin(radians));
//        drawPixel(xPos, yPos);
//    }
}

void Graphics::fillArc(int x, int y, int w, int h, int startAngle, int arcAngle) {
    int radiusX = w / 2;
    int radiusY = h / 2;
    double centerX = x + radiusX;
    double centerY = y + radiusY;

    for (int angle = startAngle; angle < startAngle + arcAngle; ++angle) {
        double radians = angle * M_PI / 180.0;
        int xPos = static_cast<int>(std::round(centerX + radiusX * cos(radians)));
        int yPos = static_cast<int>(std::round(centerY + radiusY * sin(radians)));
        drawLine(static_cast<int>(centerX), static_cast<int>(centerY), xPos, yPos);
    }
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
    while (states.size() > count + 1) {
        states.pop();
    }
    auto& state = states.top();
    this->translation = state.translate;
    this->clip.set(state.clip);
    this->paintColor = state.paintColor;
    states.pop();
    return true;
}

static bool isBigEndian() {
    static union  {
        uint32_t value = 0x01020304;
        uint8_t bytes[4];
    } endian;

    if(endian.bytes[0] == 0x01) {
        return true;
    } else if(endian.bytes[0] == 0x04) {
        return false;
    } else {
        assert(false);
    }
}

void Graphics::drawRGB(int *rgbData, int dataLength, int offset, int scanLength,
                       int x, int y, int width_, int height_,
                       bool processAlpha) {

    if(clip.isEmpty() || width <= 0 || height <= 0) {
        return;
    }
    translatePoint(x, y);
    Rect dst_rect_clip = clip.intersect(Rect::makeXYWH(x, y, width_, height_));
    if(dst_rect_clip.isEmpty()) {
        return;
    }

    int src_format = 0;


    if(isBigEndian()) {
        src_format = processAlpha ? ARGB_8888 : XRGB_8888;
    } else {
        src_format = processAlpha ? BGRA_8888 : BGRX_8888;
    }

    int bpp = PixelFormat::getBytePerPixel(src_format);

    rgbData += offset;

    int x0 = dst_rect_clip.left   - x;
    int y0 = dst_rect_clip.top    - y;

    auto* dst_pixels = buffer->addr<uint8_t*>(dst_rect_clip.left, dst_rect_clip.top);
    auto* src_pixels = reinterpret_cast<uint8_t*>(rgbData) + (y0 * scanLength) + x0 * bpp;
    drawPixels(dst_pixels, src_pixels, buffer->getRowBytes(), scanLength * bpp, width_ - x0, height_ -y0, RGBA_8888, src_format);

    // if(processAlpha) {
    //     for (int j  = 0; j < height_; ++j) {
    //         for (int i= 0; i < width_; ++i) {
    //             ((Color*)dst_pixels)[i] = rgbData[i];
    //         }
    //         dst_pixels += buffer->getRowBytes();
    //         rgbData += scanLength;
    //     }
    // } else {
    //     for (int j  = 0; j < height_; ++j) {
    //         for (int i= 0; i < width_; ++i) {
    //             ((Color*)dst_pixels)[i] = rgbData[i] & 0xFF000000;
    //         }
    //         dst_pixels += buffer->getRowBytes();
    //         rgbData += scanLength;
    //     }
    // }
}

void Graphics::drawARGB(uint8_t* data, int dataLength, int offset, int scanLength, int x, int y, int width_, int height_) {

}

void Graphics::drawBGRA(uint8_t* data, int dataLength, int offset, int scanLength, int x, int y, int width_, int height_) {

}

void Graphics::drawRoundRect(int x, int y, int w, int h, int arcWidth, int arcHeight) {

}

void Graphics::fillRoundRect(int x, int y, int w, int h, int arcWidth, int arcHeight) {

}

void Graphics::drawEllipse(int x, int y, int radiusX, int radiusY) {

}

void Graphics::fillEllipse(int x, int y, int radiusX, int radiusY) {

}














