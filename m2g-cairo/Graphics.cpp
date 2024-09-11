//
// Created by Admin on 2024/2/20.
//

#include "Graphics.h"

#include <regex>
#include <cmath>
#include <iostream>
#include <cstring>

#include "cairo/cairo.h"
#include "Image.h"
#include "Font.h"


#define R(argb) static_cast<uint8_t>((argb >> 16) & 0xFF)
#define G(argb) static_cast<uint8_t>((argb >> 8) & 0xFF)
#define B(argb) static_cast<uint8_t>(argb & 0xFF)
#define A(argb) static_cast<uint8_t>((argb >> 24) & 0xFF)

namespace m2g {


    enum Anchor {
        HCENTER = 1,
        VCENTER = 2,
        LEFT = 4,
        RIGHT = 8,
        TOP = 16,
        BOTTOM = 32,
        BASELINE = 64,
    };


    enum Transform {
        TRANS_NONE          = 0,               //   0000
        TRANS_MIRROR_ROT180 = 1,               //   0001
        TRANS_MIRROR        = 2,               //   0010
        TRANS_ROT180        = 3,               //   0011

        TRANS_MIRROR_ROT270 = 4,               //   0100
        TRANS_ROT90         = 5,               //   0101
        TRANS_ROT270        = 6,               //   0110
        TRANS_MIRROR_ROT90  = 7,               //   0111

        TRANSFORM_MASK      =  0x7,
        TRANSFORM_INVERTED_AXES =  1 << 2,
        TRANSFORM_X_FLIP =         1 << 1,
        TRANSFORM_Y_FLIP =         1,
    };

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

    int anchorTextX(int anchor, int x, int width, const Font& font) {
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

    int anchorTextY(int anchor, int y, const Font& font) {
        const cairo_font_extents_t* extents = font.getCairoFontExtents();
        switch (anchor & (TOP | BOTTOM | BASELINE | VCENTER)) {
            case BOTTOM:
                y -= (int)(extents->height / 2.f);
            case VCENTER:
                y += (int)((extents->ascent + extents->descent) / 2.f);
            case BASELINE:
                return y;
            case TOP:
                default:
                    return y + (int)extents->ascent;
        }
    }

    Graphics::Graphics(Image* image)
        : color(-1) {
        this->surface_ = image->getCairoSurface();
        this->cr = cairo_create(surface_);
        reset();
    }

    Graphics::Graphics(cairo_surface_t* surface)
        : color(-1) {
        this->surface_ = surface;
        this->cr = cairo_create(surface);
        reset();
    }

    Graphics::Graphics(const Graphics& other)
        : cr(nullptr)
        , color(-1) {
        this->surface_ = other.surface_;
        this->cr = cairo_create(surface_);
        reset();
    }

    Graphics::Graphics(Graphics&& other) noexcept
        : cr(nullptr)
        , surface_(nullptr)
        , color(-1) {
        std::swap(surface_, other.surface_);
        std::swap(cr, other.cr);
    }

    Graphics::~Graphics() {
        if(cr != nullptr) {
            cairo_destroy(cr);
        }
    }


    void Graphics::setColor(int argb) {
        this->color = argb;
        double alpha = (double)A(argb) / 255;
        double red = (double)R(argb) / 255;
        double green = (double)G(argb) / 255;
        double blue = (double)B(argb) / 255;
        cairo_set_source_rgba(cr, red, green, blue, alpha);
    }

    int Graphics::getColor() const {
        return color;
    }

    void Graphics::reset() {
        cairo_set_line_width(cr, 1);
        // cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
        // cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
         cairo_set_antialias(cr, CAIRO_ANTIALIAS_GOOD);
        // cairo_save(cr);
    }

    void Graphics::drawLine(int x0, int y0, int x1, int y1) {
        if(x0 == x1 && y0 == y1) {
            this->drawPoint(x0, y0);
            return;
        }
        cairo_new_path(cr);
        cairo_move_to (cr, x0, y0);
        cairo_line_to (cr, x1, y1);
        cairo_stroke(cr);
    };

    void Graphics::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
        cairo_new_path(cr);
        cairo_move_to(cr, x0, y0);
        cairo_line_to(cr, x1, y1);
        cairo_line_to(cr, x2, y2);
        cairo_close_path(cr);
        cairo_stroke(cr);
    }

    void Graphics::drawRect(int x, int y, int w, int h) {
        cairo_rectangle (this->cr, x, y, w, h);
        cairo_stroke(cr);
    }

