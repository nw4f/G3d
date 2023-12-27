#pragma once

#define CACHE_BLOCK_SIZE     32
#define L1_CACHE_LINE_SIZE   32
#define L2_CACHE_LINE_SIZE   64
#define LL_CACHE_FETCH_SIZE  64

#define PPC_IO_BUFFER_SIZE_MIN LL_CACHE_FETCH_SIZE
#define PPC_IO_BUFFER_ALIGN    LL_CACHE_FETCH_SIZE

#ifndef _ASSEMBLER

#include <types.h>
#include <coreinit/core.h>

#endif // _ASSEMBLER
