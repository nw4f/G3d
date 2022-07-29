#ifndef NW_G3D_SCENEANIMOBJ_H_
#define NW_G3D_SCENEANIMOBJ_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/g3d_AnimObj.h>
#include <nw/g3d/res/g3d_ResSceneAnim.h>

namespace nw { namespace g3d {

class SceneAnimObj : public AnimObj
{
public:
    void SetUserPtr(void* pUserPtr) { m_pUserPtr = pUserPtr; }

    void* GetUserPtr() { return m_pUserPtr; }

    const void* GetUserPtr() const { return m_pUserPtr; }

    template <typename T>
    T* GetUserPtr() { return static_cast<T*>(m_pUserPtr); }

    template <typename T>
    const T* GetUserPtr() const { return static_cast<const T*>(m_pUserPtr); }

protected:
    SceneAnimObj() : AnimObj(), m_pUserPtr(NULL) {}

private:
    void* m_pUserPtr;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(SceneAnimObj);
};

class CameraAnimObj : public SceneAnimObj
{
public:
    class InitArg;

    enum Alignment
    {
        BUFFER_ALIGNMENT        = 4,
    };

    CameraAnimObj() : SceneAnimObj(), m_pRes(NULL) {}

    bool Init(const InitArg& arg, void* pBuffer, size_t bufferSize);

    static size_t CalcBufferSize(const InitArg& arg);

    virtual void ClearResult();
    virtual void Calc();

    const ResCameraAnim* GetResource() const { return m_pRes; };

    void SetResource(ResCameraAnim* pRes);

    void ResetResource() { m_pRes = NULL; }

    CameraAnimResult* GetResult()
    {
        return static_cast<CameraAnimResult*>(GetResultBuffer());
    }

    const CameraAnimResult* GetResult() const
    {
        return static_cast<const CameraAnimResult*>(GetResultBuffer());
    }

    bit32 GetRotateMode() const { return m_pRes->GetRotateMode(); }

    bit32 GetProjectionMode() const { return m_pRes->GetProjectionMode(); }

private:
    class Sizer;
    ResCameraAnim* m_pRes;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(CameraAnimObj);
};

class CameraAnimObj::Sizer : public nw::g3d::Sizer
{
public:
    Sizer() : nw::g3d::Sizer() {}

    void Calc(const InitArg& arg);

    enum
    {
        RESULT_BUFFER,
        FRAMECACHE_ARRAY,
        NUM_CHUNK
    };

    Chunk chunk[NUM_CHUNK];
};

class CameraAnimObj::InitArg
{
public:
    InitArg() { Clear(); }

    void Clear()
    {
        m_NumCurve = -1;
        m_ContextEnabled = true;
        m_ContextAvailable = false;
    }

    void Reserve(const ResCameraAnim* pResAnim)
    {
        NW_G3D_ASSERT_NOT_NULL(pResAnim);
        m_NumCurve = std::max(m_NumCurve, pResAnim->GetCurveCount());
        m_ContextAvailable |= !pResAnim->IsCurveBaked();
        m_Sizer.Invalidate();
    }

    void EnableContext() { m_ContextEnabled = true; m_Sizer.Invalidate(); }

    void DisableContext() { m_ContextEnabled = false; m_Sizer.Invalidate(); }

    bool IsContextEnabled() const { return m_ContextAvailable && m_ContextEnabled; }

    void SetMaxCurveCount(int curveCount)
    {
        NW_G3D_ASSERT(curveCount >= 0);
        m_NumCurve = curveCount;
        m_ContextAvailable = true;
        m_Sizer.Invalidate();
    }

    int GetMaxCurveCount() const { return m_NumCurve; }

    bool IsValid() const { return m_NumCurve >= 0; }

    Sizer& GetSizer() const { return m_Sizer; }

private:
    int m_NumCurve;
    bool m_ContextEnabled;
    bool m_ContextAvailable;
    mutable Sizer m_Sizer;
};

namespace math {

class Mtx34;
class Mtx44;

} // namespace math

class SceneAnimHelper
{
public:
    static void CalcAimCameraMtx(Mtx34* pMtx, const CameraAnimResult* pResult);
    static void CalcRotateCameraMtx(Mtx34* pMtx, const CameraAnimResult* pResult);
    static void CalcOrthoProjMtx(Mtx44* pMtx, const CameraAnimResult* pResult);
    static void CalcPerspProjMtx(Mtx44* pMtx, const CameraAnimResult* pResult);
    static void CalcOrthoProjTexMtx(Mtx34* pMtx, const CameraAnimResult* pResult);
    static void CalcPerspProjTexMtx(Mtx34* pMtx, const CameraAnimResult* pResult);
};

} } // namespace nw::g3d

#endif // NW_G3D_SCENEANIMOBJ_H_
