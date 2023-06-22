#include <nw/g3d/g3d_ShapeAnimObj.h>
#include <algorithm>

#include <nw/g3d/g3d_ModelObj.h>

namespace nw { namespace g3d {

void ShapeAnimObj::Sizer::Calc(const InitArg& arg)
{
    NW_G3D_ASSERT(arg.IsValid());

    int numBind = std::max(arg.GetMaxShapeCount(), arg.GetMaxVertexShapeAnimCount());
    int numCurve = arg.GetMaxCurveCount();
    int numKeyShapeAnim = arg.GetMaxKeyShapeAnimCount();

    int idx = 0;
    chunk[idx++].size = Align(sizeof(float) * numKeyShapeAnim);
    chunk[idx++].size = sizeof(bit32) * numBind;
    chunk[idx++].size = Align(sizeof(s8) * numKeyShapeAnim);
    chunk[idx++].size = arg.IsContextEnabled() ? Align(sizeof(AnimFrameCache) * numCurve) : 0;
    NW_G3D_ASSERT(idx == NUM_CHUNK);

    CalcOffset(chunk, NUM_CHUNK);
}

size_t ShapeAnimObj::CalcBufferSize(const InitArg& arg)
{
    Sizer& sizer = arg.GetSizer();
    sizer.Calc(arg);
    return sizer.GetTotalSize();
}

bool ShapeAnimObj::Init(const InitArg& arg, void* pBuffer, size_t bufferSize)
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

    int numBind = std::max(arg.GetMaxShapeCount(), arg.GetMaxVertexShapeAnimCount());
    int numCurve = arg.GetMaxCurveCount();

    void* ptr = pBuffer;
    SetBufferPtr(pBuffer);
    m_pRes = NULL;
    GetBindTable().Init(sizer.GetPtr<bit32>(ptr, Sizer::BIND_TABLE), numBind);
    GetContext().Init(sizer.GetPtr<AnimFrameCache>(ptr, Sizer::FRAMECACHE_ARRAY), numCurve);
    SetResultBuffer(sizer.GetPtr(ptr, Sizer::RESULT_BUFFER));
    m_MaxVtxShpAnim = arg.GetMaxVertexShapeAnimCount();
    m_MaxSubBind = arg.GetMaxKeyShapeAnimCount();
    m_pSubBindIndexArray = sizer.GetPtr<s8>(ptr, Sizer::SUBBIND_TABLE);

