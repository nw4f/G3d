#ifndef NW_G3D_FND_GFXCONTEXT_H_
#define NW_G3D_FND_GFXCONTEXT_H_

#include <nw/g3d/g3d_config.h>

#include <cafe/gx2/gx2Enum.h>

namespace nw { namespace g3d { namespace fnd {

class CPUCache
{
public:
    /*
    static void Flush(const void* addr, size_t size);
    static void Store(const void* addr, size_t size);
    static void Invalidate(void* addr, size_t size);
    static void Sync();
    */
    static bool IsValid(const void* addr, size_t size);
    static void FillZero(void* addr, size_t size);
};

} } } // namespace nw::g3d::fnd

#endif // NW_G3D_FND_GFXCONTEXT_H_
