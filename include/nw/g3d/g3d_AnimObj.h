#ifndef NW_G3D_ANIMOBJ_H_
#define NW_G3D_ANIMOBJ_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/math/g3d_MathCommon.h>
#include <nw/g3d/res/g3d_ResAnimCurve.h>
#include <nw/g3d/res/g3d_Binding.h>
#include <nw/g3d/g3d_Sizer.h>
#include <nw/g3d/ut/g3d_Flag.h>

#include <limits>

namespace nw { namespace g3d {

namespace res {

class ResModel;

} // namespace res

class ModelObj;

class AnimFrameCtrl
{
public:
    typedef float (*PlayPolicy)(
        float inputFrame, float startFrame, float endFrame, void* pUserPtr);

    AnimFrameCtrl();

    void Init(float startFrame, float endFrame, PlayPolicy pPlayPolicy);

    void UpdateFrame()
    {
        m_Frame = m_pPlayPolicy(m_Frame + m_Step, m_StartFrame, m_EndFrame, m_pUserPtr);
    }

    void SetFrame(float frame)
    {
        m_Frame = m_pPlayPolicy(frame, m_StartFrame, m_EndFrame, m_pUserPtr);
    }

    float GetFrame() const { return m_Frame; }

    void SetFrameRange(float start, float end)
    {
        NW_G3D_ASSERT(end - start >= 0.0f);
        m_StartFrame = start;
        m_EndFrame = end;
    }

    float GetStartFrame() const { return m_StartFrame; }

    float GetEndFrame() const { return m_EndFrame; }

    void SetStep(float step) { m_Step = step; }

    float GetStep() const { return m_Step; }

    void SetPlayPolicy(PlayPolicy policy) { m_pPlayPolicy = policy; }

    PlayPolicy GetPlayPolicy() const { return m_pPlayPolicy; }

    void SetUserPtr(void* pUserPtr) { m_pUserPtr = pUserPtr; }

    void* GetUserPtr() { return m_pUserPtr; }

    const void* GetUserPtr() const { return m_pUserPtr; }

    template <typename T>
    T* GetUserPtr() { return static_cast<T*>(m_pUserPtr); }

    template <typename T>
    const T* GetUserPtr() const { return static_cast<const T*>(m_pUserPtr); }

    static const float INVALID_FRAME;

private:
    float m_Frame;
    float m_StartFrame;
    float m_EndFrame;
    float m_Step;
    PlayPolicy m_pPlayPolicy;
    void* m_pUserPtr;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(AnimFrameCtrl);
};

float PlayPolicy_Onetime(float inputFrame, float startFrame, float endFrame, void* pUserData);

float PlayPolicy_Loop(float inputFrame, float startFrame, float endFrame, void* pUserData);

class AnimBindTable
{
public:
    enum BindFlag
    {
        ENABLED             = 0,
        SKIP_CALC           = 0x1 << 0,
        SKIP_APPLY          = 0x1 << 1,
        DISABLED            = SKIP_CALC | SKIP_APPLY
    };

    enum Flag
    {
        NOT_BOUND           = 0x7FFF,
        INDEX_MASK          = 0x7FFF,
        REVERSE_SHIFT       = 15,
        REVERSE_NOT_BOUND   = NOT_BOUND     << REVERSE_SHIFT,
        REVERSE_INDEX_MASK  = INDEX_MASK    << REVERSE_SHIFT,
        FLAG_SHIFT          = REVERSE_SHIFT * 2,
        FLAG_ENABLED        = ENABLED       << FLAG_SHIFT,
        FLAG_SKIP_CALC      = SKIP_CALC     << FLAG_SHIFT,
        FLAG_SKIP_APPLY     = SKIP_APPLY    << FLAG_SHIFT,
        FLAG_DISABLED       = DISABLED      << FLAG_SHIFT,
        FLAG_MASK           = 0x3           << FLAG_SHIFT,
        TARGET_BOUND        = 0x1
    };

    AnimBindTable() {}

    void Init(bit32* pBindArray, int tableSize);

    void SetAnimCount(int animCount)
    {
        NW_G3D_ASSERT(0 <= animCount && animCount <= m_Size);
        m_NumAnim = static_cast<u16>(animCount);
    }

