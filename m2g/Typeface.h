#pragma once
#include <memory>
#include "stb_truetype.h"


class Typeface {

public:
    explicit Typeface(const stbtt_fontinfo& info);
    Typeface(const Typeface& other) = delete;
    void operator=(const Typeface& other) = delete;
    ~Typeface();

    const stbtt_fontinfo& getFontInfo();
public:
    static std::shared_ptr<Typeface> makeFormFile(const char* fileName);


private:
    stbtt_fontinfo fontInfo;
};

