#include <nw/g3d/g3d_ShaderParamAnimObj.h>
#include <algorithm>

#include <nw/g3d/g3d_ModelObj.h>

namespace nw { namespace g3d {

void ShaderParamAnimObj::Sizer::Calc(const InitArg& arg)
{
    NW_G3D_ASSERT(arg.IsValid());

    int numBind = std::max(arg.GetMaxMatCount(), arg.GetMaxMatAnimCount());
    int numCurve = arg.GetMaxCurveCount();

    int idx = 0;
    chunk[idx++].size = sizeof(bit32) * numCurve;
    chunk[idx++].size = sizeof(bit32) * numBind;
    chunk[idx++].size = Align(sizeof(u16) * arg.GetMaxParamAnimCount());
    chunk[idx++].size = arg.IsContextEnabled() ? Align(sizeof(AnimFrameCache) * numCurve) : 0;
    NW_G3D_ASSERT(idx == NUM_CHUNK);

    CalcOffset(chunk, NUM_CHUNK);
}

size_t ShaderParamAnimObj::CalcBufferSize(const InitArg& arg)
{
    Sizer& sizer = arg.GetSizer();
    sizer.Calc(arg);
    return sizer.GetTotalSize();
}

bool ShaderParamAnimObj::Init(const InitArg& arg, void* pBuffer, size_t bufferSize)
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
    m_MaxSubBind = arg.GetMaxParamAnimCount();
    m_pSubBindIndexArray = sizer.GetPtr<u16>(ptr, Sizer::SUBBIND_TABLE);

    return true;
}

void ShaderParamAnimObj::SetResource(ResShaderParamAnim* pRes)
{
    NW_G3D_ASSERT(IsAcceptable(pRes));

    m_pRes = pRes;
    m_pMatAnimArray = pRes->ref().ofsMatAnimArray.to_ptr<ResShaderParamMatAnimData>();

    SetTargetUnbound();

    bool loop = (pRes->ref().flag & AnimFlag::PLAYPOLICY_LOOP) != 0;
    ResetFrameCtrl(pRes->GetFrameCount(), loop);
    GetBindTable().SetAnimCount(pRes->GetMatAnimCount());
    GetContext().SetCurveCount(pRes->GetCurveCount());
}

BindResult ShaderParamAnimObj::Bind(const ResModel* pModel)
{
    NW_G3D_ASSERT(IsAcceptable(pModel));

    BindResult result;
    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(pModel->GetMaterialCount());

    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        ResShaderParamMatAnim* pMatAnim = GetMatAnim(idxAnim);
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
    return result;
}

BindResult ShaderParamAnimObj::Bind(const ModelObj* pModel)
{
    NW_G3D_ASSERT_NOT_NULL(pModel);
    NW_G3D_ASSERT(IsAcceptable(pModel->GetResource()));

    BindResult result;
    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(pModel->GetMaterialCount());

    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        ResShaderParamMatAnim* pMatAnim = GetMatAnim(idxAnim);
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
    return result;
}

void ShaderParamAnimObj::BindFast(const ResModel* pModel)
{
    NW_G3D_ASSERT(IsAcceptable(pModel));
    NW_G3D_ASSERT(pModel == m_pRes->GetBindModel());

    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(pModel->GetMaterialCount());
    bindTable.BindAll(m_pRes->ref().ofsBindIndexArray.to_ptr<u16>());

    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        int idxTarget = bindTable.GetTargetIndex(idxAnim);
        if (idxTarget < AnimBindTable::NOT_BOUND)
        {
            ResShaderParamMatAnim* pMatAnim = GetMatAnim(idxAnim);
            BindResult result = SubBindFast(pMatAnim);
            if (result.IsMissed())
            {
                bindTable.Unbind(idxAnim, idxTarget);
            }
        }
    }

    SetTargetBound();
}

BindResult ShaderParamAnimObj::SubBind(
    const ResShaderParamMatAnim* pMatAnim, const ResMaterial* pMaterial)
{
    BindResult result;
    const ResShaderParamMatAnim::ParamAnimInfo* pInfo =
        pMatAnim->ref().ofsParamAnimInfoArray.to_ptr<const ResShaderParamMatAnim::ParamAnimInfo>();
    int beginParamAnim = pMatAnim->ref().beginParamAnim;

    for (int idxParamAnim = 0, numParamAnim = pMatAnim->GetParamAnimCount();
        idxParamAnim < numParamAnim; ++idxParamAnim, ++pInfo)
    {
        const ResName* pName = pInfo->ofsName.GetResName();
        int idxTarget = pMaterial->GetShaderParamIndex(pName);
        m_pSubBindIndexArray[beginParamAnim + idxParamAnim] = static_cast<u16>(idxTarget);
        result |= idxTarget >= 0 ? BindResult::Bound() : BindResult::NotBound();
    }

    return result;
}

