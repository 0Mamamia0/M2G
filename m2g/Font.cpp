//
// Created by Admin on 2023/5/23.
//

#include "Font.h"
#include <iostream>
#include <fstream>
#include <vector>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "Typeface.h"
#include "Rect.h"
#include "strConver.h"


// Glyph EMPTY_GLYPH(0, 0, 0, 0, 0,  {0, 0, 0, 0}, nullptr);




Font::Font(Typeface* typeface, int face, int style, int size)
    : typeface(typeface)
    , face(face)
    , style(style)
    , size(size)
    , metrics{}
    {
        const stbtt_fontinfo& fontInfo = typeface->getFontInfo();
        int ascent, descent, lineGap;
        stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

        float pixel_height;
        switch (size) {
            case SIZE_SMALL: pixel_height = 12;
                break;
            case SIZE_MEDIUM: pixel_height = 18;
                break;
            case SIZE_LARGE: pixel_height = 24;
                break;
            default:
                pixel_height = 18;
                break;
        }
        this->scale =  stbtt_ScaleForPixelHeight(&fontInfo, pixel_height);
        metrics.ascent = (float)ascent * scale;
        metrics.descent = (float)descent * scale;
        metrics.lineGap = (float)lineGap * scale;
        metrics.baseline = metrics.ascent;
        metrics.height = metrics.ascent - metrics.descent + metrics.lineGap;


        const stbtt_fontinfo* info =  &typeface->getFontInfo();
        for (int i = 0; i < ascii_glyph_cache.size(); ++i) {
            auto* glyph = loadGlyph(i + 32);
            if(glyph) {
                ascii_glyph_cache[i] = glyph;
            } else {
                ascii_glyph_cache[i] = nullptr;
            }
        }
}


Font::~Font() {

    for(int i = 0; i < ascii_glyph_cache.size(); ++ i) {
        auto* glyph = ascii_glyph_cache[i];
        if(glyph != nullptr ) {
            if(glyph->bitmap != nullptr) {
                stbtt_FreeBitmap(glyph->bitmap, nullptr);
            }
            delete glyph;
            ascii_glyph_cache[i] = nullptr;
        }
    }



    for(const auto& pair : glyph_cache ) {
        const Glyph* glyph = pair.second;
        if(glyph != nullptr) {
            if(glyph->bitmap != nullptr) {
                stbtt_FreeBitmap(glyph->bitmap, nullptr);
            }
            delete glyph;
        }
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



int Font::getBaseLinePosition() const{
    return (int)std::round(metrics.baseline);
}

int Font::getHeight() const{
    return  (int)std::round(metrics.height);;
}

const Glyph* Font::getGlyph(char c) const {
    return this->getGlyph((int)c);
}


const Glyph* Font::getGlyph(int codepoint) const{
    if( codepoint >= 32 && codepoint <= 127) {
        return ascii_glyph_cache[codepoint - 32];
    }
    auto find = glyph_cache.find(codepoint);
    if(find != glyph_cache.end()) {
        return find->second;
    }
    auto* glyph = loadGlyph(codepoint);
    if(glyph) {
        auto result = glyph_cache.insert(std::make_pair(codepoint, glyph));
        if(result.second) {
            return result.first->second;
        }
    }
    return nullptr;
}

Glyph* Font::loadGlyph(int codepoint) const {
    const stbtt_fontinfo* info =  &typeface->getFontInfo();
    int id = stbtt_FindGlyphIndex(info, codepoint);

    if(id != 0) {
        int advance;
        int bearing;
        int width;
        int height;
        Rect bounds;
        uint8_t *bitmap;

        stbtt_GetGlyphHMetrics(info, id, &advance, &bearing);
        stbtt_GetGlyphBitmapBox(info, id, scale, scale, &bounds.left, &bounds.top, &bounds.right, &bounds.bottom);
        bitmap = stbtt_GetGlyphBitmap(info, scale, scale, id, &width, &height, nullptr, nullptr);
        return new Glyph(id, scale * (float)advance, scale * (float)bearing, width, height, bounds, bitmap);
    }
    return nullptr;
}

float Font::getAdvance(const Glyph *g1, const Glyph *g2) const {
    if(g1 == nullptr || g2 == nullptr) {
        return 0;
    }
    return  scale * (float)stbtt__GetGlyphKernInfoAdvance(&typeface->getFontInfo(), g1->id, g2->id);
}

int Font::charWidth(char c) const {
    int advanceWidth;
    int leftSideBearing;
    stbtt_GetCodepointHMetrics(&typeface->getFontInfo(), c, &advanceWidth, &leftSideBearing);
    return (int)std::round( (float)advanceWidth * scale);;
}


int Font::charsWidth(const char *str, int len) const {

    const char *tmp = str;
    int codepoint =0;
    int width = 0;
    std::vector<const Glyph*> glyphs;

    while (tmp[0] != '\0') {
        codepoint = utf8_to_codepoint( tmp, &len, strlen(tmp));
        glyphs.push_back(this->getGlyph(codepoint));
        tmp += len;
    }

    for (int i = 0; i < glyphs.size(); i ++) {
        const Glyph* glyph = glyphs[i];
        const Glyph* next;

        if(i < glyphs.size() - 1) {
            next = glyphs[i + 1];
        } else {
            next = nullptr;
        }

        width += glyph->advance;
        if(next) {
            width += this->getAdvance(glyph, next);
        }
    }


    return width;
}

int Font::stringWidth(const std::string &str) const {
    return this->charsWidth(str.c_str(), str.length());
}

const Font &Font::getDefaultFont(){
    static std::shared_ptr<Typeface> typeface = Typeface::makeFormFile("/simkai.ttf");
    static Font font(typeface.get(), FACE_SYSTEM, STYLE_BOLD, SIZE_MEDIUM);
    return font;
}

const FontMetrics &Font::getFontMetrics() const {
    return metrics;
}




