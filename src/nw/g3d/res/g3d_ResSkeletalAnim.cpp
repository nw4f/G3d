#include <nw/g3d/res/g3d_ResSkeletalAnim.h>
#include <algorithm>

namespace nw { namespace g3d { namespace res {

void ResBoneAnim::Init(BoneAnimResult* pResult, const ResBone* bone) const
{
    NW_G3D_ASSERT_NOT_NULL(pResult);

    bit32 flag = ref().flag;
    const float* pBaseValue = ref().ofsBaseValueArray.to_ptr<float>();
    NW_G3D_TABLE_FIELD float identityRT[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    if (flag & BASE_SCALE)
    {
        pResult->scale.Set(ref().ofsBaseValueArray.to_ptr<float>());
        pBaseValue += sizeof(Vec3) / sizeof(float);
    }
    else
    {
        NW_G3D_TABLE_FIELD float identityS[] = { 1.0f, 1.0f, 1.0f };
        const float* pScale = bone != NULL ? bone->ref().scale.a : identityS;
        pResult->scale.Set(pScale);
    }

    if (flag & BASE_ROTATE)
    {
        pResult->quat.Set(pBaseValue);
        pBaseValue += sizeof(Quat) / sizeof(float);
    }
    else
    {
        const float *pRotate = bone != NULL ? bone->ref().rotate.quat.a : identityRT;
        pResult->quat.Set(pRotate);
    }

    if (flag & BASE_TRANSLATE)
    {
        pResult->translate.Set(pBaseValue);
    }
    else
    {
        const float *pTranslate = bone != NULL ? bone->ref().translate.a : identityRT;
        pResult->translate.Set(pTranslate);
    }

    pResult->flag = (flag & TRANSFORM_MASK);
}

void ResBoneAnim::Eval(BoneAnimResult* pResult, float frame) const
{
    NW_G3D_ASSERT_NOT_NULL(pResult);

    for (int idxCurve = 0, numCurve = GetCurveCount(); idxCurve < numCurve; ++idxCurve)
    {
        const ResAnimCurve* pCurve = GetCurve(idxCurve);
        float* pTarget = AddOffset<float>(pResult, pCurve->ref().targetOffset);
        *pTarget = pCurve->EvalFloat(frame);
    }
}

void ResBoneAnim::Eval(BoneAnimResult* pResult, float frame, AnimFrameCache* pFrameCache) const
{
    NW_G3D_ASSERT_NOT_NULL(pResult);
    NW_G3D_ASSERT_NOT_NULL(pFrameCache);

    for (int idxCurve = 0, numCurve = GetCurveCount(); idxCurve < numCurve; ++idxCurve)
    {
        const ResAnimCurve* pCurve = GetCurve(idxCurve);
        float* pTarget = AddOffset<float>(pResult, pCurve->ref().targetOffset);
        *pTarget = pCurve->EvalFloat(frame, &pFrameCache[idxCurve]);
    }
}

} } } // namespace nw::g3d::res
