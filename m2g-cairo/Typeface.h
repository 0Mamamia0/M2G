#pragma once

#include <memory>
#include "cairo/cairo-ft.h"

namespace m2g {
    class Typeface {
    public:
        friend class Font;
        Typeface(const Typeface& other) = delete;
        ~Typeface();
        void operator=(const Typeface& other) = delete;
    public:
        explicit Typeface(FT_Face face);
        static Typeface* makeFormFile(const char* fileName);
    private:
        cairo_font_face_t* getCairoFace() const;

    private:
        FT_Face ft_face_;
        cairo_font_face_t* cr_face_;
    };

}




