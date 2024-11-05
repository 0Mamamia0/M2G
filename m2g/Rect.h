#pragma once
namespace m2g {
    class Point;
    class Rect {

    public:
        static Rect makeWH(int width, int height) ;
        static Rect makeXYWH(int x, int y, int width, int height);
        static Rect makeLTRB(int left, int top, int right, int bottom);

        Rect();
        Rect(int left, int top, int right, int bottom);

        void setZero();
        void setXYWH(int x, int y, int width, int height);

        bool isEmpty() const;
        bool contain(const Point& point) const;
        bool contain(int x, int y) const;
        bool contain(const Rect& other) const;
        int getWidth() const;
        int getHeight() const;

        Rect offset(int x, int y) const;
        Rect intersect(const Rect &rect) const;

    public:
        union {
            int left;
            int x;
        };

        union {
            int top;
            int y;
        };

        int right;
        int bottom;
    };

}



