//
// Created by Admin on 2023/5/27.
//

#ifndef M2G_POINT_H
#define M2G_POINT_H


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




#endif //M2G_POINT_H
