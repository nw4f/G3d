#ifndef NW_G3D_RES_RESANIMCURVE_H_
#define NW_G3D_RES_RESANIMCURVE_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/math/g3d_MathCommon.h>
#include <nw/g3d/res/g3d_ResCommon.h>

#include <limits>

namespace nw { namespace g3d { namespace res {

struct AnimFlag
{
    enum
    {
        CURVE_BAKED     = 0x1 << 0,

        PLAYPOLICY_LOOP = 0x1 << 2,

        NOT_BOUND       = 0xFFFF
    };
};

struct AnimFrameCache
{
    float start;
    float end;
    int keyIndex;
};

struct ResAnimConstantData
{
    u32 targetOffset;
    union
    {
        f32 fValue;
        s32 iValue;
    };
};

class ResAnimConstant : private ResAnimConstantData
{
    NW_G3D_RES_COMMON(ResAnimConstant);

public:
    float GetFloat() const { return ref().fValue; }
    int GetInt() const { return ref().iValue; }
};

struct ResAnimCurveData
{
    bit16 flag;
    u16 numKey;
    u32 targetOffset;
    f32 startFrame;
    f32 endFrame;
    union
    {
        f32 fScale;
        s32 iScale;
    };
    union
    {
        f32 fOffset;
        s32 iOffset;
    };

    Offset ofsFrameArray;
    Offset ofsKeyArray;
};

class ResAnimCurve : private ResAnimCurveData
{
    NW_G3D_RES_COMMON(ResAnimCurve);

public:
    enum Flag
    {
        FRAME_SHIFT         = 0,
        FRAME32             = 0x0 << FRAME_SHIFT,
        FRAME16             = 0x1 << FRAME_SHIFT,
        FRAME8              = 0x2 << FRAME_SHIFT,
        FRAME_MASK          = 0x3 << FRAME_SHIFT,
        NUM_FRAME           = 3,

        KEY_SHIFT           = 2,
        KEY32               = 0x0 << KEY_SHIFT,
        KEY16               = 0x1 << KEY_SHIFT,
        KEY8                = 0x2 << KEY_SHIFT,
        KEY_MASK            = 0x3 << KEY_SHIFT,
        NUM_KEY             = 3,

        CURVE_SHIFT         = 4,
        CURVE_CUBIC         = 0x0 << CURVE_SHIFT,
        CURVE_LINEAR        = 0x1 << CURVE_SHIFT,
        CURVE_BAKED_FLOAT   = 0x2 << CURVE_SHIFT,
        CURVE_STEP_INT      = 0x4 << CURVE_SHIFT,
        CURVE_BAKED_INT     = 0x5 << CURVE_SHIFT,
        CURVE_STEP_BOOL     = 0x6 << CURVE_SHIFT,
        CURVE_BAKED_BOOL    = 0x7 << CURVE_SHIFT,
        CURVE_INT_BASE      = CURVE_STEP_INT,
        CURVE_MASK          = 0x7 << CURVE_SHIFT
    };

    enum WrapMode
    {
        WRAP_CLAMP,
        WRAP_REPEAT,
        WRAP_MIRROR,

        WRAP_MASK = 0x3,
        WRAP_PRE_SHIFT = 8,
        WRAP_POST_SHIFT = 12,
    };

    /*
    void Bake(void* pBuffer, size_t bufferSize) { IsFloatCurve() ? BakeFloat(pBuffer, bufferSize) : BakeInt(pBuffer, bufferSize); }
    void BakeFloat(void* pBuffer, size_t bufferSize);
    void BakeInt(void* pBuffer, size_t bufferSize);

    size_t CalcBakedSize() const { return IsFloatCurve() ? CalcBakedFloatSize() : CalcBakedIntSize(); }
    size_t CalcBakedFloatSize() const;
    size_t CalcBakedIntSize() const;

    void Reset() { IsFloatCurve() ? ResetFloat() : ResetInt(); }
    void ResetFloat();
    void ResetInt();
    */

    float EvalFloat(float frame) const
    {
        AnimFrameCache frameCache;
        frameCache.start = std::numeric_limits<float>::infinity();
        return EvalFloat(frame, &frameCache);
    }

    int EvalInt(float frame) const
    {
        AnimFrameCache frameCache;
        frameCache.start = std::numeric_limits<float>::infinity();
        return EvalInt(frame, &frameCache);
    }

    float EvalFloat(float frame, AnimFrameCache* pFrameCache) const;
    int EvalInt(float frame, AnimFrameCache* pFrameCache) const;

    bool IsFloatCurve() const { return (ref().flag & CURVE_MASK) < CURVE_INT_BASE; }
    bool IsIntCurve() const { return (ref().flag & CURVE_MASK) >= CURVE_INT_BASE; }

    WrapMode GetPreWrapMode() const { return WrapMode((ref().flag >> WRAP_PRE_SHIFT) & WRAP_MASK);}
    WrapMode GetPostWrapMode() const { return WrapMode((ref().flag >> WRAP_POST_SHIFT) & WRAP_MASK);}

    float GetStartFrame() const { return ref().startFrame; }
    float GetEndFrame() const { return ref().endFrame; }

protected:
    class Impl;

    bit32 GetFrameType() const { return ref().flag & FRAME_MASK; }
    bit32 GetKeyType() const { return ref().flag & KEY_MASK; }
    bit32 GetCurveType() const { return ref().flag & CURVE_MASK; }

    float WrapFrame(float frame) const;

    void UpdateFrameCache(AnimFrameCache* pFrameCache, float frame) const;

    template <typename T>
    void FindFrame(AnimFrameCache *pFrameCache, float frame) const;