    void Graphics::drawRoundRect(int x, int y, int w, int h, int arcWidth, int arcHeight) {
         cairo_new_path(cr);
         cairo_move_to(cr, x + arcWidth, y);
         cairo_line_to(cr, x + w - arcWidth, y);
         cairo_curve_to(cr, x + w, y, x + w, y, x + w, y + arcHeight);
         cairo_line_to(cr, x + w, y + h - arcHeight);
         cairo_curve_to(cr, x + w, y + h, x + w, y + h, x + w - arcWidth, y + h);
         cairo_line_to(cr, x + arcWidth, y + h);
         cairo_curve_to(cr, x, y + h, x, y + h, x, y + h - arcHeight);
         cairo_line_to(cr, x, y + arcHeight);
         cairo_curve_to(cr, x, y, x, y, x + arcWidth, y);
         cairo_close_path(cr);
         cairo_stroke(cr);
    }

    void Graphics::drawArc(int x, int y, int w, int h, int startAngle, int arcAngle) {
        double xc = x + w / 2.0;
        double yc = y + h / 2.0;
        double xr = w / 2.0;
        double yr = h / 2.0;
        double start_radians = startAngle * M_PI / 180.0;
        double arc_radians = arcAngle * M_PI / 180.0;

        cairo_save(cr);
        cairo_translate(cr, xc, yc);
        cairo_scale(cr, 1.0, yr / xr);
        cairo_move_to(cr, xr * cos(start_radians), yr * sin(start_radians));
        cairo_scale(cr, xr, 1.0 / yr);
        cairo_arc(cr, 0, 0, 1.0, start_radians, start_radians + arc_radians);
        cairo_stroke(cr);
        cairo_restore(cr);
    }

    void Graphics::drawEllipse(int x, int y, int radiusX, int radiusY) {

    }

    void Graphics::drawCircle(int centerX, int centerY, int r) {
        cairo_arc(cr, centerX, centerY, r, 0, 2 * M_PI);
        cairo_stroke(cr);
    }


    void Graphics::drawChar(char c, int x, int y, int anchor, const m2g::Font& font) {
        this->drawString({c}, x, y, anchor, font);
    }

    void Graphics::drawString(const char* str, int x, int y, int anchor, const Font& font) {
        if(str != nullptr) {
            this->drawString(str, strlen(str), x, y, anchor, font);
        }
    }

    void Graphics::drawString(const char* str, size_t len, int x, int y, int anchor, const Font& font) {

        x = anchorTextX(anchor, x, font.charsWidth(str, len), font);
        y = anchorTextY(anchor, y, font);

        cairo_set_font_face(cr, font.getCairoFace());
        cairo_set_font_size(cr, font.getFontSize());

        cairo_move_to(cr, x, y);
        cairo_show_text(cr, str);
    }

    void Graphics::drawString(const std::string& str, int x, int y, int anchor, const Font& font) {
        this->drawString(str.c_str(), str.length(), x, y, anchor, font);
    }

    void Graphics::drawRGB(int* rgbData, int dataLength, int offset, int scanLength, int x, int y, int width, int height, bool processAlpha) {

        rgbData += offset;


        // TODO 优化裁剪
        if constexpr (std::endian::native == std::endian::little) {
            if(processAlpha) {
                cairo_surface_t* surface = cairo_image_surface_create(
                    CAIRO_FORMAT_ARGB32,
                    width,
                    height
                );

                uint8_t* data = cairo_image_surface_get_data(surface);
                int stride = cairo_image_surface_get_stride(surface);


                int p = width;
                while (p -- > 0) {


                }

                std::cout << "Not impl drawRGB std::endian::little" << std::endl;
//                cairo_set_source_surface(cr, surface, x, y);
                // cairo_paint(cr);
            } else {
                cairo_surface_t* surface = cairo_image_surface_create_for_data(
                    (unsigned char*)rgbData,
                    CAIRO_FORMAT_RGB24,
                    width,
                    height,
                    scanLength * 4
                );
                cairo_set_source_surface(cr, surface, x, y);
                cairo_paint(cr);
                std::cout << "Not impl drawRGB std::endian::little" << std::endl;
            }

        } else if constexpr (std::endian::native == std::endian::big) {

        }
    }

    void Graphics::drawImage(Image* image, int x, int y, int anchor) {
        int width = image->getWidth();
        int height = image->getHeight();
        x = anchorX(anchor, x, width);
        y = anchorY(anchor, y, height);

        cairo_set_source_surface(cr, image->getCairoSurface(), x, y);
        cairo_paint(cr);
    }