BindResult ShaderParamAnimObj::SubBindFast(const ResShaderParamMatAnim* pMatAnim)
{
    BindResult result;
    const ResShaderParamMatAnim::ParamAnimInfo* pInfo =
        pMatAnim->ref().ofsParamAnimInfoArray.to_ptr<const ResShaderParamMatAnim::ParamAnimInfo>();
    int beginParamAnim = pMatAnim->ref().beginParamAnim;

    for (int idxParamAnim = 0, numParamAnim = pMatAnim->GetParamAnimCount();
        idxParamAnim < numParamAnim; ++idxParamAnim, ++pInfo)
    {
        m_pSubBindIndexArray[beginParamAnim + idxParamAnim] = pInfo->subbindIndex;
        result |= pInfo->subbindIndex == AnimFlag::NOT_BOUND ?
            BindResult::NotBound() : BindResult::Bound();
    }

    return result;
}

void ShaderParamAnimObj::Calc()
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
            for (int idxMatAnim = 0; idxMatAnim < numMatAnim; ++idxMatAnim)
            {
                const ResShaderParamMatAnim* pMatAnim = GetMatAnim(idxMatAnim);
                int beginParamAnim = pMatAnim->ref().beginParamAnim;
                int beginCurve = pMatAnim->ref().beginCurve;

                if (!bindTable.IsCalcEnabled(idxMatAnim))
                {
                    continue;
                }

                void* pResult = AddOffset(GetResultBuffer(), sizeof(float) * beginCurve);
                pMatAnim->Eval(pResult, frame, &m_pSubBindIndexArray[beginParamAnim],
                    context.GetFrameCacheArray(beginCurve));
            }
        }
        else
        {
            for (int idxMatAnim = 0; idxMatAnim < numMatAnim; ++idxMatAnim)
            {
                const ResShaderParamMatAnim* pMatAnim = GetMatAnim(idxMatAnim);
                int beginParamAnim = pMatAnim->ref().beginParamAnim;
                int beginCurve = pMatAnim->ref().beginCurve;

                if (!bindTable.IsCalcEnabled(idxMatAnim))
                {
                    continue;
                }

                void* pResult = AddOffset(GetResultBuffer(), sizeof(float) * beginCurve);
                pMatAnim->Eval(pResult, frame, &m_pSubBindIndexArray[beginParamAnim]);
            }
        }

        UpdateLastFrame();
    }
}

void ShaderParamAnimObj::ApplyTo(ModelObj* pModelObj) const
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);
    NW_G3D_ASSERT(IsTargetBound());
    NW_G3D_ASSERT_NOT_NULL(pModelObj);

    const AnimBindTable& bindTable = GetBindTable();
    for (int idxMatAnim = 0, numMatAnim = bindTable.GetAnimCount();
        idxMatAnim < numMatAnim; ++idxMatAnim)
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

void ShaderParamAnimObj::ApplyTo(MaterialObj* pMaterialObj, int animIndex) const
{
    const ResShaderParamMatAnim* pMatAnim = GetMatAnim(animIndex);
    const bit32* pResult = AddOffset<bit32>(GetResultBuffer(),
        sizeof(float) * pMatAnim->ref().beginCurve);

    const ResShaderParamMatAnim::ParamAnimInfo* pInfo =
        pMatAnim->ref().ofsParamAnimInfoArray.to_ptr<ResShaderParamMatAnim::ParamAnimInfo>();
    for (int idxParamAnim = pMatAnim->ref().beginParamAnim,
        endParamAnim = idxParamAnim + pMatAnim->GetParamAnimCount();
        idxParamAnim < endParamAnim; ++idxParamAnim, ++pInfo)
    {
        int subBindIndex = m_pSubBindIndexArray[idxParamAnim];
        if (subBindIndex == AnimFlag::NOT_BOUND)
        {
            continue;
        }

        void* pSrcParam = pMaterialObj->EditShaderParam(subBindIndex);

        for (int idxCurve = pInfo->beginCurve,
            endCurve = idxCurve + pInfo->numFloatCurve + pInfo->numIntCurve;
            idxCurve < endCurve; ++idxCurve)
        {
            const ResAnimCurve* pCurve = pMatAnim->GetCurve(idxCurve);
            u32 targetOffset = pCurve->ref().targetOffset;
            bit32* pTarget = AddOffset<bit32>(pSrcParam, targetOffset);
            *pTarget = pResult[idxCurve];
        }

        for (int idxConstant = pInfo->beginConstant,
            endConstant = idxConstant + pInfo->numConstant;
            idxConstant < endConstant; ++idxConstant)
        {
            const ResAnimConstant* pConstant = pMatAnim->GetConstant(idxConstant);
            u32 targetOffset = pConstant->ref().targetOffset;
            s32* pTarget = AddOffset<s32>(pSrcParam, targetOffset);
            *pTarget = pConstant->GetInt();
        }
    }
}

} } // namespace nw::g3d
