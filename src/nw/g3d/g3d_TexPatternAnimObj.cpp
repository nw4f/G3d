#include <nw/g3d/g3d_TexPatternAnimObj.h>
#include <algorithm>

#include <nw/g3d/g3d_ModelObj.h>

namespace nw { namespace g3d {

void TexPatternAnimObj::Sizer::Calc(const InitArg& arg)
{
    NW_G3D_ASSERT(arg.IsValid());

    int numBind = std::max(arg.GetMaxMatCount(), arg.GetMaxMatAnimCount());
    int numCurve = arg.GetMaxCurveCount();
    int numPatAnim = arg.GetMaxPatAnimCount();

    int idx = 0;
    chunk[idx++].size = Align(sizeof(u16) * numPatAnim);
    chunk[idx++].size = sizeof(bit32) * numBind;
    chunk[idx++].size = Align(sizeof(s8) * numPatAnim);
    chunk[idx++].size = arg.IsContextEnabled() ? Align(sizeof(AnimFrameCache) * numCurve) : 0;

    NW_G3D_ASSERT(idx == NUM_CHUNK);

    CalcOffset(chunk, NUM_CHUNK);
}

size_t TexPatternAnimObj::CalcBufferSize(const InitArg& arg)
{
    Sizer& sizer = arg.GetSizer();
    sizer.Calc(arg);
    return sizer.GetTotalSize();
}

bool TexPatternAnimObj::Init(const InitArg& arg, void* pBuffer, size_t bufferSize)
{
    NW_G3D_ASSERT(bufferSize == 0 || pBuffer);
#if NW_G3D_IS_HOST_CAFE
    NW_G3D_WARNING(IsAligned(pBuffer, BUFFER_ALIGNMENT), "pBuffer must be aligned.");
#endif // NW_G3D_IS_HOST_CAFE

    Sizer& sizer = arg.GetSizer();
    if (!sizer.IsValid())
    {
        sizer.Calc(arg);
    }
    if (sizer.GetTotalSize() > bufferSize)
    {
        return false;
    }

    int numBind = std::max(arg.GetMaxMatCount(), arg.GetMaxMatAnimCount());
    int numCurve = arg.GetMaxCurveCount();

    void* ptr = pBuffer;
    SetBufferPtr(pBuffer);
    m_pRes = NULL;
    GetBindTable().Init(sizer.GetPtr<bit32>(ptr, Sizer::BIND_TABLE), numBind);
    GetContext().Init(sizer.GetPtr<AnimFrameCache>(ptr, Sizer::FRAMECACHE_ARRAY), numCurve);
    SetResultBuffer(sizer.GetPtr(ptr, Sizer::RESULT_BUFFER));
    m_MaxMatAnim = arg.GetMaxMatAnimCount();
    m_MaxSubBind = arg.GetMaxPatAnimCount();

    m_pSubBindIndexArray = sizer.GetPtr<s8>(ptr, Sizer::SUBBIND_TABLE);

    return true;
}

void TexPatternAnimObj::SetResource(ResTexPatternAnim* pRes)
{
    NW_G3D_ASSERT(IsAcceptable(pRes));

    m_pRes = pRes;
    m_pMatAnimArray = pRes->ref().ofsMatAnimArray.to_ptr<ResTexPatternMatAnimData>();

    SetTargetUnbound();

    bool loop = (pRes->ref().flag & AnimFlag::PLAYPOLICY_LOOP) != 0;
    ResetFrameCtrl(pRes->GetFrameCount(), loop);
    GetBindTable().SetAnimCount(pRes->GetMatAnimCount());
    GetContext().SetCurveCount(pRes->GetCurveCount());
}

BindResult TexPatternAnimObj::Bind(const ResModel* pModel)
{
    NW_G3D_ASSERT(IsAcceptable(pModel));

    BindResult result;
    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(pModel->GetMaterialCount());

    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        ResTexPatternMatAnim* pMatAnim = GetMatAnim(idxAnim);
        const ResName* pName = pMatAnim->ref().ofsName.GetResName();
        int idxTarget = pModel->GetMaterialIndex(pName);
        if (idxTarget >= 0)
        {
            const ResMaterial* pMaterial = pModel->GetMaterial(idxTarget);
            BindResult subResult = SubBind(pMatAnim, pMaterial);
            if (!subResult.IsMissed())
            {
                bindTable.Bind(idxAnim, idxTarget);
                result |= BindResult::Bound();
                continue;
            }
        }
        result |= BindResult::NotBound();
    }

