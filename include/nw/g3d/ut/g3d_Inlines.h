#ifndef NW_G3D_UT_INLINES_H_
#define NW_G3D_UT_INLINES_H_

#include <nw/g3d/g3d_config.h>

#include <string.h>

namespace nw { namespace g3d { namespace ut {

enum
{
    DEFAULT_ALIGNMENT = 4
};

NW_G3D_FORCE_INLINE
bool IsPowerOfTwo(size_t size)
{
    return ((size - 1) & size) == 0;
}
NW_G3D_FORCE_INLINE
void* AddOffset(void* ptr, size_t offset)
{
    return static_cast<char*>(ptr) + offset;
}

NW_G3D_FORCE_INLINE
const void* AddOffset(const void* ptr, size_t offset)
{
    return static_cast<const char*>(ptr) + offset;
}

template <typename ResultT>
NW_G3D_FORCE_INLINE
ResultT* AddOffset(void* ptr, size_t offset)
{
    return static_cast<ResultT*>(AddOffset(ptr, offset));
}

template <typename ResultT>
NW_G3D_FORCE_INLINE
const ResultT* AddOffset(const void* ptr, size_t offset)
{
    return static_cast<const ResultT*>(AddOffset(ptr, offset));
}
NW_G3D_FORCE_INLINE
size_t Align(size_t size, size_t alignment = DEFAULT_ALIGNMENT)
{
    NW_G3D_ASSERT(IsPowerOfTwo(alignment));
    size_t mask = alignment - 1;
    return (size + mask) & ~mask;
}

NW_G3D_FORCE_INLINE
void* Align(void* ptr, size_t alignment = DEFAULT_ALIGNMENT)
{
    return reinterpret_cast<void*>(Align(reinterpret_cast<size_t>(ptr), alignment));
}

NW_G3D_FORCE_INLINE
const void* Align(const void* ptr, size_t alignment = DEFAULT_ALIGNMENT)
{
    return reinterpret_cast<void*>(Align(reinterpret_cast<size_t>(ptr), alignment));
}

NW_G3D_FORCE_INLINE
bool IsAligned(size_t size, size_t alignment = DEFAULT_ALIGNMENT)
{
    NW_G3D_ASSERT(IsPowerOfTwo(alignment));
    return (size & (alignment - 1)) == 0;
}

NW_G3D_FORCE_INLINE
bool IsAligned(const void* ptr, size_t alignment = DEFAULT_ALIGNMENT)
{
    return IsAligned(reinterpret_cast<size_t>(ptr), alignment);
}
NW_G3D_FORCE_INLINE
u32 LoadRevU32(const u32* addr)
{
#if defined( __ghs__ )
    register u32 data;
    __lwbrx(addr, data);
    return data;
#else
    return ((*addr << 24) & 0xFF000000) | ((*addr << 8) & 0x00FF0000) |
        ((*addr >> 8) & 0x0000FF00) | ((*addr >> 24) & 0x000000FF);
#endif
}

NW_G3D_FORCE_INLINE
void StoreRevU32(u32* addr, u32 data)
{
#if defined( __ghs__ )
    __stwbrx(addr, data);
#else
    *addr = ((data << 24) & 0xFF000000) | ((data << 8) & 0x00FF0000) |
        ((data >> 8) & 0x0000FF00) | ((data >> 24) & 0x000000FF);
#endif
}

NW_G3D_FORCE_INLINE
u16 LoadRevU16(const u16* addr)
{
#if defined( __ghs__ )
    register u32 data;
    __lhbrx(addr, data);
    return data;
#else
    return ((*addr << 8) & 0xFF00) | ((*addr >> 8) & 0x00FF);
#endif
}

NW_G3D_FORCE_INLINE
void StoreRevU16(u16* addr, u16 data)
{
#if defined( __ghs__ )
    __sthbrx(addr, data);
#else
    *addr = ((data << 8) & 0xFF00) | ((data >> 8) & 0x00FF);
#endif
}

template <bool swap>
NW_G3D_FORCE_INLINE
void Copy32(void* pDst, const void* pSrc, int count)
{
#if NW_G3D_IS_HOST_CAFE
    if (swap)
    {
        bit32* dst = static_cast<bit32*>(pDst);
        const bit32* src = static_cast<const bit32*>(pSrc);
        for (const bit32* end = src + count; src != end; ++dst, ++src)
        {
            StoreRevU32(dst, *src);
        }
    }
    else
#endif
    {
        memcpy(pDst, pSrc, sizeof(bit32) * count);
    }
}

template <typename DstT, typename SrcT>
DstT FastCast(SrcT x);

template <>
NW_G3D_FORCE_INLINE
s16 FastCast<s16, float>(float x)
{
#if defined( __ghs__ )
    return __OSf32tos16Value(x);
#else
    return static_cast<s16>(x);
#endif
}

template <>
NW_G3D_FORCE_INLINE
u16 FastCast<u16, float>(float x)
{
#if defined( __ghs__ )
    return __OSf32tou16Value(x);
#else
    return static_cast<u16>(x);
#endif
}

template <>
NW_G3D_FORCE_INLINE
s8 FastCast<s8, float>(float x)
{
#if defined( __ghs__ )
    return __OSf32tos8Value(x);
#else
    return static_cast<s8>(x);
#endif
}

template <>
NW_G3D_FORCE_INLINE
u8 FastCast<u8, float>(float x)
{
#if defined( __ghs__ )
    return __OSf32tou8Value(x);
#else
    return static_cast<u8>(x);
#endif
}

template <>
NW_G3D_FORCE_INLINE
float FastCast<float, s16>(s16 x)
{
#if defined( __ghs__ )
    return OSs16tof32Value(&x);
#else
    return static_cast<float>(x);
#endif
}

template <>
NW_G3D_FORCE_INLINE
float FastCast<float, u16>(u16 x)
{
#if defined( __ghs__ )
    return OSu16tof32Value(&x);
#else
    return static_cast<float>(x);
#endif
}

template <>
NW_G3D_FORCE_INLINE
float FastCast<float, s8>(s8 x)
{
#if defined( __ghs__ )
    return OSs8tof32Value(&x);
#else
    return static_cast<float>(x);
#endif
}

template <>
NW_G3D_FORCE_INLINE
float FastCast<float, u8>(u8 x)
{
#if defined( __ghs__ )
    return OSu8tof32Value(&x);
#else
    return static_cast<float>(x);
#endif
}

template <typename DstT, typename SrcT>
NW_G3D_FORCE_INLINE
DstT StaticCast(SrcT x) { return static_cast<DstT>(x); }

template <>
NW_G3D_FORCE_INLINE
s16 StaticCast<s16, float>(float x) { return FastCast<s16, float>(x); }

template <>
NW_G3D_FORCE_INLINE
u16 StaticCast<u16, float>(float x) { return FastCast<u16, float>(x); }

template <>
NW_G3D_FORCE_INLINE
s8 StaticCast<s8, float>(float x) { return FastCast<s8, float>(x); }

template <>
NW_G3D_FORCE_INLINE
u8 StaticCast<u8, float>(float x) { return FastCast<u8, float>(x); }

template <>
NW_G3D_FORCE_INLINE
float StaticCast<float, s16>(s16 x) { return FastCast<float, s16>(x); }

template <>
NW_G3D_FORCE_INLINE
float StaticCast<float, u16>(u16 x) { return FastCast<float, u16>(x); }

template <>
NW_G3D_FORCE_INLINE
float StaticCast<float, s8>(s8 x) { return FastCast<float, s8>(x); }

template <>
NW_G3D_FORCE_INLINE
float StaticCast<float, u8>(u8 x) { return FastCast<float, u8>(x); }

NW_G3D_FORCE_INLINE
int CountOnes(bit32 x)
{
    x = (x & 0x55555555) + (x >>  1 & 0x55555555);
    x = (x & 0x33333333) + (x >>  2 & 0x33333333);
    x = (x & 0x0F0F0F0F) + (x >>  4 & 0x0F0F0F0F);
    x = (x & 0x00FF00FF) + (x >>  8 & 0x00FF00FF);
    x = (x & 0x0000FFFF) + (x >> 16 & 0x0000FFFF);
    return x;
}

NW_G3D_FORCE_INLINE
int CountLeadingZeros(bit32 x)
{
#if defined( __ghs__ )
    return __CLZ32(x);
#else
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return CountOnes(~x);
#endif
}

NW_G3D_FORCE_INLINE
int CountTrailingZeros(bit32 x)
{
#if defined( __ghs__ )
    return 32 - CountLeadingZeros(~x & (x - 1));
#else
    return CountOnes((x & (~x + 1)) - 1);
#endif
}

} } } // namespace nw::g3d::ut

#endif // NW_G3D_UT_INLINES_H_
