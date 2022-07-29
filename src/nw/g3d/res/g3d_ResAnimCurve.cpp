#include <nw/g3d/res/g3d_ResAnimCurve.h>
#include <nw/g3d/math/g3d_MathCommon.h>
#include <nw/g3d/ut/g3d_Flag.h>

#include <limits.h>

namespace nw { namespace g3d { namespace res {

class ResAnimCurve::Impl
{
public:
    static void (ResAnimCurve::* const s_pFuncFindFrame[])(AnimFrameCache*, float) const;
    static float (ResAnimCurve::* const s_pFuncEvalFloat[][NUM_KEY])(float, AnimFrameCache*) const;
    static int (ResAnimCurve::* const s_pFuncEvalInt[][NUM_KEY])(float, AnimFrameCache*) const;
};

void (ResAnimCurve::* const ResAnimCurve::Impl::s_pFuncFindFrame[])(AnimFrameCache*, float) const = {
    &ResAnimCurve::FindFrame<float>,
    &ResAnimCurve::FindFrame<s16>,
    &ResAnimCurve::FindFrame<u8>
};

float (ResAnimCurve::* const ResAnimCurve::Impl::s_pFuncEvalFloat[][NUM_KEY])(float, AnimFrameCache*) const = {
    {
        &ResAnimCurve::EvalCubic<float>,
        &ResAnimCurve::EvalCubic<s16>,
        &ResAnimCurve::EvalCubic<s8>
    },
    {
        &ResAnimCurve::EvalLinear<float>,
        &ResAnimCurve::EvalLinear<s16>,
        &ResAnimCurve::EvalLinear<s8>
    },
    {
        &ResAnimCurve::EvalBakedFloat<float>,
        &ResAnimCurve::EvalBakedFloat<s16>,
        &ResAnimCurve::EvalBakedFloat<s8>
    },
};

int (ResAnimCurve::* const ResAnimCurve::Impl::s_pFuncEvalInt[][NUM_KEY])(float, AnimFrameCache*) const = {
    {
        &ResAnimCurve::EvalStepInt<s32>,
        &ResAnimCurve::EvalStepInt<s16>,
        &ResAnimCurve::EvalStepInt<s8>
    },
    {
        &ResAnimCurve::EvalBakedInt<s32>,
        &ResAnimCurve::EvalBakedInt<s16>,
        &ResAnimCurve::EvalBakedInt<s8>
    },
    {
        &ResAnimCurve::EvalStepBool,
        &ResAnimCurve::EvalStepBool,
        &ResAnimCurve::EvalStepBool
    },
    {
        &ResAnimCurve::EvalBakedBool,
        &ResAnimCurve::EvalBakedBool,
        &ResAnimCurve::EvalBakedBool
    }
};

float ResAnimCurve::EvalFloat(float frame, AnimFrameCache* pFrameCache) const
{
    NW_G3D_ASSERT(IsFloatCurve());
    NW_G3D_ASSERT_NOT_NULL(pFrameCache);

    frame = WrapFrame(frame);
    int curveType = GetCurveType() >> CURVE_SHIFT;
    int keyType = GetKeyType() >> KEY_SHIFT;
    float result = (this->*Impl::s_pFuncEvalFloat[curveType][keyType])(frame, pFrameCache);
    return result * ref().fScale + ref().fOffset;
}

int ResAnimCurve::EvalInt(float frame, AnimFrameCache* pFrameCache) const
{
    NW_G3D_ASSERT(IsIntCurve());
    NW_G3D_ASSERT_NOT_NULL(pFrameCache);

    frame = WrapFrame(frame);
    int curveType = (GetCurveType() - CURVE_INT_BASE) >> CURVE_SHIFT;
    int keyType = GetKeyType() >> KEY_SHIFT;
    int result = (this->*Impl::s_pFuncEvalInt[curveType][keyType])(frame, pFrameCache);
    return result + ref().iOffset;
}

float ResAnimCurve::WrapFrame(float frame) const
{
    float startFrame = GetStartFrame();
    float endFrame = GetEndFrame();
    float postframeDiff = frame - endFrame;
    float preframeDiff = startFrame - frame;
    if (postframeDiff * preframeDiff >= 0)
    {
        return frame;
    }
    WrapMode wrapMode;
    float frameDiff;
    bool outWrap;
    if (preframeDiff > 0)
    {
        wrapMode = GetPreWrapMode();
        if (WRAP_CLAMP == wrapMode)
        {
            return startFrame;
        }
        frameDiff = preframeDiff;
        outWrap = false;
    }
    else
    {
        wrapMode = GetPostWrapMode();
        if (WRAP_CLAMP == wrapMode)
        {
            return endFrame;
        }
        frameDiff = postframeDiff;
        outWrap = true;
    }
    float duration = endFrame - startFrame;
    int count = static_cast<int>(frameDiff / duration);
    float frameMod = frameDiff - duration * count;
    u32 repeat = 1;
    if (wrapMode == WRAP_REPEAT)
    {
    }
    else
    {
        if (wrapMode != WRAP_MIRROR || !(count & 0x01))
        {
            repeat = 0;
        }
    }
    if ((u8)repeat != outWrap)
    {
        frame = endFrame - frameMod;
    }
    else
    {
        frame = startFrame + frameMod;
    }
    NW_G3D_ASSERT(ref().startFrame <= frame && frame <= ref().endFrame);
    return frame;
}

void ResAnimCurve::UpdateFrameCache(AnimFrameCache* pFrameCache, float frame) const
{
    if (frame < pFrameCache->start || pFrameCache->end <= frame)
    {
        float denom = Math::Rcp(ref().endFrame - ref().startFrame);
        pFrameCache->keyIndex = FastCast<u16>((frame - ref().startFrame) * denom);

        int frameType = GetFrameType() >> FRAME_SHIFT;
        (this->*Impl::s_pFuncFindFrame[frameType])(pFrameCache, frame);
    }
}


template <typename T>
void ResAnimCurve::FindFrame(AnimFrameCache *pFrameCache, float frame) const
{
    typedef Frame<T> FrameType;
    const FrameType* pFrameArray = ref().ofsFrameArray.to_ptr<FrameType>();
    T quantized = FrameType::Quantize(frame);
    int endFrameIndex = ref().numKey - 1;

    if (quantized >= pFrameArray[endFrameIndex].frame)
    {
        pFrameCache->keyIndex = static_cast<u16>(endFrameIndex);
        pFrameCache->start = pFrameArray[pFrameCache->keyIndex].Get();
        pFrameCache->end = pFrameCache->start + 1.0f;
        return;
    }

    int keyIndex = pFrameCache->keyIndex;
    if (quantized < pFrameArray[keyIndex].frame)
    {
        do
        {
            NW_G3D_ASSERT(0 < keyIndex);
            --keyIndex;
        } while (quantized < pFrameArray[keyIndex].frame);
    }
    else
    {
        do
        {
            NW_G3D_ASSERT(keyIndex < endFrameIndex);
            ++keyIndex;
        } while (pFrameArray[keyIndex].frame <= quantized);

        NW_G3D_ASSERT(0 < keyIndex);
        --keyIndex;
    }

    pFrameCache->keyIndex = keyIndex;
    pFrameCache->start = pFrameArray[keyIndex].Get();
    pFrameCache->end = pFrameArray[keyIndex + 1].Get();
}

template <typename T>
float ResAnimCurve::EvalCubic(float frame, AnimFrameCache* pFrameCache) const
{
    UpdateFrameCache(pFrameCache, frame);

    float start = pFrameCache->start;
    float end = pFrameCache->end;
    float ratio = (frame - start) * Math::Rcp(end - start);

    typedef ResCubicKey<T> KeyType;
    const KeyType* pKeyArray = ref().ofsKeyArray.to_ptr<KeyType>();
    return pKeyArray[pFrameCache->keyIndex].Get(ratio);
}

template <typename T>
float ResAnimCurve::EvalLinear(float frame, AnimFrameCache* pFrameCache) const
{
    UpdateFrameCache(pFrameCache, frame);

    float start = pFrameCache->start;
    float end = pFrameCache->end;
    float ratio = (frame - start) * Math::Rcp(end - start);

    typedef ResLinearKey<T> KeyType;
    const KeyType* pKeyArray = ref().ofsKeyArray.to_ptr<KeyType>();
    return pKeyArray[pFrameCache->keyIndex].Get(ratio);
}

template <typename T>
float ResAnimCurve::EvalBakedFloat(float frame, AnimFrameCache* /*pFrameCache*/) const
{
    typedef ResFloatKey<T> KeyType;

    int start = static_cast<int>(GetStartFrame());
    int keyIndex = static_cast<int>(frame) - start;
    float ratio = frame - (start + keyIndex);

    NW_G3D_ASSERT(keyIndex + 1 < ref().numKey);

    const KeyType* pKeyArray = ref().ofsKeyArray.to_ptr<KeyType>();
    return pKeyArray[keyIndex + 1].Get() * ratio + pKeyArray[keyIndex].Get() * (1.0f - ratio);
}

template <typename T>
int ResAnimCurve::EvalStepInt(float frame, AnimFrameCache* pFrameCache) const
{
    UpdateFrameCache(pFrameCache, frame);

    typedef ResIntKey<T> KeyType;
    const KeyType* pKeyArray = ref().ofsKeyArray.to_ptr<KeyType>();
    return pKeyArray[pFrameCache->keyIndex].Get();
}

template <typename T>
int ResAnimCurve::EvalBakedInt(float frame, AnimFrameCache* /*pFrameCache*/) const
{
    typedef ResIntKey<T> KeyType;

    int start = static_cast<int>(GetStartFrame());
    int keyIndex = static_cast<int>(frame) - start;

    const KeyType* pKeyArray = ref().ofsKeyArray.to_ptr<KeyType>();
    return pKeyArray[keyIndex].Get();
}

int ResAnimCurve::EvalStepBool(float frame, AnimFrameCache* pFrameCache) const
{
    UpdateFrameCache(pFrameCache, frame);

    typedef s32 KeyType;
    const KeyType* pKeyArray = ref().ofsKeyArray.to_ptr<KeyType>();
    return (pKeyArray[pFrameCache->keyIndex >> 5] >> (pFrameCache->keyIndex & 0x1F)) & 0x1;
}

int ResAnimCurve::EvalBakedBool(float frame, AnimFrameCache* /*pFrameCache*/) const
{
    typedef s32 KeyType;

    int start = static_cast<int>(GetStartFrame());
    int keyIndex = static_cast<int>(frame) - start;

    const KeyType* pKeyArray = ref().ofsKeyArray.to_ptr<KeyType>();
    return (pKeyArray[keyIndex >> 5] >> (keyIndex & 0x1F)) & 0x1;
}

} } } // namespace nw::g3d::res
