#ifndef NW_G3D_SKELETALANIMOBJ_H_
#define NW_G3D_SKELETALANIMOBJ_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/g3d_AnimObj.h>
#include <nw/g3d/res/g3d_ResModel.h>
#include <nw/g3d/res/g3d_ResSkeletalAnim.h>

namespace nw { namespace g3d {

class SkeletonObj;
class ModelObj;

class SkeletalAnimObj : public ModelAnimObj
{
public:
    class InitArg;

    enum Alignment
    {
        BUFFER_ALIGNMENT        = LL_CACHE_FETCH_SIZE,
    };

    SkeletalAnimObj() : ModelAnimObj(), m_pRes(NULL) {}

    bool Init(const InitArg& arg, void* pBuffer, size_t bufferSize);

    static size_t CalcBufferSize(const InitArg& arg);

    virtual BindResult Bind(const ResModel* pModel);
    virtual BindResult Bind(const ModelObj* pModelObj);
    virtual void BindFast(const ResModel* pModel);

    BindResult Bind(const ResSkeleton* pSkeleton);

    BindResult Bind(const SkeletonObj* pSkeletonObj);

    void BindFast(const ResSkeleton* pSkeleton);

    void SetBindFlag(int boneIndex, BindFlag flag)
    {
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        SetBindFlagImpl(boneIndex, flag);
    }

    void SetBindFlag(const ResSkeleton* pSkeleton, int boneIndex, BindFlag flag);

    BindFlag GetBindFlag(int boneIndex)
    {
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        return GetBindFlagImpl(boneIndex);
    }

    virtual void ClearResult();
    virtual void Calc();
    virtual void ApplyTo(ModelObj* pModelObj) const;

    void ClearResult(const ResSkeleton* pSkeleton);

    void ApplyTo(SkeletonObj* pSkeletonObj) const;

    const ResSkeletalAnim* GetResource() const { return m_pRes; };

    void SetResource(ResSkeletalAnim* pRes);

    void ResetResource()
    {
        m_pRes = NULL;
        SetTargetUnbound();
    }

    BoneAnimResult* GetResultArray()
    {
        m_Flag |= CACHE_RESULT;
        return static_cast<BoneAnimResult*>(GetResultBuffer());
    }

    const BoneAnimResult* GetResultArray() const
    {
        m_Flag |= CACHE_RESULT;
        return static_cast<const BoneAnimResult*>(GetResultBuffer());
    }

    bool IsAcceptable(const ResSkeleton* pSkeleton) const
    {
        NW_G3D_ASSERT_NOT_NULL(pSkeleton);
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        return pSkeleton->GetBoneCount() <= GetBindTable().GetSize();
    }

    bool IsAcceptable(const ResSkeletalAnim* pRes) const
    {
        NW_G3D_ASSERT_NOT_NULL(pRes);
        return pRes->GetBoneAnimCount() <= m_MaxBoneAnim;
    }

    size_t CalcLCSize() const
    {
        return Align(sizeof(BoneAnimResult) * GetBindTable().GetAnimCount(), LL_CACHE_FETCH_SIZE);
    }

    size_t LCMount(void* pLC, size_t size, bool load);

    void LCUnmount(bool store);

    bool IsResultOnCache() const { return !!(m_Flag & MASK_RESULT); }

protected:
    enum Flag
    {
        CACHE_RESULT        = 0x1 << 3,
        INVALID_RESULT      = 0x1 << 4,
        MASK_RESULT         = CACHE_RESULT | INVALID_RESULT,

    };

    ResBoneAnim* GetBoneAnim(int animIndex)
    {
        return ResBoneAnim::ResCast(&m_pBoneAnimArray[animIndex]);
    }

    const ResBoneAnim* GetBoneAnim(int animIndex) const
    {
        return ResBoneAnim::ResCast(&m_pBoneAnimArray[animIndex]);
    }

    template <typename ConvRot>
    void CalcImpl();

    template <typename ConvRot>
    void ApplyToImpl(SkeletonObj* pSkeletonObj) const;

private:
    class Sizer;

    ResSkeletalAnim* m_pRes;
    ResBoneAnimData* m_pBoneAnimArray;
    s32 m_MaxBoneAnim;
    mutable bit32 m_Flag;
    void* m_pMemResultBuffer;
    void (SkeletalAnimObj::*m_pFuncCalcImpl)();
    void (SkeletalAnimObj::*m_pFuncApplyToImpl)(SkeletonObj*) const;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(SkeletalAnimObj);
};

class SkeletalAnimObj::Sizer : public nw::g3d::Sizer
{
public:
    Sizer() : nw::g3d::Sizer() {}

    void Calc(const InitArg& arg);

    enum
    {
        RESULT_BUFFER,
        BIND_TABLE,
        FRAMECACHE_ARRAY,

