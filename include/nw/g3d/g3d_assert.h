#ifndef NW_G3D_ASSERT_H_
#define NW_G3D_ASSERT_H_

#include <cafe/os.h>

#ifndef NW_G3D_ASSERTMSG
#if NW_G3D_IS_DEBUG || NW_G3D_IS_DEVELOP
#define NW_G3D_ASSERTMSG(exp, ...)                                                                 \
    (void) ((exp) || (OSReport(__VA_ARGS__),                                            \
        OSReport("\t%s(%d)\n", NW_G3D_CODE_POSITION_NAME, __LINE__),                    \
        NW_G3D_DEBUG_BREAK(), ::exit(EXIT_FAILURE), 0))
#else
#define NW_G3D_ASSERTMSG(exp, ...) ((void)0)
#endif
#endif

#define NW_G3D_ASSERT(exp) \
    NW_G3D_ASSERTMSG((exp), "NW: Failed assertion. " #exp "\n")

#define NW_G3D_ASSERT_NOT_NULL(exp) \
    NW_G3D_ASSERTMSG((exp) != NULL, "NW: Pointer must not be NULL. ("#exp")\n")

#define NW_G3D_ASSERT_INDEX_BOUNDS(index, size)                                                    \
    NW_G3D_ASSERTMSG(static_cast<uint>(index) < static_cast<uint>(size),                           \
        "NW: " #index " is out of bounds. (%d)\n"                                                  \
        "0 <= " #index " < %d is not satisfied.\n",                                                \
        static_cast<int>(index), static_cast<int>(size))

#define NW_G3D_ASSERT_ALIGNMENT(value, alignment)                                                  \
    NW_G3D_ASSERTMSG(static_cast<uint>((value) & (alignment - 1)) == 0,                            \
        "NW: Failed alignment. \n"                                                                 \
        "value: 0x%x\n"                                                                            \
        "alignment: %d\n",                                                                         \
        value, static_cast<int>(alignment))

#define NW_G3D_ASSERT_ADDR_ALIGNMENT(addr, alignment)                                              \
    NW_G3D_ASSERTMSG((reinterpret_cast<uintptr_t>(addr) & (alignment - 1)) == 0,                   \
        "NW: Failed alignment. \n"                                                                 \
        "ptr: 0x%x\n"                                                                              \
        "alignment: %d\n",                                                                         \
        reinterpret_cast<uintptr_t>(addr), static_cast<int>(alignment))

#define NW_G3D_NOT_IMPLEMENTED() \
    NW_G3D_ASSERTMSG(0, "NW: Not implemented yet.\n")

#define NW_G3D_NOT_SUPPORTED() \
    NW_G3D_ASSERTMSG(0, "NW: Not supported.\n")

#if defined(_DEBUG) || defined(NW_DEBUG) || defined(NW_DEVELOP)
#define NW_G3D_WARNING(exp, ...)                                                                   \
    (void) ((exp) || (OSReport("NW: "), OSReport(__VA_ARGS__),               \
    OSReport("\t%s(%d)\n", NW_G3D_CODE_POSITION_NAME, __LINE__), 0))
#else
#define NW_G3D_WARNING(exp, ... ) ((void)0)
#endif

namespace nw { namespace g3d {

void DebugPrint(const char* format, ...);

} } // namespace nw::g3d

#define NW_G3D_DEBUG_BREAK()

#endif // NW_G3D_ASSERT_H_
