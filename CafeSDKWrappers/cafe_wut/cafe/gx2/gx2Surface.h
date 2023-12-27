#pragma once

#include <gx2/surface.h>

#define GX2_NUM_TEXTURE_REGISTERS      5
#define GX2_NUM_COLOR_BUFFER_REGISTERS 5
#define GX2_NUM_DEPTH_BUFFER_REGISTERS 7

#define numMips mipLevels
#define imagePtr image
#define mipPtr mipmaps
#define mipSize mipmapSize
#define mipOffset mipLevelOffset

#define auxPtr aaBuffer
#define auxSize aaSize

#define clearDepth depthClear
#define clearStencil stencilClear