        NUM_CHUNK
    };

    Chunk chunk[NUM_CHUNK];
};

class SkeletalAnimObj::InitArg
{
public:
    InitArg() { Clear(); }

    void Clear()
    {
        m_NumBone = m_NumBoneAnim = m_NumCurve = -1;
        m_ContextEnabled = true;
        m_ContextAvailable = false;

    }

    void Reserve(const ResSkeleton* pResSkeleton)
    {
        NW_G3D_ASSERT_NOT_NULL(pResSkeleton);
        m_NumBone = std::max(m_NumBone, pResSkeleton->GetBoneCount());
        m_Sizer.Invalidate();
    }

    void Reserve(const ResModel* pResModel)
    {
        NW_G3D_ASSERT_NOT_NULL(pResModel);
        Reserve(pResModel->GetSkeleton());
    }

    void Reserve(const ResSkeletalAnim* pResAnim)
    {
        NW_G3D_ASSERT_NOT_NULL(pResAnim);
        m_NumBoneAnim = std::max(m_NumBoneAnim, pResAnim->GetBoneAnimCount());
        m_NumCurve = std::max(m_NumCurve, pResAnim->GetCurveCount());
        m_ContextAvailable |= !pResAnim->IsCurveBaked();
        m_Sizer.Invalidate();
    }

    void EnableContext() { m_ContextEnabled = true; m_Sizer.Invalidate(); }

    void DisableContext() { m_ContextEnabled = false; m_Sizer.Invalidate(); }

    bool IsContextEnabled() const { return m_ContextAvailable && m_ContextEnabled; }

    void SetMaxBoneCount(int boneCount)
    {
        NW_G3D_ASSERT(boneCount >= 0);
        m_NumBone = boneCount;
        m_Sizer.Invalidate();
    }

    int GetMaxBoneCount() const
    {
        NW_G3D_ASSERT(m_NumBone >= 0);
        return m_NumBone;
    }

    void SetMaxBoneAnimCount(int boneAnimCount)
    {
        NW_G3D_ASSERT(boneAnimCount >= 0);
        m_NumBoneAnim = boneAnimCount;
        m_Sizer.Invalidate();
    }

    int GetMaxBoneAnimCount() const { return m_NumBoneAnim; }

    void SetMaxCurveCount(int curveCount)
    {
        NW_G3D_ASSERT(curveCount >= 0);
        m_NumCurve = curveCount;
        m_ContextAvailable = true;
        m_Sizer.Invalidate();
    }

    int GetMaxCurveCount() const { return m_NumCurve; }

    bool IsValid() const { return m_NumBone >= 0 && m_NumBoneAnim >= 0 && m_NumCurve >= 0; }

    Sizer& GetSizer() const { return m_Sizer; }

private:
    int m_NumBone;
    int m_NumBoneAnim;
    int m_NumCurve;
    bool m_ContextEnabled;
    bool m_ContextAvailable;

    mutable Sizer m_Sizer;
};

class SkeletalAnimBlender
{
public:
    enum BlendMode
    {
        BLEND_INTERPOLATE,
        BLEND_ADD
    };

    class InitArg;

    enum Alignment
    {
        BUFFER_ALIGNMENT        = LL_CACHE_FETCH_SIZE,
    };

    SkeletalAnimBlender() : m_pBufferPtr(NULL) {}

    bool Init(const InitArg& arg, void* pBuffer, size_t bufferSize);

    static size_t CalcBufferSize(const InitArg& arg);

    void ClearResult();

    void Blend(SkeletalAnimObj* pAnimObj, float weight);

    void ApplyTo(SkeletonObj* pSkeletonObj) const;

    void* GetBufferPtr() { return m_pBufferPtr; }

    void SetBoneCount(int count)
    {
        NW_G3D_ASSERT(count <= m_MaxBone);
        m_NumBone = static_cast<u16>(count);
    }

    int GetBoneCount() const { return m_NumBone; }

    int GetMaxBoneCount() const { return m_MaxBone; }

    void SetBlendMode(BlendMode mode) { m_Flag = (m_Flag & ~BLEND_MASK) | mode; }

    BlendMode GetBlendMode() const { return BlendMode(m_Flag & BLEND_MASK); }

    void EnableSlerp() { m_Flag |= USE_SLERP; }

    void DisableSlerp() { m_Flag &= ~USE_SLERP; }

    bool IsSlerpEnabled() const { return 0 != (m_Flag & USE_SLERP); }

    BoneAnimBlendResult* GetResultArray()
    {
        m_Flag |= CACHE_RESULT;
        return static_cast<BoneAnimBlendResult*>(m_pResultBuffer);
    }

    const BoneAnimBlendResult* GetResultArray() const
    {
        m_Flag |= CACHE_RESULT;
        return static_cast<const BoneAnimBlendResult*>(m_pResultBuffer);
    }

