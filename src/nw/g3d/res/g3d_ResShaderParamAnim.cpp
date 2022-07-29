#include <nw/g3d/res/g3d_ResShaderParamAnim.h>
#include <algorithm>

namespace nw { namespace g3d { namespace res {

template <bool useContext>
void ResShaderParamMatAnim::EvalImpl(
    void* pResult, float frame, const u16* pSubBindIndex, AnimFrameCache* pFrameCache) const
{
    int idxCurve = 0;
    const ParamAnimInfo* pInfo = ref().ofsParamAnimInfoArray.to_ptr<ParamAnimInfo>();
    const ResAnimCurveData* pCurve = ref().ofsCurveArray.to_ptr<ResAnimCurveData>();

    for (int idxParamAnim = 0, numParamAnim = GetParamAnimCount();
        idxParamAnim < numParamAnim; ++idxParamAnim, ++pInfo)
    {
        if (pSubBindIndex[idxParamAnim] == AnimFlag::NOT_BOUND)
        {

            continue;
        }

        int numFloat = pInfo->numFloatCurve;
        float* pFloatResult = AddOffset<float>(pResult, sizeof(float) * idxCurve);
        idxCurve += numFloat;
        for (int idxFloat = 0; idxFloat < numFloat;
            ++idxFloat, ++pFloatResult, ++pCurve, ++pFrameCache)
        {
            const ResAnimCurve* pFloatCurve = ResAnimCurve::ResCast(pCurve);
            if (NW_G3D_STATIC_CONDITION(useContext))
            {
                *pFloatResult = pFloatCurve->EvalFloat(frame, pFrameCache);
            }
            else
            {
                *pFloatResult = pFloatCurve->EvalFloat(frame);
            }
        }

        int numInt = pInfo->numIntCurve;
        int* pIntResult = AddOffset<int>(pResult, sizeof(int) * idxCurve);
        idxCurve += numInt;
        for (int idxInt = 0; idxInt < numInt; ++idxInt, ++pIntResult, ++pCurve, ++pFrameCache)
        {
            const ResAnimCurve* pIntCurve = ResAnimCurve::ResCast(pCurve);
            if (NW_G3D_STATIC_CONDITION(useContext))
            {
                *pIntResult = pIntCurve->EvalInt(frame, pFrameCache);
            }
            else
            {
                *pIntResult = pIntCurve->EvalInt(frame);
            }
        }
    }
}

template void ResShaderParamMatAnim::EvalImpl<true>(void*, float, const u16*, AnimFrameCache*) const;
template void ResShaderParamMatAnim::EvalImpl<false>(void*, float, const u16*, AnimFrameCache*) const;

} } } // namespace nw::g3d::res
