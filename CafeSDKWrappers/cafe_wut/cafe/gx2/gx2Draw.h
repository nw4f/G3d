#pragma once

#include <gx2/draw.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

static inline void GX2Draw(GX2PrimitiveMode mode, u32 count)
{
    GX2DrawEx(mode, count, 0, 1);
}

#ifdef __cplusplus
}
#endif // __cplusplus
