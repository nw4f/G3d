#include <nw/g3d/res/g3d_ResTexPatternAnim.h>
#include <algorithm>
#include <nw/g3d/res/g3d_ResFile.h>

namespace nw { namespace g3d { namespace res {

void ResTexPatternMatAnim::Eval(
    u16* pResult, float frame, const s8* pSubBindIndex) const
{
    const PatAnimInfo* pInfo = ref().ofsPatAnimInfoArray.to_ptr<PatAnimInfo>();

    for (int idxPatAnim = 0, numPatAnim = GetPatAnimCount();
        idxPatAnim < numPatAnim; ++idxPatAnim, ++pInfo)
    {
        if (pSubBindIndex[idxPatAnim] == -1 ||
            pInfo->curveIndex == -1)
        {
            continue;
        }

        const ResAnimCurve* pCurve = GetCurve(pInfo->curveIndex);
        pResult[idxPatAnim] = static_cast<u16>(pCurve->EvalInt(frame));
    }
}

void ResTexPatternMatAnim::Eval(
    u16* pResult, float frame, const s8* pSubBindIndex, AnimFrameCache* pFrameCache) const
{
    const PatAnimInfo* pInfo = ref().ofsPatAnimInfoArray.to_ptr<PatAnimInfo>();

    for (int idxPatAnim = 0, numPatAnim = GetPatAnimCount();
        idxPatAnim < numPatAnim; ++idxPatAnim, ++pInfo)
    {
        if (pSubBindIndex[idxPatAnim] == -1 ||
            pInfo->curveIndex == -1)
        {
            continue;
        }

        const ResAnimCurve* pCurve = GetCurve(pInfo->curveIndex);
        pResult[idxPatAnim] = static_cast<u16>(
            pCurve->EvalInt(frame, &pFrameCache[pInfo->curveIndex]));
    }
}

void ResTexPatternMatAnim::Init(u16* pResult) const
{
    const u16* pBaseValueArray = ref().ofsBaseValueArray.to_ptr<u16>();
    for (int idxPatAnim = 0, numPatAnim = GetPatAnimCount(); idxPatAnim < numPatAnim; ++idxPatAnim)
    {
        pResult[idxPatAnim] = pBaseValueArray[idxPatAnim];
    }
}

BindResult ResTexPatternAnim::Bind(const ResFile* pFile)
{
    NW_G3D_ASSERT_NOT_NULL(pFile);

    BindResult result;

    for (int idxTex = 0, numTex = GetTextureRefCount(); idxTex < numTex; ++idxTex)
    {
        ResTextureRef* pTextureRef = GetTextureRef(idxTex);
        if (!pTextureRef->IsBound())
        {
            result |= pTextureRef->Bind(pFile);
        }
    }

    return result;
}

bool ResTexPatternAnim::ForceBind(const ResTexture* pTexture, const char* name)
{
    NW_G3D_ASSERT_NOT_NULL(pTexture);

    bool found = false;

    for (int idxTex = 0, numTex = GetTextureRefCount(); idxTex < numTex; ++idxTex)
    {
        ResTextureRef* pTextureRef = GetTextureRef(idxTex);
        if (std::strcmp(pTextureRef->GetName(), name) == 0)
        {
            pTextureRef->ForceBind(pTexture);
            found = true;
        }
    }

    return found;
}

} } } // namespace nw::g3d::res
