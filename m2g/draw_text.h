//
// Created by Admin on 2023/10/30.
//

#ifndef DRAW_TEXT_H__
#define DRAW_TEXT_H__



#include <string>
#include <vector>
#include "Rect.h"
#include "Font.h"
#include "UTF8Iterator.h"
#include "draw_image.h"

void draw_glyph(uint8_t* dst, ptrdiff_t dst_stride, int dst_format, const Glyph& glyph, int x, int y, const Font& font, int color) {
    if(glyph.bitmap) {
        x += glyph.bounds.left;
        y += glyph.bounds.top;
        drawPixelsA8_ARGB(dst, glyph.bitmap, dst_stride, glyph.width, x, y, glyph.width, glyph.height, dst_format, color);
    }
}


void draw_glyph(uint8_t* dst, ptrdiff_t dst_stride, int dst_format, const Glyph& glyph, int x, int y, const Rect& clip, const Font& font, int color) {
    if(glyph.bitmap) {
        x += glyph.bounds.left;
        y += glyph.bounds.top;
        drawPixelsA8_ARGB(dst, glyph.bitmap, dst_stride, glyph.width, x, y, glyph.width, glyph.height, dst_format, color, clip);
    }
}


void draw_char(uint8_t* dst, ptrdiff_t dst_stride, int dst_format, char c, int x, int y, const Font& font, int color) {
    draw_glyph(dst, dst_stride, dst_format, font.getGlyph(c), x, y, font, color);
}


void draw_char(uint8_t* dst, ptrdiff_t dst_stride, int dst_format, char c, int x, int y, const Rect& clip, const Font& font, int color) {
    draw_glyph(dst, dst_stride, dst_format, font.getGlyph(c), x, y, clip, font, color);
}



void draw_glyphs(uint8_t* dst, ptrdiff_t dst_stride, int dst_format, const std::vector<const Glyph*>& glyphs, int x, int y, const Rect& clip, const Font& font, int color) {

    if(clip.isEmpty()) {
        return;
    }
    const auto& metrics = font.getFontMetrics();

    int x_offset = x;
    int y_offset = y;
    int top = y_offset - (int) ceil(metrics.ascent);
    int bottom = y_offset - (int) floor(metrics.descent);

    if(top > bottom || top > clip.bottom || bottom < clip.top) {
        return;
    }

    //vertical overlap
    if(top > clip.top && bottom < clip.bottom) {
        auto iter = glyphs.begin();

        //clipped
        while (iter!= glyphs.end()) {
            const auto& current = *iter;
            if (current->empty()) {
                ++iter;
                continue;
            }
            if (x_offset + current->advance > clip.left) {
                break;
            }
            x_offset += current->advance;
            if (auto iter_next = std::next(iter); iter_next!= glyphs.end()) {
                x_offset += font.getAdvance(current, *iter_next);
            }
            ++iter;
        }

        if (iter != glyphs.end()) {
            const auto& current = *iter;
            draw_glyph(dst, dst_stride, dst_format, *current, x_offset, y_offset, clip, font, color);
            x_offset += current->advance;
            if (auto iter_next = std::next(iter); iter_next!= glyphs.end()) {
                x_offset += font.getAdvance(current, *iter_next);
            }
            ++iter;
        }


        while (iter!= glyphs.end()) {
            const auto& current = *iter;
            if (current->empty()) {
                ++iter;
                continue;
            }
            if (x_offset + current->advance > clip.right) {
                break;
            }
            draw_glyph( dst, dst_stride, dst_format, *current, x_offset, y_offset, font, color);
            x_offset += current->advance;
            if (auto iter_next = std::next(iter); iter_next!= glyphs.end()) {
                x_offset += font.getAdvance(current, *iter_next);
            }
            ++iter;
        }

        if (iter != glyphs.end()) {
            const auto& current = *iter;
            draw_glyph(dst, dst_stride, dst_format, *current, x_offset, y_offset, clip, font, color);
        }


    } else {
        for (auto iter = glyphs.begin(); iter != glyphs.end(); ++iter) {
            const auto *current = *iter;

            if (current->empty()) {
                continue;
            }

            draw_glyph(dst, dst_stride, dst_format, *current, x_offset, y_offset, clip, font, color);

            x_offset += current->advance;
            if (auto iter_next = std::next(iter); iter_next != glyphs.end()) {
                x_offset += font.getAdvance(current, *iter_next);
            }
        }
    }

}

void draw_string(uint8_t* dst, ptrdiff_t dst_stride, int dst_format, const char* str, size_t len, int x, int y, const Rect& clip, const Font& font, int color) {

    if(str == nullptr || len == 0) {
        return;
    }
    std::vector<const Glyph*> glyphs;

    int codepoint;
    UTFIterator iterator(str, len);
    while ((codepoint = iterator.next()) != -1) {
        glyphs.push_back(&font.getGlyph(codepoint));
    }

    if(glyphs.size() == 0) {
        return;
    }

    draw_glyphs(dst, dst_stride, dst_format, glyphs, x, y, clip, font, color);
}









#endif //DRAW_TEXT_H__
