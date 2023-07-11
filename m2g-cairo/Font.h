//
// Created by Admin on 2024/2/20.
//

#ifndef CAIROFONT_H
#define CAIROFONT_H
#include "cairo/cairo-ft.h"
#include <string>

namespace m2g {
    class Font {
    public:
        Font(int face, int style, int size);
        ~Font();

        int getStyle() const;
        int getFace() const;
        int getSize() const;
        int getFontSize() const;



        int charWidth(char c) const;

        int charsWidth(const char* str, int len) const;

        int stringWidth(const std::string& str) const;

        int getBaseLinePosition() const;

        int getHeight() const;


        cairo_font_face_t* getCairoFace() const;
        const cairo_font_extents_t* getCairoFontExtents() const;

    private:
        int face_;
        int style_;
        int size_;
        int font_size_;

        FT_Face ft_face_;
        cairo_font_face_t* cr_face_;

        cairo_t* cr_;
        cairo_surface_t* surface_;
        cairo_font_extents_t extents_;
    };
}





#endif //CAIROFONT_H