    void Graphics::drawImage(Image* image, int x, int y, int dst_width, int dst_height, int anchor) {
        int width = image->getWidth();
        int height = image->getHeight();

        x = anchorX(anchor, x, width);
        y = anchorY(anchor, y, height);

        double rect_x = x;
        double rect_y = y;
        double rect_width = width;
        double rect_height = height;

        double scale_x = dst_width / rect_width ;
        double scale_y = dst_height / rect_height ;


        cairo_save(cr);
        cairo_translate(cr, rect_x, rect_y);
        cairo_scale(cr, scale_x, scale_y);
        cairo_set_source_surface(cr, image->getCairoSurface(), 0, 0);
        cairo_paint(cr);
        cairo_restore(cr);
    }

    void Graphics::drawRegion(Image* image, int x_src, int y_src, int w_src, int h_src, int x_dst, int y_dst, int anchor) {
        int image_width = image->getWidth();
        int image_height = image->getHeight();

        x_dst = anchorX(anchor, x_dst, image_width);
        y_dst = anchorY(anchor, y_dst, image_height);
        int x = x_dst - x_src;
        int y = y_dst - y_src;

        cairo_save(cr);
        cairo_rectangle(cr, x_dst, y_dst, w_src, h_src);
        cairo_clip(cr);

        cairo_set_source_surface(cr, image->getCairoSurface(), x, y);
        cairo_paint(cr);
        cairo_restore(cr);
    }

    void Graphics::drawRegion(Image* image, int x_src, int y_src, int w_src, int h_src, int transform, int x_dst, int y_dst, int anchor) {
        if (transform == 0) {
            drawRegion(image, x_src, y_src, w_src, h_src, x_dst, y_dst, anchor);
            return;
        }

        cairo_matrix_t matrix;
        int H = w_src, V = h_src, DX = 0, DY = 0;
        switch (transform) {
            case TRANS_NONE: {
                cairo_matrix_init_identity(&matrix);
                break;
            }
            case TRANS_ROT90: {
                cairo_matrix_init_rotate(&matrix, M_PI);
                H = h_src;
                V = w_src;
                DY = -h_src;
                break;
            }
            case TRANS_ROT180:
                cairo_matrix_init_rotate(&matrix, M_PI * 2);
                DX = -w_src;
                DY = -h_src;
                break;
            case TRANS_ROT270:
                cairo_matrix_init_rotate(&matrix, M_PI * 3);
                H = h_src;
                V = w_src;
                DX = -w_src;
                break;
            case TRANS_MIRROR:
                cairo_matrix_init_scale(&matrix, -1, 1);
                DX = -w_src;
                break;
            case TRANS_MIRROR_ROT90:
                cairo_matrix_init_rotate(&matrix, M_PI);
                cairo_matrix_scale(&matrix, -1, 1);
                H = h_src;
                V = w_src;
                DY = -h_src;
                DX = -w_src;
                break;
            case TRANS_MIRROR_ROT180:
                cairo_matrix_init_rotate(&matrix, M_PI * 2);
                cairo_matrix_scale(&matrix, -1, 1);
                DY = -h_src;
                break;
            case TRANS_MIRROR_ROT270:
                cairo_matrix_init_rotate(&matrix, M_PI * 3);
                cairo_matrix_scale(&matrix, -1, 1);
                H = h_src;
                V = w_src;
                break;
            default:
                return;
        }

        //TODO  check this
        x_dst = anchorX(anchor, x_dst, H);
        y_dst = anchorY(anchor, y_dst, V);

        cairo_save(cr);
        cairo_translate(cr, x_dst , y_dst);
        cairo_transform(cr, &matrix);
        cairo_translate(cr, DX, DY);
        this->drawRegion(image, x_src, y_src, w_src, h_src, 0,0);
        cairo_restore(cr);
    }

    void Graphics::fillRect(int x, int y, int w, int h) {
        cairo_rectangle (this->cr, x, y, w, h);
        cairo_fill(cr);
    }

