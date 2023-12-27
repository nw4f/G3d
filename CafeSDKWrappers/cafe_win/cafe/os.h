#pragma once

#include <misc/rio_Types.h>

#define OSReport RIO_LOG

#ifdef __cplusplus
extern "C"
{
#endif

inline u32 OSGetCoreId()
{
    return 1;
}

#ifdef __cplusplus
}
#endif
