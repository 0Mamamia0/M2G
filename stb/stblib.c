//
// Created by Admin on 2024/2/13.
//


#ifdef M2G_USE_STB_IMAGE_IMPL

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#ifdef __ARM_NEON
#define STBI_NEON
#endif

#include "stb_image.h"
#include "stb_image_write.h"
#endif

#ifdef M2G_USE_STB_TRUETYPE_IMPL

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#endif


