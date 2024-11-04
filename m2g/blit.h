#pragma once

namespace m2g {

    inline uint32_t load_32(void* addr) {
        return *(uint32_t*)addr;
    }

    inline void store_32(void* addr, uint32_t data) {
        *(uint32_t*)addr = data;
    }

    inline void load_and_store_32(void* __restrict dst, void* __restrict src) {
        *(uint32_t*)dst = *(uint32_t*)src;
    }


    inline void blit(uint8_t* __restrict begin, int count, ptrdiff_t stride, uint8_t* __restrict rgba) {
        while (count -- > 0) {
            load_and_store_32(begin, rgba);
            begin += stride;
        }
    }

    inline void blitV(uint8_t* __restrict begin, int height, ptrdiff_t stride, uint8_t* __restrict rgba) {
        blit(begin, height, stride, rgba);
    }

    inline void blitH(uint8_t* __restrict begin, int width, uint8_t* __restrict rgba) {
        blit(begin, width, 4, rgba);
    }

    inline void blitLineV(uint8_t* __restrict begin, int count, ptrdiff_t stride, uint8_t* __restrict rgba) {
        blit(begin, std::abs(count) , count < 0 ?  -stride : stride, rgba);
    }

    inline void blitLineH(uint8_t* __restrict begin, int count, uint8_t* __restrict rgba) {
        blit(begin, std::abs(count) , count < 0 ? -4 : 4, rgba);
    }

    inline void blitRect(uint8_t* __restrict begin, int width, int height, ptrdiff_t stride, uint8_t* __restrict rgba) {
        while (height-- > 0) {
            blitH(begin, width, rgba);
            begin += stride;
        }
    }
}