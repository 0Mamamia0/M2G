//
// Created by Admin on 2023/5/23.
//

#ifndef NXJVM_GRAPHICS_H
#define NXJVM_GRAPHICS_H

#include <stack>
#include <algorithm>
#include <memory>
#include <utility>
#include <string>
#include <cstdint>

#include "Point.h"
#include "Clip.h"
#include "color.h"

class Glyph;
class Point;
class Rect;
class Image;
class Font;
class Clip;
class PixelBuffer;

enum {
    HCENTER = 1,
    VCENTER = 2,
    LEFT = 4,
    RIGHT = 8,
    TOP = 16,
    BOTTOM = 32,
    BASELINE = 64,
};









class Graphics {

    using Translate = Point;
    struct State {
        Translate translate;
        Rect clip;
        Color paintColor;

        State(Translate translate, Rect clip, Color paintColor)
                : translate(translate), clip(clip), paintColor(paintColor){

        }

    };

public:
    explicit Graphics(std::shared_ptr<PixelBuffer> buffer);
    Graphics(std::shared_ptr<PixelBuffer> buffer, int width, int height);
    Graphics(const Graphics& other);
    Graphics(Graphics&& other) noexcept;
    Graphics& operator = (const Graphics& s) = delete;
    Graphics& operator = (const Graphics&& s) = delete;

    void setColor(const Color& color);
    void setColor(int argb);
    int getColor();
    void translate(int x, int y);
    int getTranslateX() const;
    int getTranslateY() const;
    void setClip(int x, int y, int width, int height);
    void clipRect(int x, int y, int width, int height);
    int getClipX();
    int getClipY();
    int getClipWidth();
    int getClipHeight();
    int save();
    bool restore();
    bool restoreToCount(int count);

    void clear(Color color);

    void drawPixel(int x, int y);
    void drawPixelAlpha(int x, int y, int alpha);
    void drawLineH(int x0, int x1, int y);
    void drawLineV(int x, int y0, int y1);
    void drawLineB(int x0, int y0, int x1, int y1);
    void drawLineAA(int x0, int y0, int x1, int y1);

    void drawPoint(int x, int y);
    void drawPoint(Point point);
    void drawLine(int x0, int y0, int x1, int y1);
    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2);
    void drawRect(int x, int y, int w, int h);
    void drawRect(const Rect& rect);
    void drawRoundRect(int x, int y, int w, int h, int arcWidth, int arcHeight);
    void drawArc(int x, int y, int w, int h, int startAngle, int arcAngle);
    void drawEllipse(int x, int y, int radiusX, int radiusY);
    void drawCircle(int centerX, int centerY, int r);

    void drawChar(char c, int x, int y, int anchor, const Font& font);
    void drawString(const char* str, int x, int y, int  anchor, const Font& font);
    void drawString(const std::string& str, int x, int y, int  anchor, const Font& font);


    void drawARGB(uint8_t* argb, int dataLength, int offset, int scanLength, int x, int y, int width, int height);
    void drawBGRA(uint8_t* bgra,  int dataLength, int offset, int scanLength, int x, int y, int width, int height);


    void drawRGB(int* rgbData, int dataLength, int offset, int scanLength, int x, int y, int width, int height, bool processAlpha);
    void drawImage(Image* image, int x, int y, int anchor = 0);
    void drawImage(Image* src, int x, int y, int dstWidth, int dstHeight, int anchor = 0);
    void drawImage(Image* image, int x_src, int y_src, int w_src, int h_src, int x_dst, int y_dst, int anchor = 0);
    void drawRegion(Image* src, int x_src, int y_src, int w_src, int h_src, int transform, int x_dst, int y_dst, int anchor);

    void fillRect(int x, int y, int w, int h);
    void fillRect(const Rect& rect);
    void fillRoundRect(int x, int y, int w, int h, int arcWidth, int arcHeight);
    void fillEllipse(int x, int y, int radiusX, int radiusY);
    void fillArc(int x, int y, int w, int h, int startAngle, int arcAngle);
    void fillCircle(int centerX, int centerY, int r);
    void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2);
    void copyArea(int x_src, int y_src, int width_, int height_, int x_dst, int y_dst, int anchor);

    void translatePoint(int& x, int& y);
    void translatePoint(int& x, int& y, int width, int height, int anchor);

    void fillTriangleA(int x0, int y0, int x1, int x2, int y_base);
    void fillTriangleV(int x0, int y0, int x1, int x2, int y_base);

    void setPixel(int x, int y, Color color);



private:
    int width;
    int height;
    Clip clip;
    Translate translation;
    Color paintColor;
    std::shared_ptr<PixelBuffer> buffer;
    std::stack<State> states;

};


#endif //NXJVM_GRAPHICS_H
