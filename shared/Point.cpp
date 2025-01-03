﻿
#include "Point.h"

namespace m2g {
    Point::Point() : x(0), y(0){

    }

    Point::Point(int x, int y): x(x), y(y) {

    }

    void Point::offset(int x_, int y_) {
        this->x += x_;
        this->y += y_;
    }

    void Point::offset(const Point &point) {
        this->x += point.x;
        this->y += point.y;
    }

}

