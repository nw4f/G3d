#pragma once

// Function prototypes mismatch in:
//#include <coreinit/cache.h>

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void DCInvalidateRange(void* addr, u32 nBytes);
void DCFlushRange(const void* addr, u32 nBytes);
void DCStoreRange(const void* addr, u32 nBytes);
void DCFlushRangeNoSync(const void* addr, u32 nBytes);
void DCStoreRangeNoSync(const void* addr, u32 nBytes);
void DCZeroRange(void* addr, u32 nBytes);
void DCTouchRange(const void* addr, u32 nBytes);

#define LC_MAX_DMA_BLOCKS   (128)
#define LC_MAX_DMA_BYTES    (LC_MAX_DMA_BLOCKS * 32)

#define LC_MAX_BYTES        0x4000
#define LC_MIN_ALLOC        512

u32 LCGetMaxSize(void);
u32 LCGetUnallocated(void);
u32 LCGetAllocatableSize(void);
void* LCAlloc(u32 nBytes);
void LCDealloc(void *lcAddr);
BOOL LCEnableDMA(void);
void LCDisableDMA(void);
BOOL LCIsDMAEnabled(void);
void LCLoadDMABlocks(void* lcDestAddr,  const void* memSrcAddr, u32 numBlocks);
void LCStoreDMABlocks(void* memDestAddr, const void* lcSrcAddr, u32 numBlocks);
u32 LCGetDMAQueueLength (void);
void LCWaitDMAQueue(u32 len);

BOOL OSIsAddressRangeDCValid(const void *addr, u32 nBytes);

BOOL DCCoreFlushAll(void *flush_buf, u32 size);

#ifdef __cplusplus
}
#endif
