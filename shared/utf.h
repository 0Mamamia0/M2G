#pragma once

#include <string>
#include <iostream>
#include <locale>
#include <codecvt>

namespace m2g::utf8 {
    int decode(int *codepoint, const uint8_t* str, size_t len);
}

namespace m2g::utf16 {
    int decode(int *codepoint, const uint16_t* str, size_t len);
}


namespace m2g::utf {
    template<typename data_type, int (*decoder)(int*, const data_type*, size_t) >
    class iterator {
    public:
        iterator(const data_type *s, size_t l, size_t p = 0)
            : str(s), str_len(l), pos(p), decoded(false), codepoint(0), char_len(0){
        }
        bool is_valid() {
            ensure_decoded();
            return char_len > 0;
        }

        int operator*() {
            ensure_decoded();
            return codepoint;
        }

        bool operator==(const iterator &rhs) const {
            return pos == rhs.pos;
        }

        bool operator!=(const iterator &rhs) const {
            return pos != rhs.pos;
        }

        iterator &operator++() {
            ensure_decoded();
            if (char_len <= 0) {
                pos = str_len;
                return *this;
            }
            pos += char_len;
            decoded = false;
            return *this;
        }

    private:
        void ensure_decoded() {
            if (decoded)
                return;
            char_len = decoder(&codepoint, &str[pos], str_len - pos);
            decoded = true;
        }


    private:
        const data_type *str;
        size_t str_len;
        size_t pos;
        bool decoded;
        int codepoint;
        int char_len;
    };


    template<typename data_type>
    struct iterator_selector {
        using type = void;
    };

    template<>
    struct iterator_selector<uint8_t> {
        using type = iterator<uint8_t , utf8::decode>;
    };

    template<>
    struct iterator_selector<uint16_t> {
        using type = iterator<uint16_t, utf16::decode>;
    };



    template<typename data_type>
    class range {
        using iterator_type = iterator_selector<data_type>::type;
    public:
        range(data_type* s, size_t l)
            : str(s), len(l) {
        }

        iterator_type begin() const {
            return iterator_type(str, len);
        }

        iterator_type end() const {
            return iterator_type(str, len, len);
        }

    private:
        data_type *str;
        size_t len;
    };
}




