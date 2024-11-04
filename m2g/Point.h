
#pragma once

namespace m2g{
    class Point {


    public:
        int x;
        int y;

        Point();
        Point(int x, int y);

        void offset(int x, int y);
        void offset(const Point& point);

    };
}



