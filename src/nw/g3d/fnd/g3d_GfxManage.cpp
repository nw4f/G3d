#include <nw/g3d/fnd/g3d_GfxManage.h>

#include <nw/g3d/ut/g3d_Inlines.h>
#include <nw/g3d/fnd/g3d_GfxState.h>
#include <nw/g3d/fnd/g3d_GfxObject.h>

#if NW_G3D_IS_HOST_CAFE
    #include <cafe/os/OSCache.h>
#endif // NW_G3D_IS_HOST_CAFE

namespace nw { namespace g3d { namespace fnd {

bool CPUCache::IsValid(const void* addr, size_t size)
{
    NW_G3D_ASSERT(size > 0);
#if NW_G3D_IS_HOST_CAFE
    return FALSE != OSIsAddressRangeDCValid(addr, size);
#else
    NW_G3D_UNUSED(addr);
    NW_G3D_UNUSED(size);
    return true;
#endif
}

void CPUCache::FillZero(void* addr, size_t size)
{
#if NW_G3D_IS_HOST_CAFE
    NW_G3D_ASSERT_ADDR_ALIGNMENT(addr, CACHE_BLOCK_SIZE);
#endif // NW_G3D_IS_HOST_CAFE
    NW_G3D_ASSERT_ALIGNMENT(size, CACHE_BLOCK_SIZE);
    NW_G3D_ASSERT(IsValid(addr, size));
#if NW_G3D_IS_HOST_CAFE
    DCZeroRange(addr, size);
#else
    memset(addr, 0, size);
#endif
}

} } } // namespace nw::g3d::fnd
