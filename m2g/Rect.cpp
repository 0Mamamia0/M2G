//
// Created by Admin on 2023/5/27.
//

#include "Rect.h"
#include "Point.h"

template <typename NUM>
static bool inRange(NUM num, NUM min, NUM max) {
    assert(max >= min);
    return (num >= min) && (num <= max);
}

 Rect Rect::makeWH(int width, int height) {
    assert(width >= 0 && height >= 0);
    return {0, 0, width, height};
}

 Rect Rect::makeXYWH(int x, int y, int width, int height) {
    assert(width >= 0 && height >= 0 && x + width >= x && x + height >= x);
    return {x, y, x + width, y + height};
}


 Rect Rect::makeLTRB(int left, int top, int right, int bottom) {
    if(left > right) std::swap(left, right);
    if(top > bottom) std::swap(top, bottom);
    return {left, top, right, bottom};
}

Rect::Rect(int left, int top, int right, int bottom)
        : left(left)
        , top(top)
        , right(right)
        , bottom(bottom){
}


bool Rect::contain(const Point &point) const{
    return contain(point.x, point.y);
}

bool Rect::contain(int x, int y) const{
    return inRange(x, this->left, this->right) && inRange(y, this->top, this->bottom);
}

bool Rect::contain(const Rect &other) const{
    return !this->isEmpty()
           && !other.isEmpty()
           && other.left < this->right
           && other.top < this->bottom
           && other.right >= this->left
           && other.bottom >= this->top;
}

bool Rect::isEmpty() const {
    return right <= left || bottom <= top;
}

void Rect::setZero() {
    this->left = this->top = this->right = this->bottom = 0;
}

void Rect::setXYWH(int x, int y, int width, int height) {
    this->left = x;
    this->top = y;
    this->right = x + width;
    this->bottom =  y + height;
}

int Rect::getWidth() const {
    return right - left;
}

int Rect::getHeight() const {
    return bottom - top;
}

Rect Rect::intersect(const Rect &rect) const {
    int left_ = std::max(left, rect.left);
    int top_ = std::max(top, rect.top);
    int right_ = std::min(right, rect.right);
    int bottom_ = std::min(bottom, rect.bottom);
    return {left_, top_, right_, bottom_};
}

Rect::Rect() {

}
