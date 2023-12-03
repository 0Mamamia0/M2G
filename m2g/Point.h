//
// Created by Admin on 2023/5/27.
//

#ifndef LEARNSFML_POINT_H
#define LEARNSFML_POINT_H


class Point {


public:
    int x;
    int y;

    Point();
    Point(int x, int y);

    void offset(int x, int y);
    void offset(const Point& point);

};


#endif //LEARNSFML_POINT_H
