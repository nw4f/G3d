#include <nw/g3d/g3d_SkeletalAnimObj.h>
#include <algorithm>

#include <nw/g3d/math/g3d_MathCommon.h>
#include <nw/g3d/fnd/g3d_GfxManage.h>
#include <nw/g3d/g3d_SkeletonObj.h>
#include <nw/g3d/g3d_ModelObj.h>

#include <float.h>

namespace nw { namespace g3d {

namespace {

NW_G3D_INLINE
void CopySTAndFlag(LocalMtx& local, bit32 flag, const Vec3& scale, const Vec3& translate)
{
    local.flag &= ~ResBone::TRANSFORM_MASK;
    local.flag |= flag & ResBone::TRANSFORM_MASK;
    local.scale = scale;
    local.mtxRT.SetT(translate);
}

NW_G3D_INLINE
void BlendSTAndFlag(BoneAnimBlendResult* pTarget, const BoneAnimResult* pResult, float weight)
{
#if defined( __ghs__ )
    f32x2* pTargetScaleTrans = reinterpret_cast<f32x2*>(&pTarget->scale);
    const f32x2* pScaleTrans = reinterpret_cast<const f32x2*>(&pResult->scale);
    pTargetScaleTrans[0] = __PS_MADDS0F(pScaleTrans[0], weight, pTargetScaleTrans[0]);
    pTargetScaleTrans[1] = __PS_MADDS0F(pScaleTrans[1], weight, pTargetScaleTrans[1]);
    pTargetScaleTrans[2] = __PS_MADDS0F(pScaleTrans[2], weight, pTargetScaleTrans[2]);
#else
    Vec3 scale;
    scale.Mul(pResult->scale, weight);
    pTarget->scale.Add(pTarget->scale, scale);

    Vec3 translate;
    translate.Mul(pResult->translate, weight);
    pTarget->translate.Add(pTarget->translate, translate);
#endif

    pTarget->flag |= ~pResult->flag;
    pTarget->weight += weight;
}


struct LerpRot
{
    static void Blend(
        BoneAnimBlendResult* pTarget, const BoneAnimResult* pResult, float weight)
    {
        Vec3 axes[2] = { pResult->axes[0], pResult->axes[1] };

#if defined( __ghs__ )
        const f32x2* pAxes = reinterpret_cast<const f32x2*>(axes);
        f32x2* pTargetAxes = reinterpret_cast<f32x2*>(pTarget->axes);
        pTargetAxes[0] = __PS_MADDS0F(pAxes[0], weight, pTargetAxes[0]);
        pTargetAxes[1] = __PS_MADDS0F(pAxes[1], weight, pTargetAxes[1]);
        pTargetAxes[2] = __PS_MADDS0F(pAxes[2], weight, pTargetAxes[2]);
#else
        axes[0].Mul(axes[0], weight);
        pTarget->axes[0].Add(pTarget->axes[0], axes[0]);

        axes[1].Mul(axes[1], weight);
        pTarget->axes[1].Add(pTarget->axes[1], axes[1]);
#endif
    }
};

struct SlerpRot
{
    static void Blend(
        BoneAnimBlendResult* pTarget, const BoneAnimResult* pResult, float weight)
    {
        Mtx34 mtx;
        AxesToMtx::Convert(&mtx, pResult);
        if (pTarget->weight == 0.0f)
        {
            pTarget->rotate.Set(mtx);
        }
        else
        {
            Quat quat;
            quat.Set(mtx);
            float t = weight * Math::Rcp(pTarget->weight + weight);
            pTarget->rotate.Slerp(pTarget->rotate, quat, t);
        }
    }
};

NW_G3D_INLINE
void NormalizeResult(BoneAnimBlendResult *pResult, float weight)
{
    float rcpWeight = Math::Rcp(weight);
#if defined( __ghs__ )
    f32x2* pScaleTrans = reinterpret_cast<f32x2*>(&pResult->scale);
    pScaleTrans[0] = __PS_MULS0F(pScaleTrans[0], rcpWeight);
    pScaleTrans[1] = __PS_MULS0F(pScaleTrans[1], rcpWeight);
    pScaleTrans[2] = __PS_MULS0F(pScaleTrans[2], rcpWeight);
#else
    pResult->scale.Mul(pResult->scale, rcpWeight);
    pResult->translate.Mul(pResult->translate, rcpWeight);
#endif
}

NW_G3D_INLINE
bool WeightEqualsZero(float weight)
{
    return Math::Abs(weight) < 0.001f;
}

NW_G3D_INLINE
bool WeightEqualsOne(float weight)
{
    return Math::Abs(weight - 1.0f) < 0.001f;
}

}

void SkeletalAnimObj::Sizer::Calc(const InitArg& arg)
{
    NW_G3D_ASSERT(arg.IsValid());

    int numBind = std::max(arg.GetMaxBoneCount(), arg.GetMaxBoneAnimCount());
    int numAnim = arg.GetMaxBoneAnimCount();
    int numCurve = arg.GetMaxCurveCount();

    int idx = 0;
    chunk[idx++].size = Align(sizeof(BoneAnimResult) * numAnim, LL_CACHE_FETCH_SIZE);
    chunk[idx++].size = sizeof(bit32) * numBind;
    chunk[idx++].size = arg.IsContextEnabled() ? Align(sizeof(AnimFrameCache) * numCurve) : 0;

    NW_G3D_ASSERT(idx == NUM_CHUNK);

    CalcOffset(chunk, NUM_CHUNK);
}

size_t SkeletalAnimObj::CalcBufferSize(const InitArg& arg)
{
    Sizer& sizer = arg.GetSizer();
    sizer.Calc(arg);
    return sizer.GetTotalSize();
}

bool SkeletalAnimObj::Init(const InitArg& arg, void* pBuffer, size_t bufferSize)
{
    NW_G3D_ASSERT(bufferSize == 0 || pBuffer);
    NW_G3D_WARNING(IsAligned(pBuffer, BUFFER_ALIGNMENT), "pBuffer must be aligned.");

    Sizer& sizer = arg.GetSizer();
    if (!sizer.IsValid())
    {
        sizer.Calc(arg);
    }
    if (sizer.GetTotalSize() > bufferSize)
    {
        return false;
    }

    int numBind = std::max(arg.GetMaxBoneCount(), arg.GetMaxBoneAnimCount());
    int numCurve = arg.GetMaxCurveCount();

    void* ptr = pBuffer;
    SetBufferPtr(pBuffer);
    m_pRes = NULL;
    GetBindTable().Init(sizer.GetPtr<bit32>(ptr, Sizer::BIND_TABLE), numBind);
    GetContext().Init(sizer.GetPtr<AnimFrameCache>(ptr, Sizer::FRAMECACHE_ARRAY), numCurve);
    SetResultBuffer(sizer.GetPtr(ptr, Sizer::RESULT_BUFFER));
    m_pMemResultBuffer = GetResultBuffer();
    m_MaxBoneAnim = arg.GetMaxBoneAnimCount();
    m_Flag = CACHE_RESULT;
    m_pFuncCalcImpl = NULL;
    m_pFuncApplyToImpl = NULL;
    return true;
}

void SkeletalAnimObj::SetResource(ResSkeletalAnim* pRes)
{
    NW_G3D_ASSERT(IsAcceptable(pRes));
    NW_G3D_ASSERT(pRes->GetBoneAnimCount() <= GetBindTable().GetSize());

    m_pRes = pRes;
    m_pBoneAnimArray = pRes->ref().ofsBoneAnimArray.to_ptr<ResBoneAnimData>();

    SetTargetUnbound();

    if ((pRes->GetRotateMode() == nw::g3d::res::ResSkeletalAnim::ROT_EULER_XYZ))
    {
        m_pFuncCalcImpl = &SkeletalAnimObj::CalcImpl<EulerToAxes>;
    }
    else
    {
        m_pFuncCalcImpl = &SkeletalAnimObj::CalcImpl<QuatToAxes>;
    }
    m_pFuncApplyToImpl = &SkeletalAnimObj::ApplyToImpl<AxesToMtx>;

    bool loop = (pRes->ref().flag & AnimFlag::PLAYPOLICY_LOOP) != 0;
    ResetFrameCtrl(pRes->GetFrameCount(), loop);
    GetBindTable().SetAnimCount(pRes->GetBoneAnimCount());
    GetContext().SetCurveCount(pRes->GetCurveCount());
}

BindResult SkeletalAnimObj::Bind(const ResSkeleton* pSkeleton)
{
    NW_G3D_ASSERT(IsAcceptable(pSkeleton));

    BindResult result;
    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(pSkeleton->GetBoneCount());

    for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        const char* name = GetBoneAnim(idxAnim)->GetName();
        int idxTarget = pSkeleton->GetBoneIndex(name);
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
    SkeletalAnimObj::ClearResult(pSkeleton);
    return result;
}

BindResult SkeletalAnimObj::Bind(const SkeletonObj* pSkeletonObj)
{
    NW_G3D_ASSERT_NOT_NULL(pSkeletonObj);
    return Bind(pSkeletonObj->GetResource());
}

BindResult SkeletalAnimObj::Bind(const ResModel* pModel)
{
    NW_G3D_ASSERT_NOT_NULL(pModel);
    return Bind(pModel->GetSkeleton());
}

BindResult SkeletalAnimObj::Bind(const ModelObj* pModelObj)
{
    NW_G3D_ASSERT_NOT_NULL(pModelObj);
    return Bind(pModelObj->GetSkeleton()->GetResource());
}

void SkeletalAnimObj::BindFast(const ResSkeleton* pSkeleton)
{
    NW_G3D_ASSERT(IsAcceptable(pSkeleton));
    NW_G3D_ASSERT(pSkeleton == m_pRes->GetBindSkeleton());

    AnimBindTable& bindTable = GetBindTable();
    bindTable.ClearAll(pSkeleton->GetBoneCount());
    bindTable.BindAll(m_pRes->ref().ofsBindIndexArray.to_ptr<u16>());

    SetTargetBound();
    SkeletalAnimObj::ClearResult(pSkeleton);
}

void SkeletalAnimObj::BindFast(const ResModel* pModel)
{
    NW_G3D_ASSERT_NOT_NULL(pModel);
    BindFast(pModel->GetSkeleton());
}

void SkeletalAnimObj::ClearResult()
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);

