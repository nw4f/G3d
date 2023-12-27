#pragma once

#include <gx2/texture.h>

#include <cafe/gx2/gx2Enum.h>
#include <cafe/gx2/gx2Misc.h>
#include <cafe/gx2/gx2Surface.h>

#define compSel compMap

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

static inline void GX2InitTexture(
    GX2Texture* texture,
    u32 width,
    u32 height,
    u32 depth,
    u32 numMips,
    GX2SurfaceFormat format,
    GX2SurfaceDim dim
)
{
    const GX2CompSel dstSel[54] = {
        0x04040405, 0x00040405, 0x00010405, 0x04040405,
        0x04040405, 0x00040405, 0x00040405, 0x00010405,
        0x00010205, 0x00010205, 0x00010203, 0x00010203,
        0x03020100, 0x00040405, 0x00040405, 0x00010405,
        0x00010405, 0x04040405, 0x04040405, 0x04040405,
        0x04040405, 0x04040405, 0x00010205, 0x04040405,
        0x04040405, 0x00010203, 0x00010203, 0x03020100,
        0x00010405, 0x00010405, 0x00010405, 0x00010203,
        0x00010203, 0x04040405, 0x00010203, 0x00010203,
        0x04040405, 0x04040405, 0x04040405, 0x00010205,
        0x00010205, 0x00040405, 0x00010405, 0x00010205,
        0x04040405, 0x04040405, 0x04040405, 0x00010205,
        0x00010205, 0x00010203, 0x00010203, 0x00010203,
        0x00040405, 0x00010405
    };

    texture->surface.dim     = dim;
    texture->surface.width   = width;
    texture->surface.height  = height;
    texture->surface.depth   = depth;
    texture->surface.numMips = numMips;
    texture->surface.format  = format;

    texture->surface.aa         = GX2_AA_MODE_1X;
    texture->surface.use        = GX2_SURFACE_USE_TEXTURE;
    texture->surface.tileMode   = GX2_TILE_MODE_DEFAULT;
    texture->surface.swizzle    = 0;

    texture->viewFirstMip   = 0;
    texture->viewNumMips    = numMips;
    texture->viewFirstSlice = 0;
    texture->viewNumSlices  = depth;

    texture->compSel = dstSel[(format) & 0x3f];

    GX2CalcSurfaceSizeAndAlignment(&texture->surface);

    GX2InitTextureRegs(texture);
}

static inline void GX2InitTexturePtrs(GX2Texture *texture, void *imagePtr, void *mipPtr)
{
    texture->surface.imagePtr = imagePtr;
    if (texture->surface.numMips > 1) {
        if (mipPtr) {
            texture->surface.mipPtr = mipPtr;
        } else {
            texture->surface.mipPtr = (void *) ( (u32) imagePtr + texture->surface.mipOffset[0]);
        }
    } else {
        texture->surface.mipPtr = mipPtr;
    }
}

static inline void GX2InitTextureCompSel(GX2Texture *texture, GX2CompSel compSel)
{
    texture->compSel = compSel;
}

#define samplerReg regs

#ifdef __cplusplus
}
#endif // __cplusplus
