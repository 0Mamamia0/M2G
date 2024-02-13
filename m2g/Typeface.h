//
// Created by Admin on 2023/5/30.
//

#ifndef M2G_TYPEFACE_H
#define M2G_TYPEFACE_H

#include <memory>
#include "stb_truetype.h"


class Typeface {

public:
    Typeface(const stbtt_fontinfo& info);
    Typeface(const Typeface& other) = delete;
    void operator=(const Typeface& other) = delete;
    ~Typeface();

    const stbtt_fontinfo& getFontInfo();
public:
    static std::shared_ptr<Typeface> makeFormFile(const char* fileName);


private:
    // char* data;
    stbtt_fontinfo fontInfo;

};


#endif //M2G_TYPEFACE_H