    BoneAnimResult* pResultArray = GetResultArray();

    for (int idxAnim = 0, numAnim = GetAnimCount(); idxAnim < numAnim; ++idxAnim)
    {
        ResBoneAnim* pBoneAnim = GetBoneAnim(idxAnim);
        BoneAnimResult* pResult = &pResultArray[idxAnim];
        pBoneAnim->Init(pResult, NULL);
    }
}

void SkeletalAnimObj::ClearResult(const ResSkeleton* pSkeleton)
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);
    NW_G3D_ASSERT(IsTargetBound());
    NW_G3D_ASSERT_NOT_NULL(pSkeleton);

    void (*pFuncConvert)(BoneAnimResult* pResult) =
        (m_pRes->GetRotateMode() == nw::g3d::res::ResSkeletalAnim::ROT_EULER_XYZ) ?
            &EulerToAxes::Convert :
            &QuatToAxes::Convert;

    BoneAnimResult* pResultArray = GetResultArray();
    const AnimBindTable& bindTable = GetBindTable();

    for (int idxBoneAnim = 0, numBoneAnim = bindTable.GetAnimCount();
        idxBoneAnim < numBoneAnim; ++idxBoneAnim)
    {
        int idxTarget = bindTable.GetTargetIndex(idxBoneAnim);
        if (idxTarget != AnimBindTable::NOT_BOUND)
        {
            ResBoneAnim* pBoneAnim = GetBoneAnim(idxBoneAnim);
            const ResBone* pBone = pSkeleton->GetBone(idxTarget);
            BoneAnimResult* pResult = &pResultArray[idxBoneAnim];
            pBoneAnim->Init(pResult, pBone);

            (*pFuncConvert)(pResult);
        }
    }
}

