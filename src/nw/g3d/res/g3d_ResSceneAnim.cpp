#include <nw/g3d/res/g3d_ResSceneAnim.h>

namespace nw { namespace g3d { namespace res {


void ResCameraAnim::Init(CameraAnimResult* pResult) const
{
    *pResult = *ref().ofsBaseValueArray.to_ptr<CameraAnimResult>();
}

void ResCameraAnim::Eval(CameraAnimResult* pResult, float frame) const
{
    NW_G3D_ASSERT_NOT_NULL(pResult);

    for (int idxCurve = 0, numCurve = GetCurveCount(); idxCurve < numCurve; ++idxCurve)
    {
        const ResAnimCurve* pCurve = GetCurve(idxCurve);
        float* pTarget = AddOffset<float>(pResult, pCurve->ref().targetOffset);
        *pTarget = pCurve->EvalFloat(frame);
    }
}

void ResCameraAnim::Eval(CameraAnimResult* pResult, float frame, AnimFrameCache* pFrameCache) const
{
    NW_G3D_ASSERT_NOT_NULL(pResult);

    for (int idxCurve = 0, numCurve = GetCurveCount(); idxCurve < numCurve; ++idxCurve)
    {
        const ResAnimCurve* pCurve = GetCurve(idxCurve);
        float* pTarget = AddOffset<float>(pResult, pCurve->ref().targetOffset);
        *pTarget = pCurve->EvalFloat(frame, &pFrameCache[idxCurve]);
    }
}

} } } // namespace nw::g3d::res