    BoneAnimBlendResult* GetResultArrayMutable() const
    {
        m_Flag |= CACHE_RESULT;
        return static_cast<BoneAnimBlendResult*>(m_pResultBuffer);
    }

    size_t CalcLCSize() const
    {
        return Align(sizeof(BoneAnimBlendResult) * m_NumBone, LL_CACHE_FETCH_SIZE);
    }

    size_t LCMount(void* pLC, size_t size, bool load);

    void LCUnmount(bool store);

    bool IsResultOnCache() const { return !!(m_Flag & MASK_RESULT); }

protected:
    class Impl;

    enum Flag
    {
        BLEND_MASK      = 0x1,
        USE_SLERP       = 0x1 << 1,
        NORMALIZED      = 0x1 << 2,
        CACHE_RESULT    = 0x1 << 3,
        INVALID_RESULT  = 0x1 << 4,
        MASK_RESULT     = CACHE_RESULT | INVALID_RESULT,
    };

    template <typename ConvRot, BlendMode mode>
    void ApplyToImpl(SkeletonObj* pSkeletonObj) const;

private:
    class Sizer;
    void* m_pResultBuffer;
    u16 m_MaxBone;
    u16 m_NumBone;
    mutable bit32 m_Flag;
    void* m_pMemResultBuffer;
    void* m_pBufferPtr;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(SkeletalAnimBlender);
};

class SkeletalAnimBlender::Sizer : public nw::g3d::Sizer
{
public:
    Sizer() : nw::g3d::Sizer() {}

    void Calc(const InitArg& arg);

    enum
    {
        RESULT_BUFFER,
        NUM_CHUNK
    };

    Chunk chunk[NUM_CHUNK];
};

class SkeletalAnimBlender::InitArg
{
public:
    InitArg() { Clear(); }

    void Clear() { m_NumBone = -1; }

    void Reserve(const ResSkeleton* pResSkeleton)
    {
        NW_G3D_ASSERT_NOT_NULL(pResSkeleton);
        m_NumBone = std::max(m_NumBone, pResSkeleton->GetBoneCount());
        m_Sizer.Invalidate();
    }

    void Reserve(const ResModel* pResModel)
    {
        NW_G3D_ASSERT_NOT_NULL(pResModel);
        Reserve(pResModel->GetSkeleton());
    }

    void SetMaxBoneCount(int boneCount)
    {
        NW_G3D_ASSERT(boneCount >= 0);
        m_NumBone = boneCount;
        m_Sizer.Invalidate();
    }

    int GetMaxBoneCount() const { return m_NumBone; }

    bool IsValid() const { return m_NumBone >= 0; }

    Sizer& GetSizer() const { return m_Sizer; }

private:
    int m_NumBone;
    mutable Sizer m_Sizer;
};

class EulerToAxes
{
public:
    static void Convert(BoneAnimResult* pResult);
};

class QuatToAxes
{
public:
    static void Convert(BoneAnimResult* pResult);
};

class AxesToMtx
{
public:
    static void Convert(Mtx34* pMtx, const BoneAnimResult* pResult);
    static void Convert(Mtx34* pMtx, const BoneAnimBlendResult* pResult);
};

class EulerToMtx
{
public:
    static void Convert(Mtx34* pMtx, const BoneAnimResult* pResult)
    {
        pMtx->SetR(pResult->euler);
    }
};

class QuatToMtx
{
public:
    static void Convert(Mtx34* pMtx, const BoneAnimResult* pResult)
    {
        pMtx->SetR(pResult->quat);
    }

