//
// Created by Admin on 2023/5/27.
//

#ifndef LEARNSFML_CLIP_H
#define LEARNSFML_CLIP_H

#include "Rect.h"

enum {
    CLIP_INSIDE = 0,
    CLIP_LEFT = 1,
    CLIP_RIGHT = 2,
    CLIP_BOTTOM = 4,
    CLIP_TOP = 8,
};


class Point;
class Rect;

class Clip {
    Rect bounds;
    int maxWidth;
    int maxHeight;
public:

public:
    Clip(int width, int height);

    void setZero();



    bool isEmpty();

    bool contain(const Point& point) const;

    bool contain(const Rect& rect) const;

    bool clipLineV(int x0, int &y0, int &y1) const;

    bool clipLineH(int &x0, int &x1, int &y0) const;

    bool clipLineB(int &x0, int &y0, int &x1, int &y1);

    void setXYWH(int x, int y, int width, int height);

    int getX();

    int getWidth() const;

    int getHeight() const;

    int getY();

    Rect getClipBounds() const;

    Rect getDeviceClipBounds() const;


    Rect clipRect(const Rect &rect);

    void set(const Rect& rect);

    Rect intersect(const Rect& rect) const;
};


#endif //LEARNSFML_CLIP_H
