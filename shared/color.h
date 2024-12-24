#pragma once


#include <cstdint>

#define R(argb) static_cast<uint8_t>((argb >> 16) & 0xFF)
#define G(argb) static_cast<uint8_t>((argb >> 8) & 0xFF)
#define B(argb) static_cast<uint8_t>(argb & 0xFF)
#define A(argb) static_cast<uint8_t>((argb >> 24) & 0xFF)

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color() : r(0), g(0), b(0), a(0) {

    }

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {

    }

    explicit Color(int argb) : Color(R(argb), G(argb), B(argb), A(argb)) {
    }


    Color &operator=(int argb) {
        r = R(argb);
        g = G(argb);
        b = B(argb);
        a = A(argb);
        return *this;
    }

    uint8_t *addr8() {
        return (uint8_t *) this;
    }

    int ToARGB() const {
        int color = 0;
        color |= (a & 0xFF) << 24;
        color |= (r & 0xFF) << 16;
        color |= (g & 0xFF) << 8;
        color |= (b & 0xFF);
        return color;
    }
};


#undef A
#undef R
#undef G
#undef B