    int GetSize() const { return m_Size; }

    int GetAnimCount() const { return m_NumAnim; }

    int GetTargetCount() const { return m_NumTarget; }

    void ClearAll(int targetCount);

    void BindAll(const u16* pBindIndexArray);

    void Bind(int animIndex, int targetIndex)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(animIndex, m_NumAnim);
        NW_G3D_ASSERT_INDEX_BOUNDS(targetIndex, m_NumTarget);
        m_pBindArray[animIndex] &= ~(INDEX_MASK | FLAG_MASK);
        m_pBindArray[animIndex] |= (targetIndex & INDEX_MASK);
        m_pBindArray[targetIndex] &= ~REVERSE_INDEX_MASK;
        m_pBindArray[targetIndex] |= (animIndex & INDEX_MASK) << REVERSE_SHIFT;
    }

    void Unbind(int animIndex, int targetIndex)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(animIndex, m_NumAnim);
        NW_G3D_ASSERT_INDEX_BOUNDS(targetIndex, m_NumTarget);
        NW_G3D_ASSERT(targetIndex == GetTargetIndex(animIndex));
        m_pBindArray[animIndex] |= (NOT_BOUND | FLAG_DISABLED);
        m_pBindArray[targetIndex] |= REVERSE_NOT_BOUND;
    }

    int GetTargetIndex(int animIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(animIndex, m_NumAnim);
        return  m_pBindArray[animIndex] & INDEX_MASK;
    }

    int GetAnimIndex(int targetIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(targetIndex, m_NumTarget);
        return ( m_pBindArray[targetIndex] >> REVERSE_SHIFT) & INDEX_MASK;
    }

    void SetTargetBound() { m_Flag |= TARGET_BOUND; }

    void SetTargetUnbound() { m_Flag &= ~TARGET_BOUND; }

    bool IsTargetBound() const { return 0 != (m_Flag & TARGET_BOUND); }

    bool IsCalcEnabled(int animIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(animIndex, m_NumAnim);
        return CheckFlag(m_pBindArray[animIndex], FLAG_SKIP_CALC, 0);
    }

    bool IsApplyEnabled(int animIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(animIndex, m_NumAnim);
        return CheckFlag(m_pBindArray[animIndex], FLAG_SKIP_APPLY, 0);
    }

    bool IsEnabled(int animIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(animIndex, m_NumAnim);
        return CheckFlag(m_pBindArray[animIndex], FLAG_MASK, FLAG_ENABLED);
    }

    bool IsDisabled(int animIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(animIndex, m_NumAnim);
        return CheckFlag(m_pBindArray[animIndex], FLAG_MASK, FLAG_DISABLED);
    }

    BindFlag GetBindFlag(int animIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(animIndex, m_NumAnim);
        return BindFlag(m_pBindArray[animIndex] >> FLAG_SHIFT);
    }

    void SetBindFlag(int animIndex, BindFlag flag)
    {
        SetBindFlagRaw(animIndex, CreateFlagValue<bit32>(bit32(flag), FLAG_SHIFT, FLAG_MASK));
    }

    void SetBindFlagRaw(int animIndex, bit32 flag)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(animIndex, m_NumAnim);
        m_pBindArray[animIndex] &= ~FLAG_MASK;
        m_pBindArray[animIndex] |= flag;
    }

private:
    bit32* m_pBindArray;
    bit16 m_Flag;
    u16 m_Size;
    u16 m_NumAnim;
    u16 m_NumTarget;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(AnimBindTable);
};

class AnimContext
{
public:
    AnimContext() {}

    void Init(AnimFrameCache* pFrameCacheArray, int arraySize);

    void SetCurveCount(int curveCount)
    {
        NW_G3D_WARNING(m_pFrameCacheArray == NULL || curveCount <= m_Size,
            "Too many curves to store context.\n");
        m_NumCurve = curveCount;
        Reset();
    }

    void Reset()
    {
        m_LastFrame = AnimFrameCtrl::INVALID_FRAME;
        if (IsFrameCacheValid())
        {
            for (int idxCurve = 0; idxCurve < m_NumCurve; ++idxCurve)
            {
                m_pFrameCacheArray[idxCurve].start = std::numeric_limits<float>::infinity();
            }
        }
    }

