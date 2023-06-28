#include <cmath>
#include <algorithm>
#include <limits>

#if defined( __ghs__ )
#include <ppc_ghs.h>
#endif

#include <nw/g3d/ut/g3d_Inlines.h>

namespace nw { namespace g3d { namespace math {

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
float Math::Abs(float x)
{
    return std::fabs(x);
}

NW_G3D_MATH_INLINE
float Math::NAbs(float x)
{
    return -std::fabs(x);
}

NW_G3D_MATH_INLINE
float Math::Mod(float x, float y)
{
    return std::fmod(x, y);
}

NW_G3D_MATH_INLINE
float Math::Select(float cond, float pos, float neg)
{
#if defined( __ghs__ )
    return __FSEL(cond, pos, neg);
#else
    return cond >= 0.0f ? pos : neg;
#endif
}

NW_G3D_MATH_INLINE
float Math::Min(float x, float y)
{
    return Select(x - y, y, x);
}

NW_G3D_MATH_INLINE
float Math::Max(float x, float y)
{
    return Select(x - y, x, y);
}

NW_G3D_MATH_INLINE
float Math::Clamp(float x, float minVal, float maxVal)
{
    return Max(minVal, Min(maxVal, x));
}

//--------------------------------------------------------------------------------------------------

template <typename T>
NW_G3D_MATH_INLINE
T Math::Ceil(float x)
{
    return StaticCast<T>(std::ceil(x));
}

template <typename T>
NW_G3D_MATH_INLINE
T Math::Floor(float x)
{
    if (NW_G3D_STATIC_CONDITION(std::numeric_limits<T>::is_signed))
    {
        return StaticCast<T>(std::floor(x));
    }
    else
    {
        return Trunc<T>(x);
    }
}

template <typename T>
NW_G3D_MATH_INLINE
T Math::Trunc(float x)
{
    return StaticCast<T>(x);
}

template <>
NW_G3D_MATH_INLINE
float Math::Trunc<float>(float x)
{
    float out;
    std::modf(x, &out);
    return out;
}

template <typename T>
NW_G3D_MATH_INLINE
T Math::Round(float x)
{
    if(NW_G3D_STATIC_CONDITION(std::numeric_limits<T>::is_signed))
    {
        return StaticCast<T>(x + Select(x, 0.5f, -0.5f));
    }
    else
    {
        return StaticCast<T>(x);
    }
}

template <>
NW_G3D_MATH_INLINE
float Math::Round<float>(float x)
{
    float out;
    std::modf(x + Select(x, 0.5f, -0.5f), &out);
    return out;
}


NW_G3D_MATH_INLINE
float Math::Ceil(float x) { return Ceil<float>(x); }

NW_G3D_MATH_INLINE
float Math::Floor(float x) { return Floor<float>(x); }

NW_G3D_MATH_INLINE
float Math::Trunc(float x) { return Trunc<float>(x); }

NW_G3D_MATH_INLINE
float Math::Round(float x) { return Round<float>(x); }

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
float Math::Rcp(float x)
{
#if defined( __ghs__ )
    float inv = __FRES(x);
    return (inv + inv) - (x * inv) * inv;
#else
    return 1.0f / x;
#endif
}

NW_G3D_MATH_INLINE
float Math::RSqrt(float x)
{
#if defined( __ghs__ )
    float half = 0.5f * x;
    float rsqrt = __FRSQRTE(x);
    rsqrt = rsqrt * (1.5f - half * rsqrt * rsqrt);
    return rsqrt;
#else
    return 1.0f / std::sqrt(x);
#endif
}

NW_G3D_MATH_INLINE
float Math::Sqrt(float x)
{
#if defined( __ghs__ )
    return x == 0.0f ? 0.0f : x * RSqrt(x);
#else
    return std::sqrt(x);
#endif
}

NW_G3D_MATH_INLINE
float Math::Exp(float x)
{
    return std::exp(x);
}

NW_G3D_MATH_INLINE
float Math::Log(float x)
{
    return std::log(x);
}

NW_G3D_MATH_INLINE
float Math::Log10(float x)
{
    return std::log10(x);
}

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
float Math::Sin(float rad)
{
    return SinIdx(RadToIdx(rad));
}

NW_G3D_MATH_INLINE
float Math::Cos(float rad)
{
    return CosIdx(RadToIdx(rad));
}

NW_G3D_MATH_INLINE
float Math::Tan(float rad)
{
    return TanIdx(RadToIdx(rad));
}

NW_G3D_MATH_INLINE
float Math::Asin(float x)
{
    NW_G3D_ASSERT(-1 <= x && x <= 1);
    return std::asin(x);
}

NW_G3D_MATH_INLINE
float Math::Acos(float x)
{
    NW_G3D_ASSERT(-1 <= x && x <= 1);
    return std::acos(x);
}

NW_G3D_MATH_INLINE
float Math::Atan(float x)
{
    NW_G3D_ASSERT(-1 <= x && x <= 1);
    return std::atan(x);
}

NW_G3D_MATH_INLINE
float Math::Atan2(float y, float x)
{
    return std::atan2(y, x);
}

NW_G3D_MATH_INLINE
void Math::SinCos(float* pSin, float* pCos, float rad)
{
    SinCosIdx(pSin, pCos, RadToIdx(rad));
}

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
float Math::SinIdx(u32 idx)
{
    typedef internal::SinCosSample Sample;

    u32 tblidx = (idx >> Sample::FRAC_BIT) & Sample::INDEX_MASK;
    float fraction = static_cast<float>(idx & Sample::FRAC_MASK) / static_cast<int>(Sample::FRAC_SIZE);
    const Sample& sample = internal::gSinCosTable[tblidx];

    return sample.sinValue + sample.sinDelta * fraction;
}

NW_G3D_MATH_INLINE
float Math::CosIdx(u32 idx)
{
    typedef internal::SinCosSample Sample;

    u32 tblidx = (idx >> Sample::FRAC_BIT) & Sample::INDEX_MASK;
    float fraction = static_cast<float>(idx & Sample::FRAC_MASK) / static_cast<int>(Sample::FRAC_SIZE);
    const Sample& sample = internal::gSinCosTable[tblidx];

    return sample.cosValue + sample.cosDelta * fraction;
}

NW_G3D_MATH_INLINE
float Math::TanIdx(u32 idx)
{
    float fSin, fCos;
    SinCosIdx(&fSin, &fCos, idx);
    return fSin * Rcp(fCos);
}

NW_G3D_MATH_INLINE
u32 Math::AsinIdx(float x)
{
    NW_G3D_ASSERT(-1 <= x && x <= 1);
    return RadToIdx(std::asin(x));
}

NW_G3D_MATH_INLINE
u32 Math::AcosIdx(float x)
{
    NW_G3D_ASSERT(-1 <= x && x <= 1);
    return RadToIdx(std::acos(x));
}

NW_G3D_MATH_INLINE
u32 Math::AtanIdx(float x)
{
    NW_G3D_ASSERT(-1 <= x && x <= 1);
    return RadToIdx(std::atan(x));
}

NW_G3D_MATH_INLINE
u32 Math::Atan2Idx(float y, float x)
{
    return RadToIdx(std::atan2(y, x));
}

NW_G3D_MATH_INLINE
void Math::SinCosIdx(float* pSin, float* pCos, u32 idx)
{
    NW_G3D_ASSERT_NOT_NULL(pSin);
    NW_G3D_ASSERT_NOT_NULL(pCos);
    typedef internal::SinCosSample Sample;

    u32 tblidx = (idx >> Sample::FRAC_BIT) & Sample::INDEX_MASK;
    float fraction = static_cast<float>(idx & Sample::FRAC_MASK) / static_cast<int>(Sample::FRAC_SIZE);
    const Sample& sample = internal::gSinCosTable[tblidx];

    *pSin = sample.sinValue + sample.sinDelta * fraction;
    *pCos = sample.cosValue + sample.cosDelta * fraction;
}

//--------------------------------------------------------------------------------------------------

#if defined( __ghs__ )

NW_G3D_MATH_INLINE
f32x2 Math::Rcp(f32x2 x)
{
    f32x2 inv = __PS_RES(x);
    return __PS_NMSUB(__PS_MUL(x, inv), inv, __PS_ADD(inv, inv));
}

NW_G3D_MATH_INLINE
f32x2 Math::RSqrt(f32x2 x)
{
    f32x2 rsqrt = __PS_RSQRTE(x);
    f32x2 half = __PS_MULS0F(x, 0.5f);
    f32x2 onehalf = __PS_FDUP(1.5f);
    return __PS_MUL(rsqrt, __PS_NMSUB(__PS_MUL(half, rsqrt), rsqrt, onehalf));
}

NW_G3D_MATH_INLINE
f32x2 Math::SinCos(float rad)
{
    return SinCosIdx(RadToIdx(rad));
}

NW_G3D_MATH_INLINE
f32x2 Math::SinCosIdx(u32 idx)
{
    typedef internal::SinCosSample Sample;

    u32 tblidx = (idx >> Sample::FRAC_BIT) & Sample::INDEX_MASK;

    u16 fracU16 = (idx >> 8) & 0xFFFF;
    f32x2 fraction = __PSQ_L(&fracU16, 1, OS_FASTCAST_U16);
    fraction = __PS_MULS0F(fraction, static_cast<float>(1.0f / 0xFFFF));

    const Sample& sample = internal::gSinCosTable[tblidx];
    const f32x2& value = reinterpret_cast<const f32x2&>(sample.sinValue);
    const f32x2& delta = reinterpret_cast<const f32x2&>(sample.sinDelta);
    return __PS_MADDS0(delta, fraction, value);
}

#endif // defined( __ghs__ )

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
bool Math::IsNaN(float x)
{
    return x != x;
}

} } } // namespace nw::g3d::math
