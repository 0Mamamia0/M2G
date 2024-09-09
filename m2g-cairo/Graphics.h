//
// Created by Admin on 2024/2/20.
//

#ifndef CAIROGRAPHICS_H
#define CAIROGRAPHICS_H

#include <string>
#include "cairo/cairo.h"
#include "Image.h"


namespace m2g {

    class Font;
    class Graphics {
    public:
        explicit Graphics(Image* image);

        explicit Graphics(cairo_surface_t* surface);

        // explicit Graphics(std::shared_ptr<PixelBuffer> buffer);
        //
        // Graphics(Image* image, int width, int height);
        //
        // Graphics(std::shared_ptr<PixelBuffer> buffer, int width, int height);

        Graphics(const Graphics& other);

        Graphics(Graphics&& other) noexcept;

        ~Graphics();

        Graphics& operator =(const Graphics&s) = delete;

        Graphics& operator =(const Graphics&&s) = delete;

        // void setColor(const Color& color);

        void reset();

        void setColor(int argb);

        int getColor() const;

        void translate(int x, int y);

        void setClip(int x, int y, int width, int height);

        void clipRect(int x, int y, int width, int height);

        int getClipX() const;

        int getClipY() const;

        int getClipWidth() const;

        int getClipHeight() const;

        int getTranslateX() const;

        int getTranslateY() const;

        int save();

        bool restore();

        bool restoreToCount(int count);

        void clear(int color);

        // void clear(const Color&color);

        void drawPoint(int x, int y);

        // void drawPoint(Point point);

        void drawLine(int x0, int y0, int x1, int y1);

        void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

        void drawRect(int x, int y, int w, int h);

        // void drawRect(const Rect&rect);

        void drawRoundRect(int x, int y, int w, int h, int arcWidth, int arcHeight);

        void drawArc(int x, int y, int w, int h, int startAngle, int arcAngle);

        void drawEllipse(int x, int y, int radiusX, int radiusY);

        void drawCircle(int centerX, int centerY, int r);

         void drawChar(char c, int x, int y, int anchor, const Font& font);

        void drawString(const char* str, int x, int y, int anchor, const Font& font);

        void drawString(const char* str, size_t len, int x, int y, int anchor, const Font& font);

        void drawString(const std::string& str, int x, int y, int anchor, const Font& font);

        void drawRGB(int* rgbData, int dataLength, int offset, int scanLength, int x, int y, int width, int height, bool processAlpha);

        void drawImage(Image* image, int x, int y, int anchor = 0);

        void drawImage(Image* image, int x, int y, int dst_width, int dst_height, int anchor = 0);

        void drawRegion(Image* image, int x_src, int y_src, int w_src, int h_src, int x_dst, int y_dst, int anchor = 0);

        void drawRegion(Image* image, int x_src, int y_src, int w_src, int h_src, int transform, int x_dst, int y_dst,
                        int anchor);

        void fillRect(int x, int y, int w, int h);

        // void fillRect(const Rect&rect);

        void fillRoundRect(int x, int y, int w, int h, int arcWidth, int arcHeight);

        // void fillEllipse(int x, int y, int radiusX, int radiusY);
        //
        void fillArc(int x, int y, int w, int h, int startAngle, int arcAngle);
        //
        void fillCircle(int centerX, int centerY, int r);
        //
        // void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

        void copyArea(int x_src, int y_src, int width_, int height_, int x_dst, int y_dst, int anchor);

    private:
        int color;
        cairo_t* cr;
        cairo_surface_t* surface_;
    };
}

#endif //CAIROGRAPHICS_H
