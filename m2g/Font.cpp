

#include "Font.h"
#include <iostream>
#include <fstream>
#include <m2g-def.h>
#include <vector>
#include <cmath>
#include "Typeface.h"
#include "Rect.h"
#include "utf.h"
#include "stb_truetype.h"


namespace m2g {
    static float getFontSize(int font_size) {
        switch (font_size) {
            case FontSize::SMALL:
                return 12;
            case FontSize::MEDIUM:
                return 18;
            case FontSize::LARGE:
                return 24;
            default:
                return 18;
        }
    }


    Font::Font(Typeface* typeface, int face, int style, int size)
            : face(face), style(style), size(size), invalid_glyph_width(-1), typeface(typeface), metrics{},
              ascii_glyph_cache{nullptr} {

        const stbtt_fontinfo &fontInfo = this->typeface->getFontInfo();
        int ascent, descent, lineGap;
        stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

        float pixel_height = getFontSize(size);
        this->scale = stbtt_ScaleForPixelHeight(&fontInfo, pixel_height);
        this->metrics.ascent = (float) ascent * scale;
        this->metrics.descent = (float) descent * scale;
        this->metrics.lineGap = (float) lineGap * scale;
        this->metrics.baseline = metrics.ascent;
        this->metrics.height = metrics.ascent - metrics.descent + metrics.lineGap;

    }


    Font::~Font() {

        for (auto *glyph: ascii_glyph_cache) {
            freeGlyph(glyph);
        }

        for (auto &[fst, glyph]: glyph_cache) {
            freeGlyph(glyph);
        }
    }


    int Font::getStyle() const {
        return style;
    }

    int Font::getFace() const {
        return face;
    }

    int Font::getSize() const {
        return size;
    }


    int Font::getBaseLinePosition() const {
        return (int) std::round(metrics.baseline);
    }

    int Font::getHeight() const {
        return (int) std::round(metrics.height);;
    }


    const Glyph *Font::findGlyph(char c) const {
        int codepoint = static_cast<unsigned char>(c);
        return this->findGlyph(codepoint);
    }


    const Glyph *Font::findGlyph(int codepoint) const {
        if (codepoint >= 32 && codepoint <= 127) {
            return ascii_glyph_cache[codepoint - 32];
        }
        auto find = glyph_cache.find(codepoint);
        return find == glyph_cache.end() ? nullptr : find->second;
    }


    const Glyph *Font::getGlyph(char c) const {
        return this->getGlyph((int) c);
    }

    const Glyph *Font::getGlyph(int codepoint) const {
        if (const auto *glyph = findGlyph(codepoint)) {
            return glyph;
        }
        return loadGlyph(codepoint);
    }

    Glyph *Font::loadGlyph(int codepoint) const {
        const stbtt_fontinfo *info = &typeface->getFontInfo();
        int id = stbtt_FindGlyphIndex(info, codepoint);

        if (id != 0) {
            int advance;
            int bearing;
            int width;
            int height;
            Rect bounds;
            uint8_t *bitmap;

            stbtt_GetGlyphHMetrics(info, id, &advance, &bearing);
            stbtt_GetGlyphBitmapBox(info, id, scale, scale, &bounds.left, &bounds.top, &bounds.right, &bounds.bottom);
            bitmap = stbtt_GetGlyphBitmap(info, scale, scale, id, &width, &height, nullptr, nullptr);

            auto *glyph = new Glyph(id, scale * (float) advance, scale * (float) bearing, width, height, bounds,
                                    bitmap);
            if (ASCII_MIN <= codepoint && codepoint <= ASCII_MAX) {
                ascii_glyph_cache[codepoint - ASCII_MIN] = glyph;
            } else {
                glyph_cache.insert(std::make_pair(codepoint, glyph));
            }
            return glyph;
        }
        return nullptr;
    }


    void Font::freeGlyph(Glyph *glyph) const {
        if (glyph != nullptr) {
            if (glyph->bitmap != nullptr) {
                stbtt_FreeBitmap(glyph->bitmap, nullptr);
            }
            delete glyph;
        }
    }

    float Font::getAdvance(const Glyph *g1, const Glyph *g2) const {
        if (g1 == nullptr || g2 == nullptr) {
            return 0;
        }
        return (float) stbtt_GetGlyphKernAdvance(&typeface->getFontInfo(), g1->id, g2->id) * scale;

    }


    int Font::codePointWidth(int codepoint) const {
        const Glyph *glyph = this->getGlyph(codepoint);
        return glyphWidth(glyph);
    }

    int Font::charWidth(char c) const {
        const Glyph *glyph = this->getGlyph(c);
        return glyphWidth(glyph);
    }


    int Font::glyphWidth(const Glyph *glyph) const {
        return std::ceil(glyphWidthF(glyph));
    }

    float Font::glyphWidthF(const Glyph *glyph) const {
        return glyph == nullptr ? 0 : glyph->advance;
    }


    int Font::charsWidthUTF16(const uint16_t *str, int len) const {
        float width = 0;
        for (auto codepoint: utf::range((uint8_t *) str, len)) {
            width += glyphWidthF(getGlyph(codepoint));
        }
        return (int) std::ceil(width);
    }




    int Font::glyphsWidth(const std::vector<const Glyph *> &glyphs) const {
        float width = 0;
        for (auto glyph: glyphs) {
            width += glyphWidthF(glyph);
        }
        return (int) std::ceil(width);;
    }

    int Font::charsWidth(const char *str, int len) const {
        float width = 0;
        for (auto codepoint: utf::range((uint8_t *) str, len)) {
           width += glyphWidthF(getGlyph(codepoint));
        }
        return (int) std::ceil(width);
    }

    int Font::stringWidth(const std::string &str) const {
        return this->charsWidth(str.c_str(), (int) str.length());
    }


    const FontMetrics &Font::getFontMetrics() const {
        return metrics;
    }

    std::vector<const Glyph *> Font::findGlyphs(const char *str, size_t len) const {
        std::vector<const Glyph *> glyphs;
        glyphs.reserve(len);
        for (auto codepoint: utf::range((uint8_t *) str, len)) {
            glyphs.push_back(getGlyph(codepoint));
        }
        return glyphs;
    }

    std::vector<const Glyph *> Font::findGlyphs(const char16_t *str, size_t len) const {
        std::vector<const Glyph *> glyphs;
        glyphs.reserve(len);
        for (auto codepoint: utf::range((uint16_t *) str, len)) {
            glyphs.push_back(getGlyph(codepoint));
        }
        return glyphs;
    }

    int Font::invalidGlyphWidth() const {
        if (invalid_glyph != nullptr) {
            return glyphWidth(invalid_glyph);
        }


        return 0;
    }


}
