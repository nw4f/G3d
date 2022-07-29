#ifndef NW_G3D_VISIBILITYANIMOBJ_H_
#define NW_G3D_VISIBILITYANIMOBJ_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/g3d_AnimObj.h>
#include <nw/g3d/res/g3d_ResVisibilityAnim.h>

namespace nw { namespace g3d {

class ModelObj;

class VisibilityAnimObj : public ModelAnimObj
{
public:
    class InitArg;

    enum Alignment
    {
        BUFFER_ALIGNMENT        = 4,
    };

    VisibilityAnimObj() : ModelAnimObj(), m_pRes() {}

    bool Init(const InitArg& arg, void* pBuffer, size_t bufferSize);

    static size_t CalcBufferSize(const InitArg& arg);

    virtual BindResult Bind(const ResModel* pModel);
    virtual BindResult Bind(const ModelObj* pModel);
    virtual void BindFast(const ResModel* pModel);

    void SetBindFlag(int targetIndex, BindFlag flag)
    {
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        SetBindFlagImpl(targetIndex, flag);
    }

    BindFlag GetBindFlag(int targetIndex)
    {
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        return GetBindFlagImpl(targetIndex);
    }

    virtual void ClearResult();
    virtual void Calc();
    virtual void ApplyTo(ModelObj* pModelObj) const;

    const ResVisibilityAnim* GetResource() const { return m_pRes; };

    void SetResource(ResVisibilityAnim* pRes);

    void ResetResource()
    {
        m_pRes = NULL;
        SetTargetUnbound();
    }

    bool IsAcceptable(const ResModel* pModel) const
    {
        NW_G3D_ASSERT_NOT_NULL(pModel);
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        if (m_pRes->GetVisibilityType() == ResVisibilityAnim::BONE_VISIBILITY)
        {
            return pModel->GetSkeleton()->GetBoneCount() <= GetBindTable().GetSize();
        }
        else
        {
            return pModel->GetMaterialCount() <= GetBindTable().GetSize();
        }
    }

    bool IsAcceptable(const ResVisibilityAnim* pRes) const
    {
        NW_G3D_ASSERT_NOT_NULL(pRes);
        if (pRes->GetVisibilityType() == ResVisibilityAnim::BONE_VISIBILITY)
        {
            return pRes->GetAnimCount() <= m_MaxBoneAnim;
        }
        else
        {
            return pRes->GetAnimCount() <= m_MaxMatAnim;
        }
    }

protected:
    const ResAnimCurve* GetCurve(int curveIndex) const
    {
        return ResAnimCurve::ResCast(&m_pCurveArray[curveIndex]);
    }

private:
    class Sizer;
    ResVisibilityAnim* m_pRes;
    u16 m_MaxBoneAnim;
    u16 m_MaxMatAnim;
    s32 m_NumCurve;
    ResAnimCurveData* m_pCurveArray;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(VisibilityAnimObj);
};

class VisibilityAnimObj::Sizer : public nw::g3d::Sizer
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

class VisibilityAnimObj::InitArg
{
public:
    InitArg() { Clear(); }

    void Clear()
    {
        m_NumBone = m_NumMat = m_NumBoneAnim = m_NumMatAnim = m_NumCurve = -1;
        m_ContextEnabled = true;
        m_ContextAvailable = false;
    }

    void Reserve(const ResModel* pResModel)
    {
        NW_G3D_ASSERT_NOT_NULL(pResModel);
        m_NumBone = std::max(m_NumBone, pResModel->GetSkeleton()->GetBoneCount());
        m_NumMat = std::max(m_NumMat, pResModel->GetMaterialCount());
        m_Sizer.Invalidate();
    }

    void Reserve(const ResVisibilityAnim* pResAnim)
    {
        NW_G3D_ASSERT_NOT_NULL(pResAnim);
        if (pResAnim->GetVisibilityType() == ResVisibilityAnim::BONE_VISIBILITY)
        {
            m_NumBoneAnim = std::max(m_NumBoneAnim, pResAnim->GetAnimCount());
        }
        else
        {
            m_NumMatAnim = std::max(m_NumMatAnim, pResAnim->GetAnimCount());
        }
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

    int GetMaxBoneCount() const { return m_NumBone; }

    void SetMaxMatCount(int matCount)
    {
        NW_G3D_ASSERT(matCount >= 0);
        m_NumMat = matCount;
        m_Sizer.Invalidate();
    }

    int GetMaxMatCount() const { return m_NumMat; }

    void SetMaxBoneAnimCount(int boneAnimCount)
    {
        NW_G3D_ASSERT(boneAnimCount >= 0);
        m_NumBoneAnim = boneAnimCount;
        m_Sizer.Invalidate();
    }

    int GetMaxBoneAnimCount() const { return m_NumBoneAnim; }

    void SetMaxMatAnimCount(int matAnimCount)
    {
        NW_G3D_ASSERT(matAnimCount >= 0);
        m_NumMatAnim = matAnimCount;
        m_Sizer.Invalidate();
    }

    int GetMaxMatAnimCount() const { return m_NumMatAnim; }

    void SetMaxCurveCount(int curveCount)
    {
        NW_G3D_ASSERT(curveCount >= 0);
        m_NumCurve = curveCount;
        m_ContextAvailable = true;
        m_Sizer.Invalidate();
    }

    int GetMaxCurveCount() const { return m_NumCurve; }

    bool IsValid() const
    {
        return ((m_NumBone >= 0 && m_NumBoneAnim >= 0) ||
            (m_NumMat >= 0 && m_NumMatAnim >= 0)) && m_NumCurve >= 0;
    }

    Sizer& GetSizer() const { return m_Sizer; }

private:
    int m_NumBone;
    int m_NumMat;
    int m_NumBoneAnim;
    int m_NumMatAnim;
    int m_NumCurve;
    bool m_ContextEnabled;
    bool m_ContextAvailable;
    mutable Sizer m_Sizer;
};

} } // namespace nw::g3d

#endif // NW_G3D_VISIBILITYANIMOBJ_H_
