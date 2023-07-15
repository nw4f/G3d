#include <nw/g3d/g3d_VisibilityAnimObj.h>
#include <algorithm>
#include <nw/g3d/ut/g3d_Flag.h>

#include <nw/g3d/g3d_ModelObj.h>

namespace nw { namespace g3d {

void VisibilityAnimObj::Sizer::Calc(const InitArg& arg)
{
    NW_G3D_ASSERT(arg.IsValid());

    int numTarget = 0;
    int numAnim = 0;
    if (arg.GetMaxBoneAnimCount() > 0)
    {
        numTarget = arg.GetMaxBoneCount();
        numAnim = arg.GetMaxBoneAnimCount();
    }
    if (arg.GetMaxMatAnimCount() > 0)
    {
        numTarget = std::max(numTarget, arg.GetMaxMatCount());
        numAnim = std::max(numAnim, arg.GetMaxMatAnimCount());
    }
    int numBind = std::max(numTarget, numAnim);
    int numCurve = arg.GetMaxCurveCount();

    int idx = 0;
    chunk[idx++].size = Align(numAnim, 32) >> 3;
    chunk[idx++].size = sizeof(bit32) * numBind;
    chunk[idx++].size = arg.IsContextEnabled() ? Align(sizeof(AnimFrameCache) * numCurve) : 0;
    NW_G3D_ASSERT(idx == NUM_CHUNK);

    CalcOffset(chunk, NUM_CHUNK);
}

size_t VisibilityAnimObj::CalcBufferSize(const InitArg& arg)
{
    Sizer& sizer = arg.GetSizer();
    sizer.Calc(arg);
    return sizer.GetTotalSize();
}

bool VisibilityAnimObj::Init(const InitArg& arg, void* pBuffer, size_t bufferSize)
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

    int numTarget = 0;
    int numAnim = 0;
    if (arg.GetMaxBoneAnimCount() > 0)
    {
        numTarget = arg.GetMaxBoneCount();
        numAnim = arg.GetMaxBoneAnimCount();
    }
    if (arg.GetMaxMatAnimCount() > 0)
    {
        numTarget = std::max(numTarget, arg.GetMaxMatCount());
        numAnim = std::max(numAnim, arg.GetMaxMatAnimCount());
    }
    int numBind = std::max(numTarget, numAnim);
    int numCurve = arg.GetMaxCurveCount();

    void* ptr = pBuffer;
    SetBufferPtr(pBuffer);
    m_pRes = NULL;
    GetBindTable().Init(sizer.GetPtr<bit32>(ptr, Sizer::BIND_TABLE), numBind);
    GetContext().Init(sizer.GetPtr<AnimFrameCache>(ptr, Sizer::FRAMECACHE_ARRAY), numCurve);
    SetResultBuffer(sizer.GetPtr(ptr, Sizer::RESULT_BUFFER));
    m_MaxBoneAnim = static_cast<u16>(arg.GetMaxBoneAnimCount());
    m_MaxMatAnim = static_cast<u16>(arg.GetMaxMatAnimCount());

    return true;
}

void VisibilityAnimObj::SetResource(ResVisibilityAnim* pRes)
{
    NW_G3D_ASSERT(IsAcceptable(pRes));

    m_pRes = pRes;
    m_pCurveArray = pRes->ref().ofsCurveArray.to_ptr<ResAnimCurveData>();
    m_NumCurve = pRes->GetCurveCount();

    SetTargetUnbound();

    bool loop = (pRes->ref().flag & AnimFlag::PLAYPOLICY_LOOP) != 0;
    ResetFrameCtrl(pRes->GetFrameCount(), loop);
    GetBindTable().SetAnimCount(pRes->GetAnimCount());
    GetContext().SetCurveCount(pRes->GetCurveCount());
}

BindResult VisibilityAnimObj::Bind(const ResModel* pModel)
{
    NW_G3D_ASSERT(IsAcceptable(pModel));

    const ResDicType* pDic = NULL;
    int count = 0;
    if (m_pRes->GetVisibilityType() == ResVisibilityAnim::BONE_VISIBILITY)
    {
        const ResSkeleton* pSkeleton = pModel->GetSkeleton();
        pDic = NW_G3D_RES_DIC(pSkeleton->ref().ofsBoneDic);
        count = pSkeleton->GetBoneCount();
    }
    else
    {
        pDic = NW_G3D_RES_DIC(pModel->ref().ofsMaterialDic);
        count = pModel->GetMaterialCount();
    }
    NW_G3D_ASSERT(count <= GetBindTable().GetSize());

    BindResult result;
    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(count);
    const BinString* pNameArray = m_pRes->ref().ofsNameArray.to_ptr<BinString>();
    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        const ResName* pName = pNameArray[idxAnim].GetResName();
        int idxTarget = ResDicType::FindIndex(pDic, pName);
        if (idxTarget >= 0)
        {
            bindTable.Bind(idxAnim, idxTarget);
            result |= BindResult::Bound();
        }
        else
        {
            result |= BindResult::NotBound();
        }
    }

