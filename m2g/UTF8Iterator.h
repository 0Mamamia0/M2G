//
// Created by Admin on 2023/10/30.
//

#ifndef UTF8ITERATOR_H__
#define UTF8ITERATOR_H__


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


#endif //UTF8ITERATOR_H__
