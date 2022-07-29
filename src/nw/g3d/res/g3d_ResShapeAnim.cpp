#include <nw/g3d/res/g3d_ResShapeAnim.h>
#include <nw/g3d/res/g3d_ResFile.h>

namespace nw { namespace g3d { namespace res {

void ResVertexShapeAnim::Eval(
    float* pResult, float frame, const s8* pSubBindIndex) const
{
    NW_G3D_ASSERT_NOT_NULL(pResult);
    NW_G3D_ASSERT_NOT_NULL(pSubBindIndex);

    const KeyShapeAnimInfo* pInfo = ref().ofsKeyShapeAnimInfoArray.to_ptr<KeyShapeAnimInfo>();

    for (int idxKeyShapeAnim = 0, numKeyShapeAnim = GetKeyShapeAnimCount();
        idxKeyShapeAnim < numKeyShapeAnim; ++idxKeyShapeAnim, ++pInfo)
    {
        if (pSubBindIndex[idxKeyShapeAnim] == -1 ||
            pInfo->curveIndex == -1)
        {
            continue;
        }

        const ResAnimCurve* pCurve = GetCurve(pInfo->curveIndex);
        pResult[idxKeyShapeAnim] = pCurve->EvalFloat(frame);
    }
}

void ResVertexShapeAnim::Eval(
    float* pResult, float frame, const s8* pSubBindIndex,
    AnimFrameCache* pFrameCache) const
{
    NW_G3D_ASSERT_NOT_NULL(pResult);
    NW_G3D_ASSERT_NOT_NULL(pSubBindIndex);

    const KeyShapeAnimInfo* pInfo = ref().ofsKeyShapeAnimInfoArray.to_ptr<KeyShapeAnimInfo>();

    for (int idxKeyShapeAnim = 0, numKeyShapeAnim = GetKeyShapeAnimCount();
        idxKeyShapeAnim < numKeyShapeAnim; ++idxKeyShapeAnim, ++pInfo)
    {
        if (pSubBindIndex[idxKeyShapeAnim] == -1 ||
            pInfo->curveIndex == -1)
        {
            continue;
        }

        const ResAnimCurve* pCurve = GetCurve(pInfo->curveIndex);
        pResult[idxKeyShapeAnim] = pCurve->EvalFloat(frame, &pFrameCache[pInfo->curveIndex]);
    }
}

void ResVertexShapeAnim::Init(float* pResult) const
{
    NW_G3D_ASSERT_NOT_NULL(pResult);

    pResult[0] = 0.0f;
    const float* pBaseValueArray = ref().ofsBaseValueArray.to_ptr<float>();
    for (int idxKeyShapeAnim = 1, numKeyShapeAnim = GetKeyShapeAnimCount();
        idxKeyShapeAnim < numKeyShapeAnim; ++idxKeyShapeAnim)
    {
        pResult[idxKeyShapeAnim] = pBaseValueArray[idxKeyShapeAnim - 1];
    }
}

} } } // namespace nw::g3d::res