    return true;
}

void ShapeAnimObj::SetResource(ResShapeAnim* pRes)
{
    NW_G3D_ASSERT(IsAcceptable(pRes));

    m_pRes = pRes;
    m_pVertexShapeAnimArray = pRes->ref().ofsVertexShapeAnimArray.to_ptr<ResVertexShapeAnimData>();

    SetTargetUnbound();

    bool loop = (pRes->ref().flag & AnimFlag::PLAYPOLICY_LOOP) != 0;
    ResetFrameCtrl(pRes->GetFrameCount(), loop);
    GetBindTable().SetAnimCount(pRes->GetVertexShapeAnimCount());
    GetContext().SetCurveCount(pRes->GetCurveCount());
}

BindResult ShapeAnimObj::Bind(const ResModel* pModel)
{
    NW_G3D_ASSERT(IsAcceptable(pModel));

    BindResult result;
    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(pModel->GetShapeCount());

    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        ResVertexShapeAnim* pVertexShapeAnim = GetVertexShapeAnim(idxAnim);
        const ResName* pName = pVertexShapeAnim->ref().ofsName.GetResName();
        int idxTarget = pModel->GetShapeIndex(pName);
        if (idxTarget >= 0)
        {
            const ResShape* pShape = pModel->GetShape(idxTarget);
            BindResult subResult = SubBind(pVertexShapeAnim, pShape);
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
    ShapeAnimObj::ClearResult();
    return result;
}

BindResult ShapeAnimObj::Bind(const ModelObj* pModel)
{
    NW_G3D_ASSERT_NOT_NULL(pModel);
    NW_G3D_ASSERT(IsAcceptable(pModel->GetResource()));

    BindResult result;
    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(pModel->GetShapeCount());

    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        ResVertexShapeAnim* pVertexShapeAnim = GetVertexShapeAnim(idxAnim);
        const ResName* pName = pVertexShapeAnim->ref().ofsName.GetResName();
        int idxTarget = pModel->GetResource()->GetShapeIndex(pName);
        if (idxTarget >= 0)
        {
            const ResShape* pShape = pModel->GetShape(idxTarget)->GetResource();
            BindResult subResult = SubBind(pVertexShapeAnim, pShape);
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
    ShapeAnimObj::ClearResult();
    return result;
}

void ShapeAnimObj::BindFast(const ResModel* pTarget)
{
    NW_G3D_ASSERT(IsAcceptable(pTarget));
    NW_G3D_ASSERT(pTarget == m_pRes->GetBindModel());

    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(pTarget->GetShapeCount());
    bindTable.BindAll(m_pRes->ref().ofsBindIndexArray.to_ptr<u16>());

    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        int idxTarget = bindTable.GetTargetIndex(idxAnim);
        if (idxTarget < AnimBindTable::NOT_BOUND)
        {
            ResVertexShapeAnim* pVertexShapeAnim = GetVertexShapeAnim(idxAnim);
            BindResult result = SubBindFast(pVertexShapeAnim);
            if (result.IsMissed())
            {
                bindTable.Unbind(idxAnim, idxTarget);
            }
        }
    }

    SetTargetBound();
    ShapeAnimObj::ClearResult();
}

BindResult ShapeAnimObj::SubBind(
    const ResVertexShapeAnim* pVertexShapeAnim, const ResShape* pShape)
{
    BindResult result;
    const ResVertexShapeAnim::KeyShapeAnimInfo* pInfo =
        pVertexShapeAnim->ref().ofsKeyShapeAnimInfoArray.to_ptr<
        const ResVertexShapeAnim::KeyShapeAnimInfo>();
    int beginKeyShapeAnim = pVertexShapeAnim->ref().beginKeyShapeAnim;

    for (int idxKeyShapeAnim = 0, numKeyShapeAnim = pVertexShapeAnim->GetKeyShapeAnimCount();
        idxKeyShapeAnim < numKeyShapeAnim; ++idxKeyShapeAnim, ++pInfo)
    {
        const ResName* pName = pInfo->ofsName.GetResName();
        int idxTarget = pShape->GetKeyShapeIndex(pName);
        m_pSubBindIndexArray[beginKeyShapeAnim + idxKeyShapeAnim] = static_cast<s8>(idxTarget);
        result |= idxTarget >= 0 ? BindResult::Bound() : BindResult::NotBound();
    }

    return result;
}

BindResult ShapeAnimObj::SubBindFast(const ResVertexShapeAnim* pVertexShapeAnim)
{
    BindResult result;
    const ResVertexShapeAnim::KeyShapeAnimInfo* pInfo =
        pVertexShapeAnim->ref().ofsKeyShapeAnimInfoArray.to_ptr<
        const ResVertexShapeAnim::KeyShapeAnimInfo>();
    int beginKeyShapeAnim = pVertexShapeAnim->ref().beginKeyShapeAnim;

    for (int idxKeyShapeAnim = 0, numKeyShapeAnim = pVertexShapeAnim->GetKeyShapeAnimCount();
        idxKeyShapeAnim < numKeyShapeAnim; ++idxKeyShapeAnim, ++pInfo)
    {
        m_pSubBindIndexArray[beginKeyShapeAnim + idxKeyShapeAnim] = pInfo->subbindIndex;
        result |= pInfo->subbindIndex == -1 ? BindResult::NotBound() : BindResult::Bound();
    }

    return result;
}

void ShapeAnimObj::ClearResult()
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);

    for (int idxAnim = 0, numAnim = GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        ResVertexShapeAnim* pVertexShapeAnim = GetVertexShapeAnim(idxAnim);
        int beginKeyShapeAnim = pVertexShapeAnim->ref().beginKeyShapeAnim;
        float* pResult = AddOffset<float>(GetResultBuffer(), sizeof(float) * beginKeyShapeAnim);
        pVertexShapeAnim->Init(pResult);
    }
}

