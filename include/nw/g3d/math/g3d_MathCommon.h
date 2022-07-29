#ifndef NW_G3D_MATH_COMMON_H_
#define NW_G3D_MATH_COMMON_H_

#include <nw/g3d/g3d_config.h>

namespace nw { namespace g3d { namespace math {

class Math
{
public:
    static float Abs(float x);
    static float NAbs(float x);
    static float Mod(float x, float y);
    static float Select(float cond, float pos, float neg);
    static float Min(float x, float y);
    static float Max(float x, float y);
    static float Clamp(float x, float minVal, float maxVal);

    static float Ceil(float x);
    static float Floor(float x);
    static float Trunc(float x);
    static float Round(float x);

    template <typename T> static T Ceil(float x);
    template <typename T> static T Floor(float x);
    template <typename T> static T Trunc(float x);
    template <typename T> static T Round(float x);

    static float Rcp(float x);
    static float RSqrt(float x);
    static float Sqrt(float x);
    static float Exp(float x);
    static float Log(float x);
    static float Log10(float x);

    static float Sin(float rad);
    static float Cos(float rad);
    static float Tan(float rad);
    static float Asin(float x);
    static float Acos(float x);
    static float Atan(float x);
    static float Atan2(float y, float x);
    static void SinCos(float* pSin, float* pCos, float rad);

    static float SinIdx(u32 idx);
    static float CosIdx(u32 idx);
    static float TanIdx(u32 idx);
    static u32 AsinIdx(float x);
    static u32 AcosIdx(float x);
    static u32 AtanIdx(float x);
    static u32 Atan2Idx(float y, float x);
    static void SinCosIdx(float* pSin, float* pCos, u32 idx);

#if defined( __ghs__ )
    static f32x2 Rcp(f32x2 x);
    static f32x2 RSqrt(f32x2 x);
    static f32x2 SinCos(float rad);
    static f32x2 SinCosIdx(u32 idx);
#endif // defined( __ghs__ )

    static bool IsNaN(float x);

    NW_G3D_MATH_INLINE static float Pi() { return 3.141592653589793f; }
    NW_G3D_MATH_INLINE static float Napier() { return 2.718281828459045f; }

    NW_G3D_MATH_INLINE
    static u32 RadToIdx(float rad)
    {
        return static_cast<u32>(static_cast<s64>(rad * (float(0x80000000) / Pi())));
    }

    NW_G3D_MATH_INLINE
    static u32 DegToIdx(float deg)
    {
        return static_cast<u32>(static_cast<s64>(deg * (float(0x80000000) / 180.0f)));
    }

    NW_G3D_MATH_INLINE
    static float IdxToRad(u32 idx)
    {
        return idx * (Pi() / float(0x80000000));
    }

    NW_G3D_MATH_INLINE
    static float IdxToDeg(u32 idx)
    {
        return idx * (180.0f / float(0x80000000));
    }

    NW_G3D_MATH_INLINE
    static float RadToDeg(float rad)
    {
        return rad * (180.0f / Pi());
    }

    NW_G3D_MATH_INLINE
    static float DegToRad(float deg)
    {
        return deg * (Pi() / 180.0f);
    }

};

namespace internal {

struct SinCosSample
{
    enum {
        INDEX_BIT = 8,
        FRAC_BIT = 24,
        TABLE_SIZE = 1 << INDEX_BIT,
        FRAC_SIZE = 1 << FRAC_BIT,
        INDEX_MASK = TABLE_SIZE - 1,
        FRAC_MASK = FRAC_SIZE - 1
    };

    float sinValue;
    float cosValue;
    float sinDelta;
    float cosDelta;
};

extern const SinCosSample gSinCosTable[SinCosSample::TABLE_SIZE];

} // namespace internal

} } } // namespace nw::g3d::math

#include "inl/g3d_MathCommon-inl.h"

#endif // NW_G3D_MATH_COMMON_H_