    SetTargetBound();
    TexPatternAnimObj::ClearResult();
    return result;
}

BindResult TexPatternAnimObj::Bind(const ModelObj* pModel)
{
    NW_G3D_ASSERT_NOT_NULL(pModel);
    NW_G3D_ASSERT(IsAcceptable(pModel->GetResource()));

    BindResult result;
    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(pModel->GetMaterialCount());

    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        ResTexPatternMatAnim* pMatAnim = GetMatAnim(idxAnim);
        const ResName* pName = pMatAnim->ref().ofsName.GetResName();
        int idxTarget = pModel->GetResource()->GetMaterialIndex(pName);
        if (idxTarget >= 0)
        {
            const ResMaterial* pMaterial = pModel->GetMaterial(idxTarget)->GetResource();
            BindResult subResult = SubBind(pMatAnim, pMaterial);
            if (!subResult.IsMissed())
            {
                bindTable.Bind(idxAnim, idxTarget);
                result |= BindResult::Bound();
                continue;
            }
        }
        result |= BindResult::NotBound();
    }

    SetTargetBound();
    TexPatternAnimObj::ClearResult();
    return result;
}

void TexPatternAnimObj::BindFast(const ResModel* pTarget)
{
    NW_G3D_ASSERT(IsAcceptable(pTarget));
    NW_G3D_ASSERT(pTarget == m_pRes->GetBindModel());

    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(pTarget->GetMaterialCount());
    bindTable.BindAll(m_pRes->ref().ofsBindIndexArray.to_ptr<u16>());

    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        int idxTarget = bindTable.GetTargetIndex(idxAnim);
        if (idxTarget < AnimBindTable::NOT_BOUND)
        {
            ResTexPatternMatAnim* pMatAnim = GetMatAnim(idxAnim);
            BindResult result = SubBindFast(pMatAnim);
            if (result.IsMissed())
            {
                bindTable.Unbind(idxAnim, idxTarget);
            }
        }
    }

    SetTargetBound();
    TexPatternAnimObj::ClearResult();
}

BindResult TexPatternAnimObj::SubBind(
    const ResTexPatternMatAnim* pMatAnim, const ResMaterial* pMaterial)
{
    BindResult result;
    const ResTexPatternMatAnim::PatAnimInfo* pInfo =
        pMatAnim->ref().ofsPatAnimInfoArray.to_ptr<const ResTexPatternMatAnim::PatAnimInfo>();
    int beginPatAnim = pMatAnim->ref().beginPatAnim;

    for (int idxPatAnim = 0, numPatAnim = pMatAnim->GetPatAnimCount();
        idxPatAnim < numPatAnim; ++idxPatAnim, ++pInfo)
    {
        const ResName* pName = pInfo->ofsName.GetResName();
        int idxTarget = pMaterial->GetSamplerIndex(pName);
        m_pSubBindIndexArray[beginPatAnim + idxPatAnim] = static_cast<s8>(idxTarget);
        result |= idxTarget >= 0 ? BindResult::Bound() : BindResult::NotBound();
    }

    return result;
}

BindResult TexPatternAnimObj::SubBindFast(const ResTexPatternMatAnim* pMatAnim)
{
    BindResult result;
    const ResTexPatternMatAnim::PatAnimInfo* pInfo =
        pMatAnim->ref().ofsPatAnimInfoArray.to_ptr<const ResTexPatternMatAnim::PatAnimInfo>();
    int beginPatAnim = pMatAnim->ref().beginPatAnim;

    for (int idxPatAnim = 0, numPatAnim = pMatAnim->GetPatAnimCount();
        idxPatAnim < numPatAnim; ++idxPatAnim, ++pInfo)
    {
        m_pSubBindIndexArray[beginPatAnim + idxPatAnim] = pInfo->subbindIndex;
        result |= pInfo->subbindIndex == -1 ?
            BindResult::NotBound() : BindResult::Bound();
    }

    return result;
}

