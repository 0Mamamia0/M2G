//
// Created by Admin on 2023/10/21.
//

#include <string>
#include <iostream>
#include <locale>
#include <codecvt>
#include <cstdint>

extern std::string UnicodeToUTF8(const std::wstring & wstr)
{
    std::string ret;
    try {
        std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
        ret = wcv.to_bytes(wstr);
    } catch (const std::exception & e) {
        std::cerr << e.what() << std::endl;
    }
    return ret;
}

extern std::wstring UTF8ToUnicode(const std::string & str)
{
    std::wstring ret;
    try {
        std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
        ret = wcv.from_bytes(str);
    } catch (const std::exception & e) {
        std::cerr << e.what() << std::endl;
    }
    return ret;
}


extern int utf8_to_unicode(const char* str, wchar_t * codepoint) {
    wchar_t ch = 0;
    int bytes = 0;
    int i = 0;

    if ((str[i] & 0x80) == 0) {
        // 1-byte sequence: 0xxxxxxx
        ch = str[i++] & 0x7F;
        bytes = 1;
    } else if ((str[i] & 0xE0) == 0xC0) {
        // 2-byte sequence: 110xxxxx 10xxxxxx
        ch = (str[i++] & 0x1F) << 6;
        ch |= str[i++] & 0x3F;
        bytes = 2;
    } else if ((str[i] & 0xF0) == 0xE0) {
        // 3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxx
        ch = (str[i++] & 0x0F) << 12;
        ch |= (str[i++] & 0x3F) << 6;
        ch |= str[i++] & 0x3F;
        bytes = 3;
    } else if ((str[i] & 0xF8) == 0xF0) {
        // 4-byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        ch = (str[i++] & 0x07) << 18;
        ch |= (str[i++] & 0x3F) << 12;
        ch |= (str[i++] & 0x3F) << 6;
        ch |= str[i++] & 0x3F;
        bytes = 4;
    } else {
        // Invalid UTF-8 sequence
        return -1;
    }
    *codepoint = ch;
    return bytes;
}



int utf_8_char_at(const char* text, size_t str_len, size_t index, uint32_t* len) {
    if(index >= str_len) {
        return -1;
    }
    const char* c_str = text + index;
    unsigned char c = c_str[0];

    size_t remaining_len = str_len - index;

    if ((c & 0x80) == 0x00) {
        // 1-byte sequence: 0xxxxxxx
        *len = 1;
        return static_cast<int>(c);
    } else if ((c & 0xE0) == 0xC0 && remaining_len >= 2) {
        // 2-byte sequence: 110xxxxx 10xxxxxx
        int codepoint = (c & 0x1F) << 6;
        codepoint |= c_str[1] & 0x3F;
        *len = 2;
        return codepoint;
    } else if ((c & 0xF0) == 0xE0 && remaining_len >= 3) {
        // 3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxx
        int codepoint = (c & 0x0F) << 12;
        codepoint |= (c_str[1] & 0x3F) << 6;
        codepoint |= c_str[2] & 0x3F;
        *len = 3;
        return codepoint;
    } else if ((c & 0xF8) == 0xF0 && remaining_len >= 4) {
        // 4-byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        int codepoint = (c & 0x07) << 18;
        codepoint |= (c_str[1] & 0x3F) << 12;
        codepoint |= (c_str[2] & 0x3F) << 6;
        codepoint |= c_str[3] & 0x3F;
        *len = 4;
        return codepoint;
    } else {
        // Invalid UTF-8 sequence
        return -1;
    }


}

extern int utf8_to_codepoint(const char* input, int* len, size_t size) {
    if (size == 0) {
        return -1;
    }
    unsigned char ch = input[0];
    if ((ch & 0x80) == 0x00) {
        // 1-byte sequence: 0xxxxxxx
        *len = 1;
        return static_cast<int>(ch);
    } else if ((ch & 0xE0) == 0xC0 && size >= 2) {
        // 2-byte sequence: 110xxxxx 10xxxxxx
        int codepoint = (ch & 0x1F) << 6;
        codepoint |= input[1] & 0x3F;
        *len = 2;
        return codepoint;
    } else if ((ch & 0xF0) == 0xE0 && size >= 3) {
        // 3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxx
        int codepoint = (ch & 0x0F) << 12;
        codepoint |= (input[1] & 0x3F) << 6;
        codepoint |= input[2] & 0x3F;
        *len = 3;
        return codepoint;
    } else if ((ch & 0xF8) == 0xF0 && size >= 4) {
        // 4-byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        int codepoint = (ch & 0x07) << 18;
        codepoint |= (input[1] & 0x3F) << 12;
        codepoint |= (input[2] & 0x3F) << 6;
        codepoint |= input[3] & 0x3F;
        *len = 4;
        return codepoint;
    } else {
        // Invalid UTF-8 sequence
        return -1;
    }
}