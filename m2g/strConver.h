#pragma once

#include <string>
#include <iostream>
#include <locale>
#include <codecvt>


extern int utf_8_char_at(const char* str, size_t str_len, size_t index, uint32_t* len);

//extern int utf8_to_unicode(const char* str, wchar_t * codepoint);

extern int utf8_to_codepoint(const char* input, int* len, size_t size);


