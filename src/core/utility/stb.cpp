#include "core/assert.h"

#define STB_IMAGE_IMPLEMENTATION

// Use RK_ASSERT
#define STBI_ASSERT(x) RK_ASSERT(x)

// Supported image formats
#define STBI_ONLY_PNG

#include <stb/stb_image.h>
