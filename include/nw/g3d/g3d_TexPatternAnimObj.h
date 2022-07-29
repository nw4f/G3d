#ifndef NW_G3D_TEXPATTERNANIMOBJ_H_
#define NW_G3D_TEXPATTERNANIMOBJ_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/g3d_AnimObj.h>
#include <nw/g3d/res/g3d_ResTexPatternAnim.h>

namespace nw { namespace g3d {

class ModelObj;
class MaterialObj;

class TexPatternAnimObj : public ModelAnimObj
{
public:
    class InitArg;

    enum Alignment
    {
        BUFFER_ALIGNMENT        = 4,
    };

    TexPatternAnimObj() : ModelAnimObj(), m_pRes(NULL) {}

    bool Init(const InitArg& arg, void* pBuffer, size_t bufferSize);

    static size_t CalcBufferSize(const InitArg& arg);

    virtual BindResult Bind(const ResModel* pModel);
    virtual BindResult Bind(const ModelObj* pModel);
    virtual void BindFast(const ResModel* pModel);

    void SetBindFlag(int materialIndex, BindFlag flag)
    {
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        SetBindFlagImpl(materialIndex, flag);
    }

    BindFlag GetBindFlag(int materialIndex)
    {
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        return GetBindFlagImpl(materialIndex);
    }

    virtual void ClearResult();
    virtual void Calc();
    virtual void ApplyTo(ModelObj* pModelObj) const;

    const ResTexPatternAnim* GetResource() const { return m_pRes; };

    void SetResource(ResTexPatternAnim* pRes);

    void ResetResource()
    {
        m_pRes = NULL;
        SetTargetUnbound();
    }

    bool IsAcceptable(const ResModel* pModel) const
    {
        NW_G3D_ASSERT_NOT_NULL(pModel);
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        return pModel->GetMaterialCount() <= GetBindTable().GetSize();
    }

    bool IsAcceptable(const ResTexPatternAnim* pRes) const
    {
        NW_G3D_ASSERT_NOT_NULL(pRes);
        return pRes->GetMatAnimCount() <= m_MaxMatAnim && pRes->GetPatAnimCount() <= m_MaxSubBind
            ;
    }

protected:
    ResTexPatternMatAnim* GetMatAnim(int animIndex)
    {
        return ResTexPatternMatAnim::ResCast(&m_pMatAnimArray[animIndex]);
    }

    const ResTexPatternMatAnim* GetMatAnim(int animIndex) const
    {
        return ResTexPatternMatAnim::ResCast(&m_pMatAnimArray[animIndex]);
    }

    BindResult SubBind(const ResTexPatternMatAnim* pMatAnim, const ResMaterial* pMaterial);

    BindResult SubBindFast(const ResTexPatternMatAnim* pMatAnim);

    void ApplyTo(MaterialObj* pMaterialObj, int animIndex) const;

private:
    class Sizer;
    ResTexPatternAnim* m_pRes;
    ResTexPatternMatAnimData* m_pMatAnimArray;
    s32 m_MaxMatAnim;
    s32 m_MaxSubBind;

    s8* m_pSubBindIndexArray;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(TexPatternAnimObj);
};

class TexPatternAnimObj::Sizer : public nw::g3d::Sizer
{
public:
    Sizer() : nw::g3d::Sizer() {}

    void Calc(const InitArg& arg);

    enum
    {
        RESULT_BUFFER,
        BIND_TABLE,
        SUBBIND_TABLE,
        FRAMECACHE_ARRAY,

        NUM_CHUNK
    };

    Chunk chunk[NUM_CHUNK];
};

class TexPatternAnimObj::InitArg
{
public:
    InitArg() { Clear(); }

    void Clear()
    {
        m_NumMat = m_NumMatAnim = m_NumPatAnim = m_NumCurve = -1;
        m_ContextEnabled = true;
        m_ContextAvailable = false;
    }

    void Reserve(const ResModel* pResModel)
    {
        NW_G3D_ASSERT_NOT_NULL(pResModel);
        m_NumMat = std::max(m_NumMat, pResModel->GetMaterialCount());
        m_Sizer.Invalidate();
    }

    void Reserve(const ResTexPatternAnim* pResAnim)
    {
        NW_G3D_ASSERT_NOT_NULL(pResAnim);
        m_NumMatAnim = std::max(m_NumMatAnim, pResAnim->GetMatAnimCount());
        m_NumPatAnim = std::max(m_NumPatAnim, pResAnim->GetPatAnimCount());
        m_NumCurve = std::max(m_NumCurve, pResAnim->GetCurveCount());

        m_ContextAvailable |= !pResAnim->IsCurveBaked();
        m_Sizer.Invalidate();
    }

    void EnableContext() { m_ContextEnabled = true; m_Sizer.Invalidate(); }

    void DisableContext() { m_ContextEnabled = false; m_Sizer.Invalidate(); }

    bool IsContextEnabled() const { return m_ContextAvailable && m_ContextEnabled; }

    void SetMaxMatCount(int matCount)
    {
        NW_G3D_ASSERT(matCount >= 0);
        m_NumMat = matCount;
        m_Sizer.Invalidate();
    }

    int GetMaxMatCount() const { return m_NumMat; }

    void SetMaxMatAnimCount(int matAnimCount)
    {
        NW_G3D_ASSERT(matAnimCount >= 0);
        m_NumMatAnim = matAnimCount;
        m_Sizer.Invalidate();
    }

    int GetMaxMatAnimCount() const { return m_NumMatAnim; }

    void SetMaxPatAnimCount(int patAnimCount)
    {
        NW_G3D_ASSERT(patAnimCount >= 0);
        m_NumPatAnim = patAnimCount;
        m_Sizer.Invalidate();
    }

    int GetMaxPatAnimCount() const { return m_NumPatAnim; }

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
        return m_NumMat >= 0 && m_NumMatAnim >= 0 && m_NumPatAnim >= 0 && m_NumCurve >= 0;
    }

    Sizer& GetSizer() const { return m_Sizer; }

private:
    int m_NumMat;
    int m_NumMatAnim;
    int m_NumPatAnim;
    int m_NumCurve;

    bool m_ContextEnabled;
    bool m_ContextAvailable;
    mutable Sizer m_Sizer;
};

} } // namespace nw::g3d

#endif // NW_G3D_TEXPATTERNANIMOBJ_H_
