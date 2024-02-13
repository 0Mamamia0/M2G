//
// Created by Admin on 2023/5/23.
//

#ifndef M2G_FONT_H
#define M2G_FONT_H

#include <string>
#include <cstdint>
#include <array>
#include <unordered_map>
#include "Typeface.h"
#include "Rect.h"


namespace m2g {
    typedef struct {
        float ascent;
        float descent;
        float lineGap;
        float baseline;
        float height;
    } FontMetrics;

    struct Glyph {
        int id;
        float advance;
        float bearing;
        int width;
        int height;
        Rect bounds;
        uint8_t* bitmap;

        Glyph() : Glyph(0, 0, 0, 0, 0, {}, nullptr) {
        }

        Glyph(const Glyph&other) : Glyph(other.id, other.advance, other.bearing, other.width, other.height,
                                         other.bounds, other.bitmap) {
        }

        Glyph(int id, float advance, float bearing, int width, int height, const Rect&bounds, uint8_t* bitmap)
            : id(id)
              , advance(advance)
              , bearing(bearing)
              , width(width)
              , height(height)
              , bounds(bounds)
              , bitmap(bitmap) {
        };

        void set(int id_, float advance_, float bearing_, int width_, int height_, const Rect&bounds_,
                 uint8_t* bitmap_) {
            this->id = id_;
            this->advance = advance_;
            this->bearing = bearing_;
            this->width = width_;
            this->height = height_;
            this->bounds = bounds_;
            this->bitmap = bitmap_;
        }

        void set(const Glyph& other) {
            this->set(other.id, other.advance, other.bearing, other.width, other.height, other.bounds, other.bitmap);
        }

        bool empty() const {
            // return this == nullptr || id == 0;
            return id == 0;
        }
    };


    class Font {
    public:
        Font(Typeface* typeface, int face, int style, int size);

        ~Font();

        Font(const Font& other) = delete;

        Font(Font&& other) noexcept = delete;

        int charWidth(char c) const;

        int charsWidth(const char* str, int len) const;

        int stringWidth(const std::string&str) const;

        int getBaseLinePosition() const;

        int getHeight() const;

        int getStyle() const;

        int getFace() const;

        int getSize() const;

        const Glyph* findGlyph(char c) const;

        const Glyph* findGlyph(int codepoint) const;

        const Glyph* getGlyph(char c) const;

        const Glyph* getGlyph(int codepoint) const;

        float getAdvance(const Glyph* g1, const Glyph* g2) const;

        const FontMetrics& getFontMetrics() const;

        static const Font& getDefaultFont();

    private:
        Glyph* loadGlyph(int codepoint) const;

    private:
        int face;
        int style;
        int size;
        float scale;
        Typeface* typeface;
        FontMetrics metrics;
        mutable std::unordered_map<int, Glyph *> glyph_cache;
        mutable std::array<Glyph *, 96> ascii_glyph_cache;
    };
}


#endif //M2G_FONT_H
