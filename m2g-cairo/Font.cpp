//
// Created by Admin on 2024/2/20.
//

#include "Font.h"

#include <mutex>
#include <string>


#include "cairo/cairo.h"
#include "cairo/cairo-ft.h"


namespace m2g {

    enum FontSize {
        SMALL = 8,
        MEDIUM = 0,
        LARGE = 16,
    };

    static double getFontSize(int font_size) {
        switch (font_size) {
            case FontSize::SMALL: return 12;
            case FontSize::MEDIUM: return 18;
            case FontSize::LARGE: return 24;
            default:  return 18;
        }
    }

    static std::once_flag flag;
    static FT_Library ft_library;


    Font::Font(int face, int style, int size)
        : face_(face)
        , style_(style)
        , size_(size){
        std::call_once(flag, [] {
            FT_Init_FreeType(&ft_library);
            std::atexit([] {
                FT_Done_FreeType(ft_library);
            });
        });

        FT_New_Face(ft_library, "/simkai.ttf", 0, &ft_face_);
        this->font_size_ = ::m2g::getFontSize(size);
        this->cr_face_ = cairo_ft_font_face_create_for_ft_face(ft_face_, 0);
        this->surface_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 4, 4);
        this->cr_ = cairo_create(surface_);

        cairo_set_font_face(cr_, cr_face_);
        cairo_set_font_size(cr_, this->font_size_);
        cairo_font_extents(cr_, &extents_);
    }

    Font::~Font() {
        cairo_surface_destroy(surface_);
        cairo_destroy(cr_);
        cairo_font_face_destroy(cr_face_);
        FT_Done_Face(ft_face_);
    }


    int Font::getStyle() const {
        return style_;
    }

    int Font::getFace() const {
        return face_;
    }

    int Font::getSize() const {
        return size_;
    }

    int Font::getFontSize() const {
        return font_size_;
    }


    cairo_font_face_t* Font::getCairoFace() const {
        return cr_face_;
    }

    const cairo_font_extents_t* Font::getCairoFontExtents() const {
        return &extents_;
    }

    int Font::charWidth(char c) const {
        char buffer[2];
        buffer[0] = c;
        buffer[1] = '\0';
        cairo_text_extents_t extents;
        cairo_text_extents(cr_, buffer, &extents);
        return extents.width;
    }

    int Font::charsWidth(const char* str, int len) const {
        std::string string(str, len);
        return stringWidth(string);
    }

    int Font::stringWidth(const std::string& str) const {
        cairo_text_extents_t extents;
        cairo_text_extents(cr_, str.c_str(), &extents);
        return extents.width;
    }

    int Font::getBaseLinePosition() const {
        return static_cast<int>(extents_.height + (extents_.descent + extents_.ascent) / 2.0);
    }

    int Font::getHeight() const {
        return static_cast<int>(extents_.height);
    }
}
