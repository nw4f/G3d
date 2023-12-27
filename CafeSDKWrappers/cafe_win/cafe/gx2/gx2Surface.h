#pragma once

#include <ninTexUtils/gx2/gx2Surface.h>

#define GX2_NUM_TEXTURE_REGISTERS      5
#define GX2_NUM_COLOR_BUFFER_REGISTERS 5
#define GX2_NUM_DEPTH_BUFFER_REGISTERS 7

typedef struct _GX2ColorBuffer {

    GX2Surface surface;
    u32 viewMip;
    u32 viewFirstSlice;
    u32 viewNumSlices;
    void *auxPtr;
    u32  auxSize;
    u32 _regs[GX2_NUM_COLOR_BUFFER_REGISTERS];

} GX2ColorBuffer;

typedef struct _GX2DepthBuffer {

    GX2Surface surface;
    u32 viewMip;
    u32 viewFirstSlice;
    u32 viewNumSlices;
    void *hiZPtr;
    u32  hiZSize;
    f32 clearDepth;
    u32 clearStencil;
    u32 _regs[GX2_NUM_DEPTH_BUFFER_REGISTERS];

} GX2DepthBuffer;
