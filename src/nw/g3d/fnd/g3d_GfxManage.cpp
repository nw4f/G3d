#include <nw/g3d/fnd/g3d_GfxManage.h>

#include <algorithm>
#include <nw/g3d/ut/g3d_Inlines.h>
#include <nw/g3d/fnd/g3d_GfxState.h>
#include <nw/g3d/fnd/g3d_GfxObject.h>

#if NW_G3D_IS_HOST_WIN
    #define WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif // NOMINMAX
    #include <windows.h>
#elif NW_G3D_IS_HOST_CAFE
    #include <cafe/os/OSTime.h>
#endif

namespace nw { namespace g3d { namespace fnd {

#if NW_G3D_IS_GX2
GPUClock::Tick GPUClock::s_Freq = OS_TIMER_CLOCK;
#elif NW_G3D_IS_GL
GPUClock::Tick GPUClock::s_Freq = 1000 * 1000 * 1000;
#endif

CPUClock::Freq CPUClock::s_Freq;

void CPUClock::Freq::Init()
{
#if NW_G3D_IS_GX2
    m_Value = OS_TIMER_CLOCK;
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    [[maybe_unused]] BOOL success = QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_Value));
    NW_G3D_ASSERT(success);
#endif
}

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
