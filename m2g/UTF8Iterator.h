#pragma once
#include <cstddef>
#include "strConver.h"

class UTFIterator {

public:

    UTFIterator(const char* str,  size_t len)
        :str(str), len(len), pos(0){
    }

    int next() {
        int codepoint;
        uint32_t count = 0;
        codepoint = utf_8_char_at(str, len, pos, &count);
        pos += count;
        return codepoint;
    }

private:
    const char* str;
    size_t len;
    size_t pos;
};