void SkeletalAnimObj::SetBindFlag(const ResSkeleton* pSkeleton, int boneIndex, BindFlag flag)
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);
    NW_G3D_ASSERT(IsTargetBound());
    NW_G3D_ASSERT_NOT_NULL(pSkeleton);

    bit32 flagRaw = CreateFlagValue<bit32>(
        bit32(flag), AnimBindTable::FLAG_SHIFT, AnimBindTable::FLAG_MASK);
    AnimBindTable& bindTable = GetBindTable();
    int branchEndIndex = pSkeleton->GetBranchEndIndex(boneIndex);
    do
    {
        int idxBoneAnim = bindTable.GetAnimIndex(boneIndex);
        if (idxBoneAnim != AnimBindTable::NOT_BOUND)
        {
            bindTable.SetBindFlagRaw(idxBoneAnim, flagRaw);
        }
    } while (++boneIndex < branchEndIndex);
}

void SkeletalAnimObj::Calc()
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);
    NW_G3D_ASSERT(IsTargetBound());

    if (IsFrameChanged())
    {
        (this->*m_pFuncCalcImpl)();
        UpdateLastFrame();
    }
}

void SkeletalAnimObj::ApplyTo(ModelObj* pModelObj) const
{
    NW_G3D_ASSERT_NOT_NULL(pModelObj);
    ApplyTo(pModelObj->GetSkeleton());
}