void TexPatternAnimObj::ClearResult()
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);

    for (int idxAnim = 0, numAnim = GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        ResTexPatternMatAnim* pMatAnim = GetMatAnim(idxAnim);
        int beginPatAnim = pMatAnim->ref().beginPatAnim;
        u16* pResult = AddOffset<u16>(GetResultBuffer(), sizeof(u16) * beginPatAnim);
        pMatAnim->Init(pResult);
    }
}

void TexPatternAnimObj::Calc()
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);
    NW_G3D_ASSERT(IsTargetBound());

    if (IsFrameChanged())
    {
        float frame = GetFrameCtrl().GetFrame();
        const AnimBindTable& bindTable = GetBindTable();
        AnimContext& context = GetContext();
        int numMatAnim = bindTable.GetAnimCount();
        if (context.IsFrameCacheValid())
        {
            for (int idxPatAnim = 0; idxPatAnim < numMatAnim; ++idxPatAnim)
            {
                const ResTexPatternMatAnim* pMatAnim = GetMatAnim(idxPatAnim);
                int beginPatAnim = pMatAnim->ref().beginPatAnim;
                int beginCurve = pMatAnim->ref().beginCurve;

                if (!bindTable.IsCalcEnabled(idxPatAnim))
                {
                    continue;
                }

                u16* pResult = AddOffset<u16>(GetResultBuffer(), sizeof(u16) * beginPatAnim);
                pMatAnim->Eval(pResult, frame, &m_pSubBindIndexArray[beginPatAnim],
                    context.GetFrameCacheArray(beginCurve));
            }
        }
        else
        {
            for (int idxMatAnim = 0; idxMatAnim < numMatAnim; ++idxMatAnim)
            {
                const ResTexPatternMatAnim* pMatAnim = GetMatAnim(idxMatAnim);
                int beginPatAnim = pMatAnim->ref().beginPatAnim;

                if (!bindTable.IsCalcEnabled(idxMatAnim))
                {
                    continue;
                }

                u16* pResult = AddOffset<u16>(GetResultBuffer(), sizeof(u16) * beginPatAnim);
                pMatAnim->Eval(pResult, frame, &m_pSubBindIndexArray[beginPatAnim]);
            }
        }

        UpdateLastFrame();
    }
}

void TexPatternAnimObj::ApplyTo(ModelObj* pModelObj) const
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);
    NW_G3D_ASSERT(IsTargetBound());
    NW_G3D_ASSERT_NOT_NULL(pModelObj);

    const AnimBindTable& bindTable = GetBindTable();
    int numMatAnim = bindTable.GetAnimCount();
    for (int idxMatAnim = 0; idxMatAnim < numMatAnim; ++idxMatAnim)
    {
        if (!bindTable.IsApplyEnabled(idxMatAnim))
        {
            continue;
        }

        int idxMat = bindTable.GetTargetIndex(idxMatAnim);
        MaterialObj* pMaterial = pModelObj->GetMaterial(idxMat);
        ApplyTo(pMaterial, idxMatAnim);
    }
}

void TexPatternAnimObj::ApplyTo(MaterialObj* pMaterialObj, int animIndex) const
{
    const ResTexPatternMatAnim* pMatAnim = GetMatAnim(animIndex);
    int beginPatAnim = pMatAnim->ref().beginPatAnim;
    const u16* pResult = AddOffset<u16>(GetResultBuffer(), sizeof(u16) * beginPatAnim);

    const ResTexPatternMatAnim::PatAnimInfo* pInfo =
        pMatAnim->ref().ofsPatAnimInfoArray.to_ptr<ResTexPatternMatAnim::PatAnimInfo>();
    int numPatAnim = pMatAnim->GetPatAnimCount();
    for (int idxPatAnim = 0; idxPatAnim < numPatAnim; ++idxPatAnim, ++pInfo)
    {
        int subBindIndex = m_pSubBindIndexArray[beginPatAnim + idxPatAnim];
        if (subBindIndex == -1)
        {
            continue;
        }

        pMaterialObj->SetResTexture(subBindIndex, m_pRes->GetTextureRef(pResult[idxPatAnim])->Get());
    }
}

} } // namespace nw::g3d
