//
// Created by Admin on 2023/7/5.
//

#ifndef LEARNSFML_COLOR_H
#define LEARNSFML_COLOR_H

#include <cstdint>

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;


    Color(): r(0), g(0), b(0), a(0){

    }

    explicit Color(int argb):
            r(static_cast<uint8_t>((argb >> 16) & 0xFF)),
            g(static_cast<uint8_t>((argb >> 8) & 0xFF)),
            b(static_cast<uint8_t>(argb & 0xFF)),
            a(static_cast<uint8_t>((argb >> 24) & 0xFF)){
    }

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a): r(r), g(g), b(b), a(a){

    }




    Color& operator = (int argb) {
        r = static_cast<uint8_t>((argb >> 16) & 0xFF);
        g = static_cast<uint8_t>((argb >> 8) & 0xFF);
        b = static_cast<uint8_t>(argb & 0xFF);
        a = static_cast<uint8_t>((argb >> 24) & 0xFF);
        return *this;
    }


    uint8_t* addr8() {
        return (uint8_t*)this;
    }

    int ToARGB() const {
        int color = 0;
        color |= (a & 0xFF) << 24;
        color |= (r & 0xFF) << 16;
        color |= (g & 0xFF) << 8;
        color |= (b & 0xFF);
        return color;
    }

//    Color operator*(Color color, float s) {
//        return {
//            static_cast<uint8_t>(color.r * s),
//            static_cast<uint8_t>(color.g * s),
//            static_cast<uint8_t>(color.b * s),
//            static_cast<uint8_t>(color.a * s)
//        };
//    }
//
//    Color operator+ (Color c0, Color c1) {
//        return {
//                static_cast<uint8_t>(c0.r + c1.r),
//                static_cast<uint8_t>(c0.g + c1.g),
//                static_cast<uint8_t>(c0.b + c1.b),
//                static_cast<uint8_t>(c0.a + c1.a)
//        };
//    }

};

#endif //LEARNSFML_COLOR_H