void SkeletalAnimObj::ApplyTo(SkeletonObj* pSkeletonObj) const
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);
    NW_G3D_ASSERT(IsTargetBound());
    NW_G3D_ASSERT_NOT_NULL(pSkeletonObj);

    (this->*m_pFuncApplyToImpl)(pSkeletonObj);
}

template <typename ConvRot>
void SkeletalAnimObj::CalcImpl()
{
    float frame = GetFrameCtrl().GetFrame();
    BoneAnimResult* pResultArray = GetResultArray();
    const AnimBindTable& bindTable = GetBindTable();
    AnimContext& context = GetContext();

    if (context.IsFrameCacheValid())
    {
        int numContext = 0;
        for (int idxBoneAnim = 0, numBoneAnim = bindTable.GetAnimCount();
            idxBoneAnim < numBoneAnim; ++idxBoneAnim)
        {
            const ResBoneAnim* pBoneAnim = GetBoneAnim(idxBoneAnim);
            int idxContext = numContext;
            numContext += pBoneAnim->GetCurveCount();

            if (!bindTable.IsCalcEnabled(idxBoneAnim))
            {
                continue;
            }

            BoneAnimResult* pResult = &pResultArray[idxBoneAnim];
            pBoneAnim->Eval(pResult, frame, context.GetFrameCacheArray(idxContext));
            ConvRot::Convert(pResult);
        }
    }
    else
    {
        for (int idxBoneAnim = 0, numBoneAnim = bindTable.GetAnimCount();
            idxBoneAnim < numBoneAnim; ++idxBoneAnim)
        {
            if (!bindTable.IsCalcEnabled(idxBoneAnim))
            {
                continue;
            }

            const ResBoneAnim* pBoneAnim = GetBoneAnim(idxBoneAnim);
            BoneAnimResult* pResult = &pResultArray[idxBoneAnim];
            pBoneAnim->Eval(pResult, frame);
            ConvRot::Convert(pResult);
        }
    }
}

template <typename ConvRot>
void SkeletalAnimObj::ApplyToImpl(SkeletonObj* pSkeletonObj) const
{
    LocalMtx* pLocalMtxArray = pSkeletonObj->GetLocalMtxArray();
    const BoneAnimResult* pResultArray = GetResultArray();
    const AnimBindTable& bindTable = GetBindTable();

    for (int idxBoneAnim = 0, numBoneAnim = bindTable.GetAnimCount();
        idxBoneAnim < numBoneAnim; ++idxBoneAnim)
    {
        if (!bindTable.IsApplyEnabled(idxBoneAnim))
        {
            continue;
        }

        int idxBone = bindTable.GetTargetIndex(idxBoneAnim);
        LocalMtx& local = pLocalMtxArray[idxBone];
        const BoneAnimResult* pResult = &pResultArray[idxBoneAnim];
        CopySTAndFlag(local, pResult->flag, pResult->scale, pResult->translate);
        ConvRot::Convert(&local.mtxRT, pResult);
    }
}

class SkeletalAnimBlender::Impl
{
public:
    static void (SkeletalAnimBlender::* const s_pFuncApplyTo[])(SkeletonObj*) const;
};

void (SkeletalAnimBlender::* const SkeletalAnimBlender::Impl::s_pFuncApplyTo[])(SkeletonObj*) const = {
    &SkeletalAnimBlender::ApplyToImpl<AxesToMtx, BLEND_INTERPOLATE>,
    &SkeletalAnimBlender::ApplyToImpl<AxesToMtx, BLEND_ADD>,
    &SkeletalAnimBlender::ApplyToImpl<QuatToMtx, BLEND_INTERPOLATE>,
    &SkeletalAnimBlender::ApplyToImpl<QuatToMtx, BLEND_ADD>
};

void SkeletalAnimBlender::Sizer::Calc(const InitArg& arg)
{
    NW_G3D_ASSERT(arg.IsValid());

    int numTarget = arg.GetMaxBoneCount();

    int idx = 0;
    chunk[idx++].size = Align(sizeof(BoneAnimBlendResult) * numTarget, BUFFER_ALIGNMENT);
    NW_G3D_ASSERT(idx == NUM_CHUNK);

    CalcOffset(chunk, NUM_CHUNK);
}

size_t SkeletalAnimBlender::CalcBufferSize(const InitArg& arg)
{
    Sizer& sizer = arg.GetSizer();
    sizer.Calc(arg);
    return sizer.GetTotalSize() + BUFFER_ALIGNMENT;
}

