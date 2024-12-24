#pragma once

#include <stack>
#include <memory>
#include <string>
#include <vector>
#include "color.h"
#include "Point.h"
#include "Clip.h"

namespace m2g {
    struct Glyph;

    class Rect;

    class Font;

    class Image;

    class PixelBuffer;


    class Graphics {
        using Translate = Point;

        struct State {
            Translate translate;
            Rect clip;
            Color paintColor;

            State(Translate translate, Rect clip, Color paintColor)
                    : translate(translate), clip(clip), paintColor(paintColor) {
            }

            State(const State &other) = default;

            State(State &&other) noexcept
                    : translate(other.translate), clip(other.clip), paintColor(other.paintColor) {
            }
        };

    public:
        explicit Graphics(Image *image);

        explicit Graphics(std::shared_ptr<PixelBuffer> buffer);

        Graphics(Image *image, int width, int height);

        Graphics(std::shared_ptr<PixelBuffer> buffer, int width, int height);

        Graphics(const Graphics &other);

        Graphics(Graphics &&other) noexcept;

        Graphics &operator=(const Graphics &s) = delete;

        Graphics &operator=(const Graphics &&s) = delete;

        void setColor(const Color &color);

        void setColor(int argb);

        int getColor();

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

        void clear(const Color &color);

        void drawPoint(int x, int y);

        void drawPoint(Point point);

        void drawLine(Point p1, Point p2);

        void drawLine(int x0, int y0, int x1, int y1);

        void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

        void drawTriangle(Point p1, Point p2, Point p3);

        void drawRect(int x, int y, int w, int h);

        void drawRect(const Rect &rect);

        void drawRoundRect(int x, int y, int w, int h, int arcWidth, int arcHeight);

        void drawArc(int x, int y, int w, int h, int startAngle, int arcAngle);

        void drawEllipse(int x, int y, int radiusX, int radiusY);

        void drawCircle(int centerX, int centerY, int r);

        void drawChar(char c, int x, int y, int anchor, const Font &font);
        void drawChar(char16_t c, int x, int y, int anchor, const Font &font);
        void drawGlyph(const Glyph *glyph, int x, int y, int anchor, const Font &font);

        void drawString(const char *str, int x, int y, int anchor, const Font &font);

        void drawString(const char *str, size_t len, int x, int y, int anchor, const Font &font);
        void drawString(const char16_t *str, size_t len, int x, int y, int anchor, const Font &font);
        void drawGlyphs(const std::vector<const Glyph *> &glyphs, int x, int y, int anchor, const Font &font);

        void drawString(const std::string &str, int x, int y, int anchor, const Font &font);

        void drawRGB(int *rgbData, int dataLength, int offset, int scanLength, int x, int y, int width, int height,
                     bool processAlpha);

        void drawImage(Image *image, int x, int y, int anchor = 0);

        void drawImage(Image *src, int x, int y, int dst_width, int dst_height, int anchor = 0);

        void drawImage(Image *image, int x_src, int y_src, int w_src, int h_src, int x_dst, int y_dst, int anchor = 0);

        void drawRegion(Image *src, int x_src, int y_src, int w_src, int h_src, int transform, int x_dst, int y_dst,
                        int anchor);

        void fillRect(int x, int y, int w, int h);

        void fillRect(const Rect &rect);

        void fillRoundRect(int ox, int oy, int w, int h, int arcWidth, int arcHeight);

        void fillEllipse(int x, int y, int radiusX, int radiusY);

        void fillArc(int x, int y, int w, int h, int startAngle, int arcAngle);

        void fillCircle(int centerX, int centerY, int r);

        void fillTriangle(Point p1, Point p2, Point p3);

        void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

        void copyArea(int x_src, int y_src, int width_, int height_, int x_dst, int y_dst, int anchor);

    private:
        void translatePoint(int &x, int &y) const;

        void translatePoint(int &x, int &y, int width, int height, int anchor);

        void setPixel(int x, int y, Color color);

        void drawPixel(int x, int y);

        void drawPixelAlpha(int x, int y, int alpha);

        void drawPointT(Point point);

        void drawLineH(int x0, int x1, int y);

        void drawLineV(int x, int y0, int y1);

        void drawLineB(int x0, int y0, int x1, int y1);

        void drawLineAA(int x0, int y0, int x1, int y1);

        void fillTriangleA(int x0, int y0, int x1, int x2, int y_base);



    private:
        int width;
        int height;
        Clip clip;
        Translate translation;
        Color paintColor;
        std::shared_ptr<PixelBuffer> buffer;
        std::stack<State> states;
    };
}



