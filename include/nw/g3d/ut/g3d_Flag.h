#ifndef NW_G3D_UT_FLAG_H_
#define NW_G3D_UT_FLAG_H_

#include <nw/g3d/g3d_config.h>

#define NW_G3D_FLAG_DECLARE(mbit, mname) \
    FLAG_##mname##_SHIFT = (mbit), \
    FLAG_##mname = 0x1 << (mbit)

#define NW_G3D_FLAG_VALUE_DECLARE(mbit, msize, mname) \
    FLAG_##mname##_VALUE_SHIFT = (mbit), \
    FLAG_##mname##_VALUE_MASK = (((0x1ul << (msize)) - 1) << (mbit))

#define NW_G3D_GET_FLAG_VALUE(mflags, mname, mtype) \
    GetFlagValue<mtype>((mflags), (FLAG_##mname##_VALUE_SHIFT), (FLAG_##mname##_VALUE_MASK))

#define NW_G3D_SET_FLAG_VALUE(mflags, mname, mval) \
    ((mflags) = SetFlagValue( \
        (mflags), (FLAG_##mname##_VALUE_SHIFT), (FLAG_##mname##_VALUE_MASK), (mval)))

namespace nw { namespace g3d { namespace ut {

template<typename TFlags, typename TMask, typename TCheck>
NW_G3D_INLINE
bool CheckFlag(TFlags flags, TMask mask, TCheck check)
{
    return (flags & static_cast<TFlags>(mask)) == static_cast<TFlags>(check);
}

template<typename TFlags, typename TMask>
NW_G3D_INLINE
bool CheckFlag(TFlags flags, TMask mask)
{
    return CheckFlag(flags, mask, mask);
}

template<typename TFlags, typename TMask, typename TValue>
NW_G3D_INLINE
TFlags CreateFlagValue(TValue value, int shift, TMask mask)
{
    return (value << shift) & static_cast<TFlags>(mask);
}

template<typename TResult, typename TFlags, typename TMask>
NW_G3D_INLINE
TResult GetFlagValue(TFlags flags, int shift, TMask mask)
{
    return static_cast<TResult>((flags & static_cast<TFlags>(mask)) >> shift);
}

template<typename TFlags, typename TMask, typename TValue>
NW_G3D_INLINE
TFlags SetFlagValue(TFlags flags, int shift, TMask mask, TValue value)
{
    return (flags & ~(static_cast<TFlags>(mask))) | ((value << shift) & static_cast<TFlags>(mask));
}

template<typename TFlags, typename TMask>
NW_G3D_INLINE
TFlags InvertFlag(TFlags flags, TMask mask)
{
    return (flags & ~(static_cast<TFlags>(mask))) | ((~flags) & static_cast<TFlags>(mask));
}

template<typename TValue>
NW_G3D_INLINE
void FillBits(bit32* pBitArray, int count, TValue value)
{
    NW_G3D_ASSERT(count == 0 || pBitArray);
    NW_G3D_ASSERT((static_cast<bit32>(value) & (~bit32(0x1))) == 0);
    int numWord = ((count + 0x1F) >> 5);
    if (value)
    {
        for (int idxWord = 0; idxWord < numWord; ++idxWord)
        {
            pBitArray[idxWord] = ~bit32(0);
        }
    }
    else
    {
        for (int idxWord = 0; idxWord < numWord; ++idxWord)
        {
            pBitArray[idxWord] = 0;
        }
    }
}

template<typename TResult>
NW_G3D_INLINE
TResult IsBitOn(const bit32* pBitArray, int wordIndex, int bitIndex)
{
    NW_G3D_ASSERT((bitIndex & ~0x1F) == 0);
    return static_cast<TResult>((pBitArray[wordIndex] >> bitIndex) & 0x1u);
}

template<typename TResult>
NW_G3D_INLINE
TResult IsBitOn(const bit32* pBitArray, int index)
{
    return IsBitOn<TResult>(pBitArray, index >> 5, index & 0x1F);
}

template<typename TValue>
NW_G3D_INLINE
void SetBit(bit32* pBitArray, int wordIndex, int bitIndex, TValue value)
{
    NW_G3D_ASSERT((bitIndex & ~0x1F) == 0);
    NW_G3D_ASSERT((static_cast<bit32>(value) & (~bit32(0x1))) == 0);
    bit32& word = pBitArray[wordIndex];
    word &= ~(0x1u << bitIndex);
    word |= static_cast<bit32>(value) << bitIndex;
}

template<typename TValue>
NW_G3D_INLINE
void SetBit(bit32* pBitArray, int index, TValue value)
{
    SetBit(pBitArray, index >> 5, index & 0x1F, value);
}

} } } // namespace nw::g3d::ut

#endif //  NW_G3D_UT_FLAG_H_