bool SkeletalAnimBlender::Init(const InitArg& arg, void* pBuffer, size_t bufferSize)
{
    NW_G3D_ASSERT(bufferSize == 0 || pBuffer);
    NW_G3D_WARNING(IsAligned(pBuffer, BUFFER_ALIGNMENT), "pBuffer must be aligned.");

    Sizer& sizer = arg.GetSizer();
    if (!sizer.IsValid())
    {
        sizer.Calc(arg);
    }
    if (sizer.GetTotalSize() > bufferSize)
    {
        return false;
    }

    m_pBufferPtr = pBuffer;

    void* ptr = Align(pBuffer, BUFFER_ALIGNMENT);
    m_pResultBuffer = sizer.GetPtr<void>(ptr, Sizer::RESULT_BUFFER);
    m_pMemResultBuffer = m_pResultBuffer;
    m_Flag = CACHE_RESULT;
    m_MaxBone = static_cast<u16>(arg.GetMaxBoneCount());
    m_NumBone = m_MaxBone;

    return true;
}

void SkeletalAnimBlender::ClearResult()
{
    m_Flag |= NORMALIZED | CACHE_RESULT;

    size_t resultSize = Align(sizeof(BoneAnimBlendResult) * m_NumBone, BUFFER_ALIGNMENT);
    CPUCache::FillZero(m_pResultBuffer, resultSize);
}

void SkeletalAnimBlender::Blend(SkeletalAnimObj* pAnimObj, float weight)
{
    NW_G3D_ASSERT_NOT_NULL(pAnimObj);

    if (WeightEqualsZero(weight))
    {
        return;
    }

    m_Flag &= ~NORMALIZED;

    pAnimObj->Calc();

    const BoneAnimResult* pResultArray = pAnimObj->GetResultArray();
    BoneAnimBlendResult* pTargetArray = GetResultArray();
    const AnimBindTable& bindTable = pAnimObj->GetBindTable();

    if (IsSlerpEnabled())
    {
        for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
        {
            if (!bindTable.IsApplyEnabled(idxAnim))
            {
                continue;
            }

            int idxTarget = bindTable.GetTargetIndex(idxAnim);
            BoneAnimBlendResult* pTarget = &pTargetArray[idxTarget];
            const BoneAnimResult* pResult = &pResultArray[idxAnim];
            SlerpRot::Blend(pTarget, pResult, weight);
            BlendSTAndFlag(pTarget, pResult, weight);
        }
    }
    else
    {
        for (int idxAnim = 0, numAnim = bindTable.GetAnimCount(); idxAnim < numAnim; ++idxAnim)
        {
            if (!bindTable.IsApplyEnabled(idxAnim))
            {
                continue;
            }

            int idxTarget = bindTable.GetTargetIndex(idxAnim);
            BoneAnimBlendResult* pTarget = &pTargetArray[idxTarget];
            const BoneAnimResult* pResult = &pResultArray[idxAnim];
            LerpRot::Blend(pTarget, pResult, weight);
            BlendSTAndFlag(pTarget, pResult, weight);
        }
    }
}

void SkeletalAnimBlender::ApplyTo(SkeletonObj* pSkeletonObj) const
{
    int funcIndex = (m_Flag & (BLEND_MASK | USE_SLERP)) | ((m_Flag & NORMALIZED) >> 2);
    (this->*Impl:: s_pFuncApplyTo[funcIndex])(pSkeletonObj);
    m_Flag |= NORMALIZED;
}

template <typename ConvRot, SkeletalAnimBlender::BlendMode mode>
void SkeletalAnimBlender::ApplyToImpl(SkeletonObj* pSkeletonObj) const
{
    NW_G3D_ASSERT_NOT_NULL(pSkeletonObj);
    BoneAnimBlendResult* pResultArray = GetResultArrayMutable();
    LocalMtx* pLocalMtxArray = pSkeletonObj->GetLocalMtxArray();
    for (int idxBone = 0; idxBone < m_NumBone; ++idxBone)
    {
        BoneAnimBlendResult* pResult = &pResultArray[idxBone];
        float weight = pResult->weight;
        if (WeightEqualsZero(weight))
        {
            continue;
        }

        if (NW_G3D_STATIC_CONDITION(mode != BLEND_ADD) && !WeightEqualsOne(weight))
        {
            NormalizeResult(pResult, weight);
        }

        LocalMtx& local = pLocalMtxArray[idxBone];
        CopySTAndFlag(local, ~pResult->flag, pResult->scale, pResult->translate);
        ConvRot::Convert(&local.mtxRT, pResult);
    }
}

} } // namespace nw::g3d
