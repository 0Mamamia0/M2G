
#include <cstdint>
#include <cstddef>
#include "utf.h"


namespace m2g::utf8 {
    int decode(int *codepoint, const uint8_t* str, size_t len) {
        if(len <= 0) {
            return 0;
        }
        uint8_t c1 = str[0];
        if ((c1 & 0x80) == 0x00) {
            *codepoint = c1;
            return 1;
        } else if ((c1 & 0xE0) == 0xC0 && len >= 2) {
            *codepoint = (c1 & 0x1F) << 6 | (str[1] & 0x3F);
            return 2;
        } else if ((c1 & 0xF0) == 0xE0 && len >= 3) {
            *codepoint = (c1 & 0x0F) << 12 | (str[1] & 0x3F) << 6 | (str[2] & 0x3F);
            return 3;
        } else if ((c1 & 0xF8) == 0xF0 && len >= 4) {
            *codepoint = (c1 & 0x07) << 18 | (str[1] & 0x3F) << 12 | (str[2] & 0x3F) << 6 | (str[3] & 0x3F);
            return 4;
        } else {
            return -1;
        }
    }


}

namespace m2g::utf16 {
    int decode(int *codepoint, const uint16_t *str, size_t len) {
        if (len <= 0) {
            return 0;
        }
        uint16_t c = str[0];
        if ((0xD800 <= c && c <= 0xDFFF)) {
            if (c > 0xDBFF) {
                return -1;
            }
            if (len < 2) {
                return -1;
            }

            uint16_t c2 = str[1];
            if (0xDC00 <= c2 && c2 <= 0xDFFF) {
                *codepoint = (c << 10) + c2 - 0x35FDC00;
                return 2;
            }
        }
        *codepoint = (int) c;
        return 1;
    }

}
