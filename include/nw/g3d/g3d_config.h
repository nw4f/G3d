#ifndef NW_G3D_CONFIG_H_
#define NW_G3D_CONFIG_H_

#if !defined(NW_DEBUG) && !defined(NW_DEVELOP) && !defined(NW_RELEASE)
#error "Build target is not defined."
#endif

#include <cstddef>
#include <cstdlib>
#include <new>

#define NW_G3D_CONFIG_USE_STATIC 1

#define NW_G3D_PERF_ENABLE 1
#define NW_G3D_PERF_LEVEL 0

#define NW_G3D_FORCE_PPC_SYNC 0

#define NW_G3D_MAX_SHADER_KEY 32

#include <nw/g3d/g3d_defs.h>

#include <sdk_ver.h>
#include <cafe/gx2.h>

#include <nw/g3d/g3d_assert.h>

#endif // NW_G3D_CONFIG_H_
