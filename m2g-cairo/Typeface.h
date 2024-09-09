//
// Created by Admin on 2023/5/30.
//

#ifndef M2G_TYPEFACE_H
#define M2G_TYPEFACE_H

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
        static std::shared_ptr<Typeface> makeFormFile(const char* fileName);
    private:
        cairo_font_face_t* getCairoFace() const;

    private:
        FT_Face ft_face_;
        cairo_font_face_t* cr_face_;
    };

}



#endif //M2G_TYPEFACE_H
