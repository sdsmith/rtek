#include "core/assert.h"

#define STB_IMAGE_IMPLEMENTATION

// Use RK_ASSERT
#define STBI_ASSERT(x) RK_ASSERT(x)

// Supported image formats
#define STBI_ONLY_PNG

// Better failure messages
#define STBI_FAILURE_USERMSG

#include <stb/stb_image.h>
