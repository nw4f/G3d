#include <nw/g3d/g3d_AnimObj.h>
#include <limits>
#include <nw/g3d/g3d_SkeletonObj.h>

namespace nw { namespace g3d {

const float AnimFrameCtrl::INVALID_FRAME = std::numeric_limits<float>::infinity();

AnimFrameCtrl::AnimFrameCtrl()
    : m_Frame(0.0f)
    , m_StartFrame(0.0f)
    , m_EndFrame(0.0f)
    , m_Step(1.0f)
    , m_pPlayPolicy(PlayPolicy_Onetime)
    , m_pUserPtr(NULL)
{
}

void AnimFrameCtrl::Init(float startFrame, float endFrame, PlayPolicy pPlayPolicy)
{
    m_Frame = 0.0f;
    SetFrameRange(startFrame, endFrame);
    m_Step = 1.0f;
    m_pPlayPolicy = pPlayPolicy;
    m_pUserPtr = NULL;
}

float PlayPolicy_Onetime(float inputFrame, float startFrame, float endFrame, void* /*pUserData*/)
{
    if (inputFrame >= endFrame)
    {
        return endFrame;
    }
    else if (inputFrame < startFrame)
    {
        return startFrame;
    }
    else
    {
        return inputFrame;
    }
}

float PlayPolicy_Loop(float inputFrame, float startFrame, float endFrame, void* /*pUserData*/)
{
    float postframeDiff = inputFrame - endFrame;
    float preframeDiff = startFrame - inputFrame;
    if (postframeDiff * preframeDiff >= 0)
    {
        return inputFrame;
    }
    float frameDiff;
    if (postframeDiff >= 0)
    {
        frameDiff = postframeDiff;
    }
    else
    {
        frameDiff = preframeDiff;
    }

    float duration = endFrame - startFrame;
    if (duration == 0.0f)
    {
        return startFrame;
    }

    float frameMod = frameDiff - duration * static_cast<int>(frameDiff / duration);
    float framePostStart = startFrame + frameMod;
    float framePreEnd = endFrame - frameMod;
    return (postframeDiff >= 0) ? framePostStart : framePreEnd;
}

void AnimBindTable::Init(bit32* pBindArray, int tableSize)
{
    NW_G3D_ASSERT(pBindArray != NULL || tableSize == 0);
    m_pBindArray = pBindArray;
    m_Flag = 0;
    m_Size = static_cast<u16>(tableSize);
    m_NumAnim = 0;
    m_NumTarget = 0;
}

void AnimBindTable::ClearAll(int targetCount)
{
    NW_G3D_ASSERT(0 <= targetCount && targetCount <= m_Size);
    m_NumTarget = static_cast<u16>(targetCount);
    int clearSize = m_NumTarget > m_NumAnim ? m_NumTarget : m_NumAnim;
    for (int bindIndex = 0; bindIndex < clearSize; ++bindIndex)
    {
        m_pBindArray[bindIndex] = bit32(NOT_BOUND | REVERSE_NOT_BOUND | FLAG_DISABLED);
    }
}

void AnimBindTable::BindAll(const u16* pBindIndexArray)
{
    NW_G3D_ASSERT(pBindIndexArray != NULL || m_NumAnim == 0);
    for (int animIndex = 0; animIndex < m_NumAnim; ++animIndex)
    {
        u32 targetIndex = pBindIndexArray[animIndex];
        NW_G3D_ASSERT_INDEX_BOUNDS(targetIndex, m_NumTarget);
        if (targetIndex < NOT_BOUND)
        {
            Bind(animIndex, targetIndex);
        }
    }
}

void AnimContext::Init(AnimFrameCache* pFrameCacheArray, int arraySize)
{
    m_pFrameCacheArray = pFrameCacheArray;
    m_Size = (pFrameCacheArray == NULL) ? 0 : arraySize;
    m_NumCurve = 0;
}

AnimObj::AnimObj() : m_pBufferPtr(NULL)
{
    SetFrameCtrl(NULL);
}

void AnimObj::ResetFrameCtrl(int frameCount, bool loop)
{
    m_DefaultFrameCtrl.Init(0.0f, static_cast<float>(frameCount),
        loop ? PlayPolicy_Loop : PlayPolicy_Onetime);
}

} } // namespace nw::g3d
