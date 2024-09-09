
#include "Typeface.h"

#include <mutex>
#include <vector>


static std::once_flag flag;
static FT_Library ft_library;


namespace m2g {
    std::shared_ptr<Typeface> Typeface::makeFormFile(const char *fileName) {
        std::call_once(flag, [] {
            FT_Init_FreeType(&ft_library);
            std::atexit([] {
                FT_Done_FreeType(ft_library);
            });
        });
        FT_Face ft_face;
        FT_Error error = FT_New_Face(ft_library, fileName, 0, &ft_face);
        if(!error) {
            return std::make_shared<Typeface>(ft_face);
        }
        return nullptr;
    }

    Typeface::Typeface(FT_Face face)
            : ft_face_(face) {
        this->cr_face_ = cairo_ft_font_face_create_for_ft_face(ft_face_, 0);
    }


    Typeface::~Typeface() {
        cairo_font_face_destroy(cr_face_);
        FT_Done_Face(ft_face_);
    }

    cairo_font_face_t *Typeface::getCairoFace() const {
        return cr_face_;
    }
}