    SetTargetBound();
    VisibilityAnimObj::ClearResult();
    return result;
}

BindResult VisibilityAnimObj::Bind(const ModelObj* pModelObj)
{
    NW_G3D_ASSERT_NOT_NULL(pModelObj);
    return Bind(pModelObj->GetResource());
}

void VisibilityAnimObj::BindFast(const ResModel* pModel)
{
    NW_G3D_ASSERT(IsAcceptable(pModel));
    NW_G3D_ASSERT(pModel == m_pRes->GetBindModel());
    int count = m_pRes->GetVisibilityType() == ResVisibilityAnim::BONE_VISIBILITY ?
        pModel->GetSkeleton()->GetBoneCount() : pModel->GetMaterialCount();

    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(count);
    bindTable.BindAll(m_pRes->ref().ofsBindIndexArray.to_ptr<u16>());

    SetTargetBound();
    VisibilityAnimObj::ClearResult();
}

void VisibilityAnimObj::ClearResult()
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);

    int sizeResult = Align(GetAnimCount(), 32) >> 3;
    memcpy(GetResultBuffer(), m_pRes->ref().ofsBaseValueArray.to_ptr(), sizeResult);
}

void VisibilityAnimObj::Calc()
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);
    NW_G3D_ASSERT(IsTargetBound());

    if (IsFrameChanged())
    {
        float frame = GetFrameCtrl().GetFrame();
        const AnimBindTable& bindTable = GetBindTable();
        AnimContext& context = GetContext();
        bit32* pResultBuffer = static_cast<bit32*>(GetResultBuffer());
        if (context.IsFrameCacheValid())
        {
            for (int idxCurve = 0; idxCurve < m_NumCurve; ++idxCurve)
            {
                const ResAnimCurve* pCurve = GetCurve(idxCurve);
                int idxAnim = static_cast<int>(pCurve->ref().targetOffset);

                if (!bindTable.IsCalcEnabled(idxAnim))
                {
                    continue;
                }

                int visible = pCurve->EvalInt(frame, context.GetFrameCacheArray(idxCurve));
                SetBit(pResultBuffer, idxAnim, static_cast<bit32>(visible));
            }
        }
        else
        {
            for (int idxCurve = 0; idxCurve < m_NumCurve; ++idxCurve)
            {
                const ResAnimCurve* pCurve = GetCurve(idxCurve);
                int idxAnim = static_cast<int>(pCurve->ref().targetOffset);

                if (!bindTable.IsCalcEnabled(idxAnim))
                {
                    continue;
                }

                int visible = pCurve->EvalInt(frame);
                SetBit(pResultBuffer, idxAnim, static_cast<bit32>(visible));
            }
        }
        UpdateLastFrame();
    }
}

void VisibilityAnimObj::ApplyTo(ModelObj* pModelObj) const
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);
    NW_G3D_ASSERT(IsTargetBound());
    NW_G3D_ASSERT_NOT_NULL(pModelObj);

    bit32* pVisArray = m_pRes->GetVisibilityType() == ResVisibilityAnim::BONE_VISIBILITY ?
        pModelObj->GetBoneVisArray() : pModelObj->GetMatVisArray();
    const bit32* pResultBuffer = static_cast<const bit32*>(GetResultBuffer());
    const AnimBindTable& bindTable = GetBindTable();
    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        if (!bindTable.IsApplyEnabled(idxAnim))
        {
            continue;
        }

        int idxTarget = bindTable.GetTargetIndex(idxAnim);
        bit32 visible = IsBitOn<bit32>(pResultBuffer, idxAnim);
        SetBit(pVisArray, idxTarget, visible);
    }
}

} } // namespace nw::g3d