    static void Convert(Mtx34* pMtx, const BoneAnimBlendResult* pResult)
    {
        pMtx->SetR(pResult->rotate);
    }
};

namespace internal {

NW_G3D_INLINE
bool LengthSqEqualsZero(float length)
{
    return length == 0.0f;
}

}

NW_G3D_INLINE
void EulerToAxes::Convert(BoneAnimResult* pResult)
{
#if defined( __ghs__ )
    f32x2 sincosx = Math::SinCos(pResult->euler.x);
    f32x2 sincosy = Math::SinCos(pResult->euler.y);
    f32x2 sincosz = Math::SinCos(pResult->euler.z);

    f32x2 cossinx = __PS_MERGE10(sincosx, sincosx);

    f32x2 sxsz_cxcz = __PS_MUL(sincosx, sincosz);
    f32x2 cxsz_sxcz = __PS_MUL(cossinx, sincosz);
    f32x2 cxcz_sxsz = __PS_MERGE10(sxsz_cxcz, sxsz_cxcz);
    f32x2 sxcz_cxsz = __PS_MERGE10(cxsz_sxcz, cxsz_sxcz);

    f32x2 sy_sy = __PS_MERGE00(sincosy, sincosy);
    f32x2 cysz_cycz = __PS_MULS1(sincosz, sincosy);

    f32x2 xz_yy = __PS_MADD(cxcz_sxsz, sy_sy, sxsz_cxcz);
    f32x2 xy_yz = __PS_MSUB(sxcz_cxsz, sy_sy, cxsz_sxcz);

    pResult->axes[0].x = cysz_cycz[1];
    pResult->axes[1].x = cysz_cycz[0];

    reinterpret_cast<f32x2&>(pResult->axes[0].a[1]) = __PS_MERGE00(xy_yz, xz_yy);
    reinterpret_cast<f32x2&>(pResult->axes[1].a[1]) = __PS_MERGE11(xz_yy, xy_yz);
#else
    float sx, sy, sz, cx, cy, cz;
    Math::SinCos(&sx, &cx, pResult->euler.x);
    Math::SinCos(&sy, &cy, pResult->euler.y);
    Math::SinCos(&sz, &cz, pResult->euler.z);

    float cxcz = cx * cz;
    float sxsy = sx * sy;
    float cxsz = cx * sz;

    pResult->axes[0].x = cy * cz;
    pResult->axes[1].x = cy * sz;

    pResult->axes[0].y = sxsy * cz - cxsz;
    pResult->axes[1].y = sxsy * sz + cxcz;

    pResult->axes[0].z = cxcz * sy + sx * sz;
    pResult->axes[1].z = cxsz * sy - sx * cz;
#endif
}

NW_G3D_INLINE
void QuatToAxes::Convert(BoneAnimResult* pResult)
{
    const Quat& quat = pResult->quat;
    Vec3& axisX = pResult->axes[0];
    Vec3& axisY = pResult->axes[1];

    float yy2 = 2 * quat.y * quat.y;
    float zz2 = 2 * quat.z * quat.z;
    float xx2 = 2 * quat.x * quat.x;
    float xy2 = 2 * quat.x * quat.y;
    float xz2 = 2 * quat.x * quat.z;
    float yz2 = 2 * quat.y * quat.z;
    float wz2 = 2 * quat.w * quat.z;
    float wx2 = 2 * quat.w * quat.x;
    float wy2 = 2 * quat.w * quat.y;

    axisX.x = 1.0f - yy2 - zz2;
    axisX.y = xy2 - wz2;
    axisX.z = xz2 + wy2;

    axisY.x = xy2 + wz2;
    axisY.y = 1.0f - xx2 - zz2;
    axisY.z = yz2 - wx2;
}

NW_G3D_INLINE
void AxesToMtx::Convert(Mtx34* pMtx, const BoneAnimBlendResult* pResult)
{
    const Vec3& axisX = pResult->axes[0];
    const Vec3& axisY = pResult->axes[1];
    Vec3 axisZ;

    axisZ.Cross(axisX, axisY);
    float lengthSqY = Vec3::LengthSq(axisY);
    float lengthSqZ = Vec3::LengthSq(axisZ);
    if (nw::g3d::internal::LengthSqEqualsZero(lengthSqY) || nw::g3d::internal::LengthSqEqualsZero(lengthSqZ))
    {
        NW_G3D_WARNING(false, "Failed to normalize matrix.\n");
        return;
    }

#if defined( __ghs__ )
    f32x2 lengthSqYZ = { lengthSqY, lengthSqZ };
    f32x2 rsqrtYZ = Math::RSqrt(lengthSqYZ);
    float rsqrtY = rsqrtYZ[0], rsqrtZ = rsqrtYZ[1];
#else
    float rsqrtY = Math::RSqrt(lengthSqY);
    float rsqrtZ = Math::RSqrt(lengthSqZ);
#endif

    Vec3& unitX = *Vec3::Cast(pMtx->v[0].a);
    Vec3& unitY = *Vec3::Cast(pMtx->v[1].a);
    Vec3& unitZ = *Vec3::Cast(pMtx->v[2].a);
    unitY.Mul(axisY, rsqrtY);
    unitZ.Mul(axisZ, rsqrtZ);
    unitX.Cross(unitY, unitZ);
}

NW_G3D_INLINE
void AxesToMtx::Convert(Mtx34* pMtx, const BoneAnimResult* pResult)
{
    const Vec3& axisX = pResult->axes[0];
    const Vec3& axisY = pResult->axes[1];

    Vec3& unitX = *Vec3::Cast(pMtx->v[0].a);
    Vec3& unitY = *Vec3::Cast(pMtx->v[1].a);
    Vec3& unitZ = *Vec3::Cast(pMtx->v[2].a);
    unitX.Set(axisX);
    unitY.Set(axisY);
    unitZ.Cross(axisX, axisY);
}

} } // namespace nw::g3d

#endif // NW_G3D_SKELETALANIMOBJ_H_
