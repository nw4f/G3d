#include <nw/g3d/fnd/g3d_GfxManage.h>

#include <algorithm>
#include <nw/g3d/ut/g3d_Inlines.h>
#include <nw/g3d/fnd/g3d_GfxState.h>
#include <nw/g3d/fnd/g3d_GfxObject.h>

#include <cafe/os/OSTime.h>

namespace nw { namespace g3d { namespace fnd {

GPUClock::Tick GPUClock::s_Freq = OS_TIMER_CLOCK;

CPUClock::Freq CPUClock::s_Freq;

void CPUClock::Freq::Init()
{
    m_Value = OS_TIMER_CLOCK;
}

bool CPUCache::IsValid(const void* addr, size_t size)
{
    NW_G3D_ASSERT(size > 0);
    return FALSE != OSIsAddressRangeDCValid(addr, size);
}

void CPUCache::FillZero(void* addr, size_t size)
{
    NW_G3D_ASSERT_ADDR_ALIGNMENT(addr, CACHE_BLOCK_SIZE);
    NW_G3D_ASSERT_ALIGNMENT(size, CACHE_BLOCK_SIZE);
    NW_G3D_ASSERT(IsValid(addr, size));
    DCZeroRange(addr, size);
}

} } } // namespace nw::g3d::fnd
