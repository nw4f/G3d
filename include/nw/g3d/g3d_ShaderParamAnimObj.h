#ifndef NW_G3D_SHADERPARAMANIMOBJ_H_
#define NW_G3D_SHADERPARAMANIMOBJ_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/g3d_AnimObj.h>
#include <nw/g3d/res/g3d_ResShaderParamAnim.h>

namespace nw { namespace g3d {

class ModelObj;
class MaterialObj;

class ShaderParamAnimObj : public ModelAnimObj
{
public:
    class InitArg;

    enum Alignment
    {
        BUFFER_ALIGNMENT        = 4,
    };

    ShaderParamAnimObj() : ModelAnimObj(), m_pRes(NULL) {}

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

    virtual void ClearResult() {}
    virtual void Calc();
    virtual void ApplyTo(ModelObj* pModelObj) const;

    const ResShaderParamAnim* GetResource() const { return m_pRes; };

    void SetResource(ResShaderParamAnim* pRes);

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

    bool IsAcceptable(const ResShaderParamAnim* pRes) const
    {
        NW_G3D_ASSERT_NOT_NULL(pRes);
        return pRes->GetMatAnimCount() <= m_MaxMatAnim && pRes->GetParamAnimCount() <= m_MaxSubBind;
    }

protected:
    ResShaderParamMatAnim* GetMatAnim(int animIndex)
    {
        return ResShaderParamMatAnim::ResCast(&m_pMatAnimArray[animIndex]);
    }

    const ResShaderParamMatAnim* GetMatAnim(int animIndex) const
    {
        return ResShaderParamMatAnim::ResCast(&m_pMatAnimArray[animIndex]);
    }

    BindResult SubBind(const ResShaderParamMatAnim* pMatAnim, const ResMaterial* pMaterial);

    BindResult SubBindFast(const ResShaderParamMatAnim* pMatAnim);

    void ApplyTo(MaterialObj* pMaterialObj, int animIndex) const;

private:
    class Sizer;
    ResShaderParamAnim* m_pRes;
    ResShaderParamMatAnimData* m_pMatAnimArray;
    s32 m_MaxMatAnim;
    s32 m_MaxSubBind;
    u16* m_pSubBindIndexArray;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(ShaderParamAnimObj);
};

class ShaderParamAnimObj::Sizer : public nw::g3d::Sizer
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

class ShaderParamAnimObj::InitArg
{
public:
    InitArg() { Clear(); }

    void Clear()
    {
        m_NumMat = m_NumMatAnim = m_NumParamAnim = m_NumCurve = -1;
        m_ContextEnabled = true;
        m_ContextAvailable = false;
    }

    void Reserve(const ResModel* pResModel)
    {
        NW_G3D_ASSERT_NOT_NULL(pResModel);
        m_NumMat = std::max(m_NumMat, pResModel->GetMaterialCount());
        m_Sizer.Invalidate();
    }

    void Reserve(const ResShaderParamAnim* pResAnim)
    {
        NW_G3D_ASSERT_NOT_NULL(pResAnim);
        m_NumMatAnim = std::max(m_NumMatAnim, pResAnim->GetMatAnimCount());
        m_NumParamAnim = std::max(m_NumParamAnim, pResAnim->GetParamAnimCount());
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

    void SetMaxParamAnimCount(int paramAnimCount)
    {
        NW_G3D_ASSERT(paramAnimCount >= 0);
        m_NumParamAnim = paramAnimCount;
        m_Sizer.Invalidate();
    }

    int GetMaxParamAnimCount() const { return m_NumParamAnim; }

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
        return m_NumMat >= 0 && m_NumMatAnim >= 0 && m_NumParamAnim >= 0 && m_NumCurve >= 0;
    }

    Sizer& GetSizer() const { return m_Sizer; }

private:
    int m_NumMat;
    int m_NumMatAnim;
    int m_NumParamAnim;
    int m_NumCurve;
    bool m_ContextEnabled;
    bool m_ContextAvailable;
    mutable Sizer m_Sizer;
};

} } // namespace nw::g3d

#endif // NW_G3D_SHADERPARAMANIMOBJ_H_
