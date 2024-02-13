
#include "Typeface.h"

#include <fstream>
#include <vector>




std::shared_ptr<Typeface> Typeface::makeFormFile(const char *fileName) {
    std::ifstream file(fileName, std::ios::binary);
    if(file.is_open()) {
        file.seekg(0, std::ios_base::end);
        std::streampos len = file.tellg();
        file.seekg(0, std::ios_base::beg);

        char* data = new char[len];
        file.read(data, len);


        stbtt_fontinfo info;
        if (stbtt_InitFont(&info, (unsigned char *) data, 0)) {
            return std::make_shared<Typeface>(info);
        } else {
            delete[] data;
        }
    }
    return nullptr;
}

Typeface::Typeface(const stbtt_fontinfo &info): fontInfo(info) {
}

Typeface::~Typeface() {
    delete[] fontInfo.data;
}


const stbtt_fontinfo& Typeface::getFontInfo() {
    return fontInfo;
}