    int GetCurveCount() const { return m_NumCurve; }

    bool IsFrameCacheValid() const { return 0 < m_NumCurve && m_NumCurve <= m_Size; }

    AnimFrameCache* GetFrameCacheArray(int startIndex) { return &m_pFrameCacheArray[startIndex]; }

    void SetLastFrame(float frame) { m_LastFrame = frame; }

    float GetLastFrame() const { return m_LastFrame; }

private:
    AnimFrameCache* m_pFrameCacheArray;
    int m_Size;
    int m_NumCurve;
    float m_LastFrame;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(AnimContext);
};

class AnimObj
{
public:
    enum BindFlag
    {
        ENABLED             = AnimBindTable::ENABLED,
        SKIP_CALC           = AnimBindTable::SKIP_CALC,
        SKIP_APPLY          = AnimBindTable::SKIP_APPLY,
        DISABLED            = AnimBindTable::DISABLED
    };

    virtual ~AnimObj() {}

    AnimFrameCtrl& GetFrameCtrl() { return *m_pFrameCtrl; }

    const AnimFrameCtrl& GetFrameCtrl() const { return *m_pFrameCtrl; }

    void SetFrameCtrl(AnimFrameCtrl* pFrameCtrl)
    {
        m_pFrameCtrl = pFrameCtrl != NULL ? pFrameCtrl : &m_DefaultFrameCtrl;
    }

    AnimFrameCtrl& GetDefaultFrameCtrl() { return m_DefaultFrameCtrl; }

    const AnimFrameCtrl& GetDefaultFrameCtrl() const { return m_DefaultFrameCtrl; }

    AnimContext& GetContext() { return m_Context; }

    const AnimContext& GetContext() const { return m_Context; }

    void InvalidateContext() { m_Context.Reset(); }

    virtual void ClearResult() = 0;

    virtual void Calc() = 0;

    int GetCurveCount() const { return m_Context.GetCurveCount(); }

    void* GetBufferPtr() { return m_pBufferPtr; }

protected:
    AnimObj();

    void SetBufferPtr(void* pBuffer) { m_pBufferPtr = pBuffer; }

    void SetResultBuffer(void* pBuffer) { m_pResultBuffer = pBuffer; }

    void* GetResultBuffer() { return m_pResultBuffer; }

    const void* GetResultBuffer() const { return m_pResultBuffer; }

    void ResetFrameCtrl(int frameCount, bool loop);

    bool IsFrameChanged() const { return m_Context.GetLastFrame() != m_pFrameCtrl->GetFrame(); }

    void UpdateLastFrame() { m_Context.SetLastFrame(m_pFrameCtrl->GetFrame()); }

private:
    AnimFrameCtrl* m_pFrameCtrl;
    AnimFrameCtrl m_DefaultFrameCtrl;
    AnimContext m_Context;
    void* m_pResultBuffer;
    void* m_pBufferPtr;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(AnimObj);
};

class ModelAnimObj : public AnimObj
{
public:
    virtual BindResult Bind(const ResModel* pModel) = 0;

    virtual BindResult Bind(const ModelObj* pModel) = 0;

    virtual void BindFast(const ResModel* pModel) = 0;

    AnimBindTable& GetBindTable() { return m_BindTable; }

    const AnimBindTable& GetBindTable() const { return m_BindTable; }

    bool IsTargetBound() const { return GetBindTable().IsTargetBound(); }

    virtual void ApplyTo(ModelObj* pModelObj) const = 0;

    int GetAnimCount() const { return m_BindTable.GetAnimCount(); }

    int GetTargetCount() const { return m_BindTable.GetTargetCount(); }

protected:
    ModelAnimObj() : AnimObj() {}

    void SetTargetBound()
    {
        GetBindTable().SetTargetBound();
        InvalidateContext();
    }

    void SetTargetUnbound() { GetBindTable().SetTargetUnbound(); }

    void SetBindFlagImpl(int targetIndex, BindFlag flag);

    BindFlag GetBindFlagImpl(int targetIndex) const;

private:
    NW_G3D_DISALLOW_COPY_AND_ASSIGN(ModelAnimObj);

    AnimBindTable m_BindTable;
};

} } // namespace nw::g3d

#endif // NW_G3D_ANIMOBJ_H_
