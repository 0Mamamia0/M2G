
#include "Typeface.h"

#include <fstream>
#include <vector>

namespace m2g {
    Typeface* Typeface::makeFormFile(const char *fileName) {
        std::ifstream file(fileName, std::ios::binary);
        if(!file.is_open()) {
            return nullptr;
        }

        try {
            file.seekg(0, std::ios_base::end);
            std::streampos len = file.tellg();
            file.seekg(0, std::ios_base::beg);

            char* data = new char[len];
            file.read(data, len);

            if (static_cast<std::streamsize>(file.gcount()) != len) {
                return nullptr;
            }

            stbtt_fontinfo info;
            if (stbtt_InitFont(&info, (unsigned char *) data, 0)) {
                return new Typeface(info);
            } else {
                delete[] data;
            }

        } catch (const std::exception& e) {
            return nullptr;
        }


        return nullptr;
    }

    Typeface::Typeface(const stbtt_fontinfo &info)
            : fontInfo{info} {
    }

    Typeface::~Typeface() {
        delete[] fontInfo.data;
    }


    const stbtt_fontinfo& Typeface::getFontInfo() {
        return fontInfo;
    }

}
