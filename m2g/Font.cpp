//
// Created by Admin on 2023/5/23.
//

#include "Font.h"
#include <iostream>
#include <fstream>
#include <m2g-def.h>
#include <vector>
#include <cmath>
#include <UTF8Iterator.h>
#include "Typeface.h"
#include "Rect.h"
#include "strConver.h"
#include "stb_truetype.h"


namespace m2g {


    static float getFontSize(int font_size) {
        switch (font_size) {
            case FontSize::SMALL: return 12;
            case FontSize::MEDIUM: return 18;
            case FontSize::LARGE: return 24;
            default:  return  18;
        }
    }


    Font::Font(std::shared_ptr<Typeface> typeface, int face, int style, int size)
        : face(face)
        , style(style)
        , size(size)
        , typeface(std::move(typeface))
        , metrics{}
        , ascii_glyph_cache{nullptr} {

        const stbtt_fontinfo& fontInfo = this->typeface->getFontInfo();
        int ascent, descent, lineGap;
        stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

        float pixel_height = getFontSize(size);
        this->scale = stbtt_ScaleForPixelHeight(&fontInfo, pixel_height);
        this->metrics.ascent = (float)ascent * scale;
        this->metrics.descent = (float)descent * scale;
        this->metrics.lineGap = (float)lineGap * scale;
        this->metrics.baseline = metrics.ascent;


        this->metrics.height = metrics.ascent - metrics.descent + metrics.lineGap;




        for (int i = 0; i < ascii_glyph_cache.size(); ++i) {
            this->ascii_glyph_cache[i] = loadGlyph(i + 32);
        }
    }


    Font::~Font() {
        for (auto* glyph : ascii_glyph_cache) {
            if (glyph != nullptr) {
                if (glyph->bitmap != nullptr) {
                    stbtt_FreeBitmap(glyph->bitmap, nullptr);
                }
            }
        }

        for (auto* glyph : ascii_glyph_cache) {
            delete glyph;
        }

        for (auto& [fst, glyph] : glyph_cache) {
            if (glyph != nullptr) {
                if (glyph->bitmap != nullptr) {
                    stbtt_FreeBitmap(glyph->bitmap, nullptr);
                }
            }
        }

        for (auto& [fst, glyph] : glyph_cache) {
            delete glyph;
        }
        glyph_cache.clear();
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
        return (int)std::round(metrics.baseline);
    }

    int Font::getHeight() const {
        return (int)std::round(metrics.height);;
    }


    const Glyph* Font::findGlyph(char c) const {
        return this->findGlyph((int)c);
    }

    const Glyph* Font::findGlyph(int codepoint) const {
        if (codepoint >= 32 && codepoint <= 127) {
            return ascii_glyph_cache[codepoint - 32];
        }
        auto find = glyph_cache.find(codepoint);
        return find == glyph_cache.end() ? nullptr : find->second;
    }


    const Glyph* Font::getGlyph(char c) const {
        return this->getGlyph((int)c);
    }

    const Glyph* Font::getGlyph(int codepoint) const {
        if(const auto* glyph = findGlyph(codepoint)) {
            return glyph;
        }

        if(auto* glyph = loadGlyph(codepoint)) {
            glyph_cache.insert(std::make_pair(codepoint, glyph));
            return glyph;
        }
        return nullptr;
    }

    Glyph* Font::loadGlyph(int codepoint) const {
        const stbtt_fontinfo* info = &typeface->getFontInfo();
        int id = stbtt_FindGlyphIndex(info, codepoint);

        if (id != 0) {
            int advance;
            int bearing;
            int width;
            int height;
            Rect bounds;
            uint8_t* bitmap;

            stbtt_GetGlyphHMetrics(info, id, &advance, &bearing);
            stbtt_GetGlyphBitmapBox(info, id, scale, scale, &bounds.left, &bounds.top, &bounds.right, &bounds.bottom);
            bitmap = stbtt_GetGlyphBitmap(info, scale, scale, id, &width, &height, nullptr, nullptr);
            return new Glyph(id, scale * (float)advance, scale * (float)bearing, width, height, bounds, bitmap);
        }
        return nullptr;
    }

    float Font::getAdvance(const Glyph* g1, const Glyph* g2) const {
        if (g1 == nullptr || g2 == nullptr) {
            return 0;
        }
        return (float)stbtt_GetGlyphKernAdvance(&typeface->getFontInfo(), g1->id, g2->id) * scale;

    }

    int Font::charWidth(char c) const {
        const Glyph* glyph = this->getGlyph(c);
        return glyph == nullptr ? 0 : (int)std::ceil(glyph->advance);
    }


    int Font::charsWidth(const char* str, int len) const {
        float width = 0;
        int codepoint = -1;
        int invalid_width = 0;
        const Glyph* first = nullptr;
        const Glyph* next = nullptr;
        // const Glyph* invalid = this->getGlyph(65533); // �;

        UTFIterator iterator(str, len);


        // if(invalid == nullptr) {
        if ((codepoint = iterator.next()) != -1) {
            next = this->getGlyph(codepoint);
            if (next != nullptr) {
                width += next->advance;
            }
            while ((codepoint = iterator.next()) != -1) {
                first = next;
                next = this->getGlyph(codepoint);

                if (next != nullptr) {
                    width += this->getAdvance(first, next);
                    width += next->advance;
                }
            }
        };
        // } else {
        //     if((codepoint = iterator.next()) != -1) {
        //         next = this->getGlyph(codepoint);
        //         if(next == nullptr) {
        //             next = invalid;
        //         }
        //         width += next->advance;
        //
        //         while ((codepoint = iterator.next()) != -1) {
        //             first = next;
        //             next = this->getGlyph(codepoint);
        //             if(next == nullptr) {
        //                 next = invalid;
        //             }
        //             width += this->getAdvance(first, next);
        //             width += next->advance;
        //         }
        //     };
        // }

        return (int)std::ceil(width);
    }

    int Font::stringWidth(const std::string& str) const {
        return this->charsWidth(str.c_str(), (int)str.length());
    }


    const FontMetrics& Font::getFontMetrics() const {
        return metrics;
    }

    std::vector<const Glyph*> Font::findGlyphs(const char *str, size_t len) const {
        std::vector<const Glyph*> glyphs;
        glyphs.reserve(len * 2);
        int codepoint;

        UTFIterator iterator(str, len);
        while ((codepoint = iterator.next()) != -1) {
            glyphs.push_back(getGlyph(codepoint));
        }
        return glyphs;
    }
}
