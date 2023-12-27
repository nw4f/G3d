#pragma once

#include <coreinit/debug.h>
// #define OSReport RIO_LOG

#define OSHalt(msg)             OSPanic(__FILE__, __LINE__, msg)

#include <cafe/os/OSCache.h>
#include <cafe/os/OSCore.h>
#include <cafe/os/OSTime.h>
