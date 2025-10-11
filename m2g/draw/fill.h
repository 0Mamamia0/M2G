#pragma once

template<typename T>
inline T load(const void* addr) {
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
    T data;
    std::memcpy(&data, addr, sizeof(T));
    return data;
}


template<typename T>
inline void store(void* addr, T data) {
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
    std::memcpy(addr, &data, sizeof(T));
}


template<typename T>
inline void load_and_store(void* __restrict dst, const void* __restrict src) {
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
    T data;
    std::memcpy(&data, src, sizeof(T));
    std::memcpy(dst, &data, sizeof(T));
}



inline void fill_n(uint8_t* __restrict begin, int count, uint32_t data) {
    while (count -- > 0) {
        store<uint32_t>(begin, data);
        begin += 4;
    }
}

inline void fill_n_stride(uint8_t* __restrict begin, int count, ptrdiff_t stride, uint32_t data) {
    while (count -- > 0) {
        store<uint32_t>(begin, data);
        begin += stride;
    }
}



template<bool flip>
inline void fill_h(uint8_t* __restrict begin, int count, uint8_t* __restrict rgba) {
    const auto data = load<uint32_t>(rgba);
    if constexpr (flip) {
        uint8_t* start = begin - 4 * (count - 1);
        fill_n(start, count, data);
    } else {
        fill_n(begin,  count, data);
    }
}

template<bool flip>
inline void fill_v(uint8_t* __restrict begin, int count, ptrdiff_t stride, uint8_t* __restrict rgba) {
    const auto data = load<uint32_t>(rgba);
    if constexpr (flip) {
        uint8_t* start = begin - (count - 1) * stride;
        fill_n_stride(start, count, stride, data);
    } else {
        fill_n_stride(begin, count, stride, data);
    }
}


inline void fill_rect(uint8_t* __restrict begin, int width, int height, ptrdiff_t stride, uint8_t* __restrict rgba) {
    const auto data = load<uint32_t>(rgba);
    if (stride == width * 4) {
        int total = width * height;
        fill_n(begin, total, data);
    } else {
        while (height-- > 0) {
            fill_n(begin, width, data);
            begin += stride;
        }
    }
}