    void Graphics::fillRoundRect(int x, int y, int w, int h, int arcWidth, int arcHeight) {
        cairo_new_path(cr);
        cairo_move_to(cr, x + arcWidth, y);
        cairo_line_to(cr, x + w - arcWidth, y);
        cairo_curve_to(cr, x + w, y, x + w, y, x + w, y + arcHeight);
        cairo_line_to(cr, x + w, y + h - arcHeight);
        cairo_curve_to(cr, x + w, y + h, x + w, y + h, x + w - arcWidth, y + h);
        cairo_line_to(cr, x + arcWidth, y + h);
        cairo_curve_to(cr, x, y + h, x, y + h, x, y + h - arcHeight);
        cairo_line_to(cr, x, y + arcHeight);
        cairo_curve_to(cr, x, y, x, y, x + arcWidth, y);
        cairo_close_path(cr);
        cairo_fill(cr);
    }

    void Graphics::fillArc(int x, int y, int w, int h, int startAngle, int arcAngle) {
        double xc = x + w / 2.0;
        double yc = y + h / 2.0;
        double xr = w / 2.0;
        double yr = h / 2.0;
        double start_radians = startAngle * M_PI / 180.0;
        double arc_radians = arcAngle * M_PI / 180.0;

        cairo_save(cr);
        cairo_translate(cr, xc, yc);
        cairo_scale(cr, 1.0, yr / xr);
        cairo_move_to(cr, xr * cos(start_radians), yr * sin(start_radians));
        cairo_scale(cr, xr, 1.0 / yr);
        cairo_arc(cr, 0, 0, 1.0, start_radians, start_radians + arc_radians);
        cairo_fill(cr);
        cairo_restore(cr);

    }

    void Graphics::fillCircle(int centerX, int centerY, int r) {
        cairo_arc(cr, centerX, centerY, r, 0, 2 * M_PI);
        cairo_fill(cr);
    }

    void Graphics::copyArea(int x_src, int y_src, int width_, int height_, int x_dst, int y_dst, int anchor) {

         double x1 = x_src;
         double y1 = y_src;
         double x2 = x_src + width_;
         double y2 = y_src + height_;

         double dest_x = x_dst;
         double dest_y = y_dst;

        cairo_rectangle(cr, dest_x, dest_y, width_, height_);
        cairo_clip(cr);
        // TODO 检查区域是否重合，不知道cairo有没有做处理
        cairo_set_source_surface(cr, surface_, x1, y1);
        cairo_paint(cr);
    }


    void Graphics::translate(int x, int y) {
        cairo_translate(cr, x, y);
    }

    void Graphics::setClip(int x, int y, int width, int height) {
        cairo_reset_clip(cr);
        cairo_rectangle(cr, x, y, width, height);
        cairo_clip(cr);
    }

    void Graphics::clipRect(int x, int y, int width, int height) {
        cairo_rectangle(cr, x, y, width, height);
        cairo_clip(cr);
    }

    int Graphics::getClipX() const {
        double x;
        cairo_clip_extents(cr, &x, nullptr, nullptr, nullptr);
        return static_cast<int>(x);
    }

    int Graphics::getClipY() const {
        double y;
        cairo_clip_extents(cr, nullptr, &y, nullptr, nullptr);
        return static_cast<int>(y);
    }

    int Graphics::getClipWidth() const {
        double width, height;
        cairo_clip_extents(cr, nullptr, nullptr, &width, nullptr);
        return static_cast<int>(width);
    }

    int Graphics::getClipHeight() const {
        double height;
        cairo_clip_extents(cr, nullptr, nullptr, nullptr, &height);
        return static_cast<int>(height);
    }

    int Graphics::getTranslateX() const {
        double x;
        cairo_get_current_point(cr, &x, nullptr);
        return static_cast<int>(x);
    }

    int Graphics::getTranslateY() const {
        double y;
        cairo_get_current_point(cr , nullptr, &y);
        return static_cast<int>(y);
    }

    int Graphics::save() {
        cairo_save(cr);
        return 0;
    }

    bool Graphics::restore() {
        cairo_restore(cr);
        return true;
    }

    bool Graphics::restoreToCount(int count) {
        return true;
    }

    void Graphics::clear(int color) {
        // double red = ((color >> 24) & 0xFF) / 255.0;
        // double green = ((color >> 16) & 0xFF) / 255.0;
        // double blue = ((color >> 8) & 0xFF) / 255.0;
        // double alpha = (color & 0xFF) / 255.0;
        //
        //
        // cairo_set_source_rgba(cr, red, green, blue, alpha);
        // cairo_paint(cr);
    }

    void Graphics::drawPoint(int x, int y) {
        cairo_move_to(cr, x, y);
        cairo_line_to(cr, x+1, y+1);
        cairo_stroke(cr);
    }



}

