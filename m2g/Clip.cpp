//
// Created by Admin on 2023/5/27.
//


#include <algorithm>
#include "Clip.h"
#include "Rect.h"





static int code(const Rect& rect, int x, int y) {
    int code = CLIP_INSIDE;
    if (x < rect.left) {
        code |= CLIP_LEFT;
    } else if (x >= rect.right) {
        code |= CLIP_RIGHT;
    }
    if (y < rect.top) {
        code |= CLIP_TOP;
    } else if (y >= rect.bottom) {
        code |= CLIP_BOTTOM;
    }
    return code;
}


void Clip::setZero() {
   bounds.setZero();
}


bool Clip::contain(const Point &point) const {
    return bounds.contain(point);
}

bool Clip::isEmpty() {
    return bounds.isEmpty();
}


bool Clip::clipLineV(int x, int& y0, int& y1) const {
    Rect rect = getDeviceClipBounds();
    if(rect.isEmpty()) {
        return false;
    }

    if(x < rect.left || x >= rect.right ) {
        return false;
    }

    y0 = std::clamp(y0, rect.top, rect.bottom - 1);
    y1 = std::clamp(y1, rect.top, rect.bottom - 1);
    return true;
}

bool Clip::clipLineH(int& x0, int& x1, int& y) const {
    Rect rect = getDeviceClipBounds();
    if(rect.isEmpty()) {
        return false;
    }

    if(y < rect.top ||y >= rect.bottom ) {
        return false;
    }

    x0 = std::clamp(x0, rect.left, rect.right - 1);
    x1 = std::clamp(x1, rect.left, rect.right - 1);
    return true;
}

bool Clip::clipLineB(int& x0, int& y0, int& x1, int& y1) {

    Rect rect = getDeviceClipBounds();
    if(rect.isEmpty()) {
        return false;
    }

    int y = rect.top;
    int x = rect.left;
    int x_max = rect.right - 1;
    int y_max = rect.bottom - 1;
    int code1 = code(rect, x0, y0);
    int code2 = code(rect, x1, y1);

    while (true) {
        if(!(code1 | code2)) {
            return true;
        } else if(code1 & code2) {
            return false;
        } else  {
            int new_x, new_y;
            int codeOut = code1 ? code1 : code2;

            if (codeOut & CLIP_TOP) {        // 点在上方区域外
                new_x = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
                new_y = y;
            } else if (codeOut & CLIP_BOTTOM) {   // 点在下方区域外
                new_x = x0 + (x1 - x0) * (y_max - y0) / (y1 - y0);
                new_y = y_max;
            } else if (codeOut & CLIP_RIGHT) {    // 点在右侧区域外
                new_y = y0 + (y1 - y0) * (x_max - x0) / (x1 - x0);
                new_x = x_max;
            } else if (codeOut & CLIP_LEFT) {     // 点在左侧区域外
                new_y = y0 + (y1 - y0) * (x - x0) / (x1 - x0);
                new_x = x;
            }
            if (codeOut == code1) {         // 更新第一个端点
                x0 = new_x;
                y0 = new_y;
                code1 = code( rect, new_x, new_y);
            } else {                        // 更新第二个端点
                x1 = new_x;
                y1 = new_y;
                code2 = code(rect, new_x, new_y);
            }
        }
    }

}

Clip::Clip(int width, int height)
    : bounds {0, 0, width, height}
    , maxWidth(width)
    , maxHeight(height){
}

void Clip::setXYWH(int x, int y, int width, int height) {
    bounds.setXYWH(x, y, width, height);
}

int Clip::getX() {
    return bounds.left;
}

int Clip::getWidth() const{
    return bounds.right - bounds.left;
}

int Clip::getHeight() const{
    return bounds.bottom - bounds.top;
}

int Clip::getY() {
    return bounds.top;
}

Rect Clip::getClipBounds() const {
    return bounds;
}

Rect Clip::clipRect(const Rect &rect) {
    return bounds.intersect(rect);
}

Rect Clip::getDeviceClipBounds() const {
    return bounds.intersect(Rect::makeWH(maxWidth, maxHeight));
}

void Clip::set(const Rect& rect) {
    this->bounds = rect;
}

Rect Clip::intersect(const Rect& rect) const {
    return bounds.intersect(rect);
}

bool Clip::contain(const Rect &rect) const {
    return bounds.contain(rect);
}

