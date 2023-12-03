//
// Created by Admin on 2023/5/27.
//

#ifndef LEARNSFML_RECT_H
#define LEARNSFML_RECT_H


#include <cassert>
#include <algorithm>


class Point;

class Rect {


public:
    static Rect makeWH(int width, int height) ;
    static Rect makeXYWH(int x, int y, int width, int height);
    static Rect makeLTRB(int left, int top, int right, int bottom);

public:
    Rect();
    Rect(int left, int top, int right, int bottom);

    bool isEmpty() const;
    bool contain(const Point& point) const;
    bool contain(int x, int y) const;
    bool contain(const Rect& other) const;


public:
    int left;
    int top;
    int right;
    int bottom;

    void setZero();

    void setXYWH(int x, int y, int width, int height);

    int getWidth() const;

    int getHeight() const;

    Rect intersect(const Rect &rect) const;
};


#endif //LEARNSFML_RECT_H
