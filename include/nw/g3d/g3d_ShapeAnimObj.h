#ifndef NW_G3D_SHAPEANIMOBJ_H_
#define NW_G3D_SHAPEANIMOBJ_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/g3d_AnimObj.h>
#include <nw/g3d/res/g3d_ResShapeAnim.h>

namespace nw { namespace g3d {

class ModelObj;
class ShapeObj;

class ShapeAnimObj : public ModelAnimObj
{
public:
    class InitArg;

    enum Alignment
    {
        BUFFER_ALIGNMENT        = 4,
    };

    ShapeAnimObj() : ModelAnimObj(), m_pRes(NULL) {}

    bool Init(const InitArg& arg, void* pBuffer, size_t bufferSize);

    static size_t CalcBufferSize(const InitArg& arg);

    virtual BindResult Bind(const ResModel* pModel);
    virtual BindResult Bind(const ModelObj* pModel);
    virtual void BindFast(const ResModel* pModel);

    void SetBindFlag(int shapeIndex, BindFlag flag)
    {
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        SetBindFlagImpl(shapeIndex, flag);
    }

    BindFlag GetBindFlag(int shapeIndex)
    {
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        return GetBindFlagImpl(shapeIndex);
    }

    virtual void ClearResult();
    virtual void Calc();
    virtual void ApplyTo(ModelObj* pModelObj) const;

    const ResShapeAnim* GetResource() const { return m_pRes; };

    void SetResource(ResShapeAnim* pRes);

    void ResetResource()
    {
        m_pRes = NULL;
        SetTargetUnbound();
    }

    bool IsAcceptable(const ResModel* pModel) const
    {
        NW_G3D_ASSERT_NOT_NULL(pModel);
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        return pModel->GetShapeCount() <= GetBindTable().GetSize();
    }

    bool IsAcceptable(const ResShapeAnim* pRes) const
    {
        NW_G3D_ASSERT_NOT_NULL(pRes);
        return pRes->GetVertexShapeAnimCount() <= m_MaxVtxShpAnim &&
            pRes->GetKeyShapeAnimCount() <= m_MaxSubBind;
    }

    int GetKeyShapeCount(int shapeIndex) const
    {
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        NW_G3D_ASSERT_INDEX_BOUNDS(shapeIndex, GetBindTable().GetTargetCount());
        int idxAnim = GetBindTable().GetAnimIndex(shapeIndex);
        if (idxAnim != AnimBindTable::NOT_BOUND && GetBindTable().IsApplyEnabled(idxAnim))
        {
            return m_pRes->GetVertexShapeAnim(idxAnim)->GetKeyShapeAnimCount();
        }
        return 0;
    }

    s8* GetKeyShapeIndexArray(int shapeIndex) const
    {
        NW_G3D_ASSERT_NOT_NULL(m_pRes);
        NW_G3D_ASSERT_INDEX_BOUNDS(shapeIndex, GetBindTable().GetTargetCount());
        int idxAnim = GetBindTable().GetAnimIndex(shapeIndex);
        if (idxAnim != AnimBindTable::NOT_BOUND && GetBindTable().IsApplyEnabled(idxAnim))
        {
            s32 beginKeyShapeAnim = m_pRes->GetVertexShapeAnim(idxAnim)->ref().beginKeyShapeAnim;
            return &m_pSubBindIndexArray[beginKeyShapeAnim];
        }
        return NULL;
    }

protected:
    ResVertexShapeAnim* GetVertexShapeAnim(int animIndex)
    {
        return ResVertexShapeAnim::ResCast(&m_pVertexShapeAnimArray[animIndex]);
    }

    const ResVertexShapeAnim* GetVertexShapeAnim(int animIndex) const
    {
        return ResVertexShapeAnim::ResCast(&m_pVertexShapeAnimArray[animIndex]);
    }

    BindResult SubBind(const ResVertexShapeAnim* pVertexShapeAnim, const ResShape* pShape);

    BindResult SubBindFast(const ResVertexShapeAnim* pVertexShapeAnim);