    template <typename T>
    float EvalCubic(float frame, AnimFrameCache* pFrameCache) const;

    template <typename T>
    float EvalLinear(float frame, AnimFrameCache* pFrameCache) const;

    template <typename T>
    float EvalBakedFloat(float frame, AnimFrameCache* pFrameCache) const;

    template <typename T>
    int EvalStepInt(float frame, AnimFrameCache* pFrameCache) const;

    template <typename T>
    int EvalBakedInt(float frame, AnimFrameCache* pFrameCache) const;

    int EvalStepBool(float frame, AnimFrameCache* pFrameCache) const;

    int EvalBakedBool(float frame, AnimFrameCache* pFrameCache) const;

    template <typename T>
    void BakeImpl(void* pBuffer, float start, int numKey);
};

NW_G3D_FORCE_INLINE
float CastS10_5ToF32(s16 value)
{
    return FastCast<float>(value) * (1.0f / 32.0f);
}

NW_G3D_FORCE_INLINE
s16 CastF32ToS10_5(float value)
{
    return Math::Floor<s16>(value * 32.0f);
}

NW_G3D_FORCE_INLINE
float CalcCubic(float t, float c0, float c1, float c2, float c3)
{
    return ((c3 * t + c2) * t) * t + (c1 * t + c0);
}

NW_G3D_FORCE_INLINE
float CalcLinear(float t, float c0, float c1)
{
    return c1 * t + c0;
}

template <typename T>
struct Frame
{
    T frame;

    float Get() const { return FastCast<float>(frame); }

    static T Quantize(float frame) { return Math::Floor<T>(frame); }
};

template <>
struct Frame<float>
{
    float frame;

    float Get() const { return frame; }

    static float Quantize(float frame) { return frame; }
};

template <>
struct Frame<s16>
{
    s16 frame;

    float Get() const { return CastS10_5ToF32(frame); }

    static s16 Quantize(float frame) { return CastF32ToS10_5(frame); }
};

template <typename T>
struct ResCubicKey
{
    T coef[4];

    float Get(float ratio) const
    {
        return CalcCubic(ratio, static_cast<float>(coef[0]), static_cast<float>(coef[1]),
            static_cast<float>(coef[2]), static_cast<float>(coef[3]));
    }
};

template <typename T>
struct ResLinearKey
{
    T coef[2];

    float Get(float ratio) const
    {
        return CalcLinear(ratio, static_cast<float>(coef[0]), static_cast<float>(coef[1]));
    }
};

template <typename T>
struct ResFloatKey
{
    T value;

    float Get() const { return StaticCast<float>(value); }
};

template <typename T>
struct ResIntKey
{
    T value;

    int Get() const { return static_cast<int>(value); }
};

#if defined( __ghs__ )

template <>
NW_G3D_FORCE_INLINE
float ResCubicKey<float>::Get(float param) const
{
    f32x2 term23 = __PSQ_LX(coef, 8, 0, 0);
    f32x2 term01 = __PSQ_LX(coef, 0, 0, 0);
    term23 = __PS_MULS0F(term23, param);
    term23 = __PS_MULS0F(term23, param);
    f32x2 one_t = { 1.0f, param };
    term01 = __PS_MUL(term01, one_t);
    term23 = __PS_MUL(term23, one_t);
    f32x2 sum = __PS_ADD(term01, term23);
    sum = __PS_SUM0(sum, sum, sum);
    return sum[0];
};

template <>
NW_G3D_FORCE_INLINE
float ResCubicKey<s16>::Get(float param) const
{
    f32x2 term23 = __PSQ_LX(coef, 4, 0, OS_FASTCAST_S16);
    f32x2 term01 = __PSQ_LX(coef, 0, 0, OS_FASTCAST_S16);
    term23 = __PS_MULS0F(term23, param);
    term23 = __PS_MULS0F(term23, param);
    f32x2 one_t = { 1.0f, param };
    term01 = __PS_MUL(term01, one_t);
    term23 = __PS_MUL(term23, one_t);
    f32x2 sum = __PS_ADD(term01, term23);
    sum = __PS_SUM0(sum, sum, sum);
    return sum[0];
};

template <>
NW_G3D_FORCE_INLINE
float ResCubicKey<s8>::Get(float param) const
{
    f32x2 term23 = __PSQ_LX(coef, 2, 0, OS_FASTCAST_S8);
    f32x2 term01 = __PSQ_LX(coef, 0, 0, OS_FASTCAST_S8);
    term23 = __PS_MULS0F(term23, param);
    term23 = __PS_MULS0F(term23, param);
    f32x2 one_t = { 1.0f, param };
    term01 = __PS_MUL(term01, one_t);
    term23 = __PS_MUL(term23, one_t);
    f32x2 sum = __PS_ADD(term01, term23);
    sum = __PS_SUM0(sum, sum, sum);
    return sum[0];
};

template <>
NW_G3D_FORCE_INLINE
float ResLinearKey<float>::Get(float param) const
{
    f32x2 coef01 = __PSQ_L(coef, 0, 0);
    return coef01[0] + coef01[1] * param;
};

template <>
NW_G3D_FORCE_INLINE
float ResLinearKey<s16>::Get(float param) const
{
    f32x2 coef01 = __PSQ_L(coef, 0, OS_FASTCAST_S16);
    return coef01[0] + coef01[1] * param;
};

template <>
NW_G3D_FORCE_INLINE
float ResLinearKey<s8>::Get(float param) const
{
    f32x2 coef01 = __PSQ_L(coef, 0, OS_FASTCAST_S8);
    return coef01[0] + coef01[1] * param;
};

#endif

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESANIMCURVE_H_
