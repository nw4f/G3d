#include <nw/g3d/g3d_SceneAnimObj.h>

#include <nw/g3d/math/g3d_Vector3.h>
#include <nw/g3d/math/g3d_Matrix34.h>
#include <nw/g3d/math/g3d_Matrix44.h>

namespace nw { namespace g3d {

void CameraAnimObj::Sizer::Calc(const InitArg& arg)
{
    NW_G3D_ASSERT(arg.IsValid());

    int numCurve = arg.GetMaxCurveCount();

    int idx = 0;
    chunk[idx++].size = sizeof(CameraAnimResult);
    chunk[idx++].size = arg.IsContextEnabled() ? Align(sizeof(AnimFrameCache) * numCurve) : 0;
    NW_G3D_ASSERT(idx == NUM_CHUNK);

    CalcOffset(chunk, NUM_CHUNK);
}

size_t CameraAnimObj::CalcBufferSize(const InitArg& arg)
{
    Sizer& sizer = arg.GetSizer();
    sizer.Calc(arg);
    return sizer.GetTotalSize();
}

bool CameraAnimObj::Init(const InitArg& arg, void* pBuffer, size_t bufferSize)
{
    NW_G3D_ASSERT_NOT_NULL(pBuffer);
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

    int numCurve = arg.GetMaxCurveCount();

    void* ptr = pBuffer;
    SetBufferPtr(pBuffer);
    m_pRes = NULL;
    GetContext().Init(sizer.GetPtr<AnimFrameCache>(ptr, Sizer::FRAMECACHE_ARRAY), numCurve);
    SetResultBuffer(sizer.GetPtr(ptr, Sizer::RESULT_BUFFER));

    return true;
}

void CameraAnimObj::SetResource(ResCameraAnim* pRes)
{
    NW_G3D_ASSERT_NOT_NULL(pRes);

    m_pRes = pRes;

    bool loop = (pRes->ref().flag & AnimFlag::PLAYPOLICY_LOOP) != 0;
    ResetFrameCtrl(pRes->GetFrameCount(), loop);
    GetContext().SetCurveCount(pRes->GetCurveCount());
    CameraAnimObj::ClearResult();
}

void CameraAnimObj::ClearResult()
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);

    m_pRes->Init(GetResult());
}

void CameraAnimObj::Calc()
{
    NW_G3D_ASSERT_NOT_NULL(m_pRes);

    if (IsFrameChanged())
    {
        float frame = GetFrameCtrl().GetFrame();
        AnimContext& context = GetContext();
        if (context.IsFrameCacheValid())
        {
            m_pRes->Eval(GetResult(), frame, context.GetFrameCacheArray(0));
        }
        else
        {
            m_pRes->Eval(GetResult(), frame);
        }
        UpdateLastFrame();
    }
}

} } // namespace nw::g3d
