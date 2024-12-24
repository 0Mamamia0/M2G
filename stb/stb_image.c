#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#ifdef __ARM_NEON
#define STBI_NEON
#endif

#include "stb_image.h"
#include "stb_image_write.h"
