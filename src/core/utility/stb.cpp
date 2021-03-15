#include "core/assert.h"

/**
 * \file stb.cpp
 * \brief Implementation file for the stb library (think stb_image et al).
 */

#define STB_IMAGE_IMPLEMENTATION

// Use RK_ASSERT
#define STBI_ASSERT(x) RK_ASSERT(x)

// Supported image formats
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG

// // User friendly failure messages
// #define STBI_FAILURE_USERMSG

#include "core/utility/stb_image.h"
