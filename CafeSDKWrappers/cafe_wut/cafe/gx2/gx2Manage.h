#pragma once

#include <gx2/displaylist.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

static inline void GX2BeginDisplayList(void* pDisplayList, u32 byteSize)
{
    GX2BeginDisplayListEx(pDisplayList, byteSize, GX2_ENABLE);
}

#ifdef __cplusplus
}
#endif // __cplusplus
