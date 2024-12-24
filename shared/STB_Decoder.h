#pragma once


#include <cstdint>
#include <memory>
#include "PixelBuffer.h"

class PixelBuffer;

namespace m2g {
    class STB_Decoder {
    public:
        static std::shared_ptr<PixelBuffer> decode(const char* filename);
        static std::shared_ptr<PixelBuffer> decode(uint8_t* data, int length);
    };
}

