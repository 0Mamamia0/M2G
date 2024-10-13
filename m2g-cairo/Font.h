//
// Created by Admin on 2024/2/20.
//

#ifndef CAIROFONT_H
#define CAIROFONT_H
#include "cairo/cairo-features.h"
#include "cairo/cairo-ft.h"


#include <string>
#include <memory>
#include "Typeface.h"


namespace m2g {

    typedef struct {
        float ascent;
        float descent;
        float lineGap;
        float baseline;
        float height;
    } FontMetrics;


    class Font {
    public:
        Font(std::shared_ptr<Typeface> typeface, int face, int style, int size);
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
        const FontMetrics& getFontMetrics() const;
        cairo_font_face_t* getCairoFace() const;
        const cairo_font_extents_t* getCairoFontExtents() const;
    private:
        int face_;
        int style_;
        int size_;
        int font_size_;

        cairo_t* cr_;
        cairo_surface_t* surface_;
        cairo_font_extents_t extents_;

        FontMetrics metrics_;
        std::shared_ptr<Typeface> typeface_;
    };
}





#endif //CAIROFONT_H