void ShapeAnimObj::Calc()
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);
    NW_G3D_ASSERT(IsTargetBound());

    if (IsFrameChanged())
    {
        float frame = GetFrameCtrl().GetFrame();
        const AnimBindTable& bindTable = GetBindTable();
        AnimContext& context = GetContext();
        int numAnim = bindTable.GetAnimCount();
        float* pResultArray = static_cast<float*>(GetResultBuffer());
        if (context.IsFrameCacheValid())
        {
            for (int idxAnim = 0; idxAnim < numAnim; ++idxAnim)
            {
                const ResVertexShapeAnim* pVertexShapeAnim = GetVertexShapeAnim(idxAnim);
                int beginKeyShapeAnim = pVertexShapeAnim->ref().beginKeyShapeAnim;
                int beginCurve = pVertexShapeAnim->ref().beginCurve;

                if (!bindTable.IsCalcEnabled(idxAnim))
                {
                    continue;
                }

                float* pResult = &pResultArray[beginKeyShapeAnim];
                pVertexShapeAnim->Eval(pResult, frame, &m_pSubBindIndexArray[beginKeyShapeAnim],
                    context.GetFrameCacheArray(beginCurve));
            }
        }
        else
        {
            for (int idxAnim = 0; idxAnim < numAnim; ++idxAnim)
            {
                const ResVertexShapeAnim* pVertexShapeAnim = GetVertexShapeAnim(idxAnim);
                int beginKeyShapeAnim = pVertexShapeAnim->ref().beginKeyShapeAnim;

                if (!bindTable.IsCalcEnabled(idxAnim))
                {
                    continue;
                }

                float* pResult = &pResultArray[beginKeyShapeAnim];
                pVertexShapeAnim->Eval(pResult, frame, &m_pSubBindIndexArray[beginKeyShapeAnim]);
            }
        }

        UpdateLastFrame();
    }
}

void ShapeAnimObj::ApplyTo(ModelObj* pModelObj) const
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);
    NW_G3D_ASSERT(IsTargetBound());
    NW_G3D_ASSERT_NOT_NULL(pModelObj);

    const AnimBindTable& bindTable = GetBindTable();
    for (int idxAnim = 0, numAnim = GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        if (!bindTable.IsApplyEnabled(idxAnim))
        {
            continue;
        }

        int idxShape = bindTable.GetTargetIndex(idxAnim);
        ShapeObj* pShape = pModelObj->GetShape(idxShape);
        ApplyTo(pShape, idxAnim);
    }
}

void ShapeAnimObj::ApplyTo(ShapeObj* pShapeObj, int animIndex) const
{
    const ResVertexShapeAnim* pVertexShapeAnim = GetVertexShapeAnim(animIndex);
    int beginKeyShapeAnim = pVertexShapeAnim->ref().beginKeyShapeAnim;
    const float* pResult = AddOffset<float>(GetResultBuffer(), sizeof(float) * beginKeyShapeAnim);

    const ResVertexShapeAnim::KeyShapeAnimInfo* pInfo =
        pVertexShapeAnim->ref().ofsKeyShapeAnimInfoArray.to_ptr<
        ResVertexShapeAnim::KeyShapeAnimInfo>();
    float baseWeight = 1.0f;
    for (int idxKeyShapeAnim = 1, numKeyShapeAnim = pVertexShapeAnim->GetKeyShapeAnimCount();
        idxKeyShapeAnim < numKeyShapeAnim; ++idxKeyShapeAnim, ++pInfo)
    {
        int subBindIndex = m_pSubBindIndexArray[beginKeyShapeAnim + idxKeyShapeAnim];
        if (subBindIndex == -1)
        {
            continue;
        }

        float weight = pResult[idxKeyShapeAnim];
        baseWeight -= weight;
        pShapeObj->SetBlendWeight(subBindIndex, weight);
    }

    int baseIndex = m_pSubBindIndexArray[beginKeyShapeAnim];
    NW_G3D_ASSERT(baseIndex >= 0);
    pShapeObj->SetBlendWeight(baseIndex, baseWeight);
}

} } // namespace nw::g3d
