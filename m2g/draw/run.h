#pragma once

#include <cmath>

template<bool fill, typename Fun>
void arcRun(int rx, int ry, Fun fun) {
    int rx2 = rx * rx;
    int ry2 = ry * ry;
    int p;

    int x = 0, y = ry; // (区域1)初始点
    int px = 0, py = 2 * rx2 * y; // 像素计算终止条件项

    fun(x, y);

    p = round(ry2 - rx2 * ry + 0.25 * rx2);
    while (px < py) {
        x++;
        px += 2 * ry2; // 通过累加来计算 2ry^2 x[k+1], 而不是每次都用乘法
        if (p < 0) {
            p += ry2 + px;
            if constexpr (!fill) {
                fun(x, y);
            }
        } else {
            y--;
            py -= 2 * rx2;
            p += ry2 + px - py;
            fun(x, y);
        }
    }

    // 区域2
    p = round(ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2);
    while (y > 0) {
        y--;
        py -= 2 * rx2;
        if (p > 0) {
            p += rx2 - py;
        } else {
            x++;
            px += 2 * ry2;
            p += rx2 - py + px;
        }
        fun(x, y);
    }
}


template<bool fill, typename Fun>
void circleRun(int r, Fun fun) {
    int y = r;
    int p = 1 - r;
    for (int x = 0; x < y; x++) {
        if (p < 0) {
            // 中点在园内
            p += 2 * x + 1;
        } else {
            // 中点在圆外或圆上
            y -= 1;
            p += 2 * (x - y) + 1;
            if constexpr (fill) {
                fun(x, y);
            }
        }
        fun(y, x);
    }
}