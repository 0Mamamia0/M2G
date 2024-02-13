//
// Created by Admin on 2023/5/27.
//

#ifndef M2G_CLIP_H
#define M2G_CLIP_H

#include "Rect.h"


namespace m2g {
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
        Clip(int width, int height);

        void setZero();

        void set(const Rect& rect);

        bool isEmpty() const;

        bool contain(const Point& point) const;

        bool contain(const Rect& rect) const;

        bool clipLineV(int x0, int &y0, int &y1) const;

        bool clipLineH(int &x0, int &x1, int &y0) const;

        bool clipLineB(int &x0, int &y0, int &x1, int &y1) const;

        void setXYWH(int x, int y, int width, int height);

        int getY() const;

        int getX() const;

        int getWidth() const;

        int getHeight() const;

        Rect getClipBounds() const;

        Rect getDeviceClipBounds() const;

        Rect clipRect(const Rect &rect) const;

        Rect intersect(const Rect& rect) const;
    };
}



#endif //M2G_CLIP_H