    void ApplyTo(ShapeObj* pShapeObj, int animIndex) const;

private:
    class Sizer;
    ResShapeAnim* m_pRes;
    ResVertexShapeAnimData* m_pVertexShapeAnimArray;
    s32 m_MaxVtxShpAnim;
    s32 m_MaxSubBind;
    s8* m_pSubBindIndexArray;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(ShapeAnimObj);
};

class ShapeAnimObj::Sizer : public nw::g3d::Sizer
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

class ShapeAnimObj::InitArg
{
public:
    InitArg() { Clear(); }

    void Clear()
    {
        m_NumShape = m_NumVertexShapeAnim = m_NumKeyShapeAnim = m_NumCurve = -1;
        m_ContextEnabled = true;
        m_ContextAvailable = false;
    }

    void Reserve(const ResModel* pResModel)
    {
        NW_G3D_ASSERT_NOT_NULL(pResModel);
        m_NumShape = std::max(m_NumShape, pResModel->GetShapeCount());
        m_Sizer.Invalidate();
    }

    void Reserve(const ResShapeAnim* pResAnim)
    {
        NW_G3D_ASSERT_NOT_NULL(pResAnim);
        m_NumVertexShapeAnim = std::max(m_NumVertexShapeAnim, pResAnim->GetVertexShapeAnimCount());
        m_NumKeyShapeAnim = std::max(m_NumKeyShapeAnim, pResAnim->GetKeyShapeAnimCount());
        m_NumCurve = std::max(m_NumCurve, pResAnim->GetCurveCount());
        m_ContextAvailable |= !pResAnim->IsCurveBaked();
        m_Sizer.Invalidate();
    }

    void EnableContext() { m_ContextEnabled = true; m_Sizer.Invalidate(); }

    void DisableContext() { m_ContextEnabled = false; m_Sizer.Invalidate(); }

    bool IsContextEnabled() const { return m_ContextAvailable && m_ContextEnabled; }

    void SetMaxShapeCount(int shapeCount)
    {
        NW_G3D_ASSERT(shapeCount >= 0);
        m_NumShape = shapeCount;
        m_Sizer.Invalidate();
    }

    int GetMaxShapeCount() const { return m_NumShape; }

    void SetMaxVertexShapeAnimCount(int vertexShapeAnimCount)
    {
        NW_G3D_ASSERT(vertexShapeAnimCount >= 0);
        m_NumVertexShapeAnim = vertexShapeAnimCount;
        m_Sizer.Invalidate();
    }

    int GetMaxVertexShapeAnimCount() const { return m_NumVertexShapeAnim; }

    void SetMaxKeyShapeAnimCount(int keyShapeAnimCount)
    {
        NW_G3D_ASSERT(keyShapeAnimCount >= 0);
        m_NumKeyShapeAnim = keyShapeAnimCount;
        m_Sizer.Invalidate();
    }

    int GetMaxKeyShapeAnimCount() const { return m_NumKeyShapeAnim; }

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
        return m_NumShape >= 0 && m_NumVertexShapeAnim >= 0 &&
            m_NumKeyShapeAnim >= 0 && m_NumCurve >= 0;
    }

    Sizer& GetSizer() const { return m_Sizer; }

    InitArg(int shapeCount, int vertexShapeAnimCount, int keyShapeAnimCount)
        : m_NumShape(shapeCount)
        , m_NumVertexShapeAnim(vertexShapeAnimCount)
        , m_NumKeyShapeAnim(keyShapeAnimCount)
        , m_NumCurve(0)
        , m_ContextEnabled(false)
        , m_ContextAvailable(true)
    {
        NW_G3D_ASSERT(shapeCount >= 0);
        NW_G3D_ASSERT(vertexShapeAnimCount >= 0);
        NW_G3D_ASSERT(keyShapeAnimCount >= 0);
    }

    void EnableContext(int curveCount)
    {
        NW_G3D_ASSERT(curveCount >= 0);
        m_NumCurve = curveCount;
        m_ContextEnabled = true;
        m_Sizer.Invalidate();
    }

private:
    int m_NumShape;
    int m_NumVertexShapeAnim;
    int m_NumKeyShapeAnim;
    int m_NumCurve;
    bool m_ContextEnabled;
    bool m_ContextAvailable;
    mutable Sizer m_Sizer;
};

} } // namespace nw::g3d

#endif // NW_G3D_SHAPEANIMOBJ_H_
