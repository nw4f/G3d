#include <nw/g3d/g3d_MaterialObj.h>
#include <algorithm>

namespace nw { namespace g3d {

void MaterialObj::Sizer::Calc(const InitArg& arg)
{
    ResMaterial* pRes = arg.GetResource();
    int numDirtyFlag = pRes->GetShaderParamCount() - pRes->GetShaderParamVolatileCount();
    size_t dirtyFlagBufferSize = Align(numDirtyFlag, 32) >> 3;

    int idx = 0;
    chunk[idx++].size = dirtyFlagBufferSize;

    chunk[idx++].size = Align(pRes->GetSrcParamSize());
    chunk[idx++].size = sizeof(ResTexture*) * pRes->GetTextureCount();
    NW_G3D_ASSERT(idx == NUM_CHUNK);

    CalcOffset(chunk, NUM_CHUNK);
}

size_t MaterialObj::CalcBufferSize(const InitArg& arg)
{
    Sizer& sizer = arg.GetSizer();
    sizer.Calc(arg);
    return sizer.GetTotalSize();
}

bool MaterialObj::Init(const InitArg& arg, void* pBuffer, size_t bufferSize)
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

    ResMaterial* pRes = arg.GetResource();
    int numTexture = pRes->GetTextureCount();

    void* ptr = pBuffer;
    m_pRes = pRes;
    m_Flag = BLOCK_BUFFER_SWAP;

    m_pDirtyFlagArray = sizer.GetPtr<u32>(ptr, Sizer::DIRTY_FLAG_ARRAY);

    memset(static_cast<GfxBuffer_t*>(&m_MatBlock), 0, sizeof(GfxBuffer_t));
    m_pSrcParam = sizer.GetPtr(ptr, Sizer::SRC_PARAM_ARRAY);
    memcpy(m_pSrcParam, pRes->ref().ofsSrcParam.to_ptr(), pRes->GetSrcParamSize());
    m_ppTextureArray = sizer.GetPtr<ResTexture*>(ptr, Sizer::TEXTURE_ARRAY);
    m_pUserPtr = NULL;
    m_pBufferPtr = pBuffer;
    m_pBlockBuffer = NULL;

    for (int idxTexture = 0; idxTexture < numTexture; ++idxTexture)
    {
        m_ppTextureArray[idxTexture] = pRes->GetTextureRef(idxTexture)->Get();
    }

    return true;
}

bool MaterialObj::SetupBlockBuffer(void* pBuffer, size_t bufferSize)
{
    NW_G3D_ASSERT(bufferSize == 0 || pBuffer);
#if NW_G3D_IS_HOST_CAFE
    NW_G3D_ASSERT_ADDR_ALIGNMENT(pBuffer, BLOCK_BUFFER_ALIGNMENT);
#endif // NW_G3D_IS_HOST_CAFE
    NW_G3D_ASSERT((m_Flag & BLOCK_BUFFER_VALID) == 0);

    size_t size = CalcBlockBufferSize();

    if (size > bufferSize)
    {
        return false;
    }

    m_pBlockBuffer = pBuffer;

    ResetDirtyFlags();

    m_MatBlock.SetData(pBuffer, bufferSize);
    m_MatBlock.Setup();
    m_Flag |= BLOCK_BUFFER_VALID;

    return true;
}

void MaterialObj::CalcMatBlock()
{
    bool ret;
    void* pBuffer = m_MatBlock.GetData();
    if (IsBlockSwapEnabled())
    {
        ret = ConvertDirtyParams<true>(pBuffer);
    }
    else
    {
        ret = ConvertDirtyParams<false>(pBuffer);
    }

    if (ret)
        m_MatBlock.DCFlush();
}

template <bool swap>
bool MaterialObj::ConvertDirtyParams(void* pBuffer)
{
    int numDirtyFlag = m_pRes->GetShaderParamCount() - m_pRes->GetShaderParamVolatileCount();
    bool ret = false;

    if (m_Flag & PARAM_DIRTY)
    {
        for (int idxFlag32 = 0, numFlag32 = (numDirtyFlag + 31) >> 5;
            idxFlag32 < numFlag32; ++idxFlag32)
        {
            bit32 flag32 = m_pDirtyFlagArray[idxFlag32];


            while (flag32)
            {
                int bitIndex = 31 - CountLeadingZeros(flag32);
                int paramIndex = (idxFlag32 << 5) + bitIndex;
                ResShaderParam* pParam = GetResShaderParam(paramIndex);
                NW_G3D_ASSERT(pParam->GetOffset() >= 0);
                void* pDst = AddOffset(pBuffer, pParam->GetOffset());
                const void* pSrc = AddOffset(m_pSrcParam, pParam->GetSrcOffset());
                pParam->Convert<swap>(pDst, pSrc);
                flag32 ^= 0x1 << bitIndex;
            }
            m_pDirtyFlagArray[idxFlag32] = flag32;
        }
        m_Flag ^= PARAM_DIRTY;
        ret = true;
    }

    for (int idxParam = numDirtyFlag, numParam = m_pRes->GetShaderParamCount();
        idxParam < numParam; ++idxParam)
    {
        ResShaderParam* pParam = GetResShaderParam(idxParam);
        int offset = pParam->GetOffset();
        if (offset >= 0)
        {
            void* pDst = AddOffset(pBuffer, offset);
            const void* pSrc = AddOffset(m_pSrcParam, pParam->GetSrcOffset());
            pParam->Convert<swap>(pDst, pSrc);
            ret = true;
        }
    }

    return ret;
}

void MaterialObj::ResetDirtyFlags()
{
    int numDirtyFlag = m_pRes->GetShaderParamCount() - m_pRes->GetShaderParamVolatileCount();
    memset(m_pDirtyFlagArray, 0, Align(numDirtyFlag, 32) >> 3);

    int numFlag32 = Align(numDirtyFlag, 32) >> 5;
    for (int idxFlag32 = 0; idxFlag32 < numFlag32; ++idxFlag32)
    {
        u32 flag32 = 0;
        int numBit = std::min(32, numDirtyFlag - idxFlag32 * 32);
        for (int bitShift = 0; bitShift < numBit; ++bitShift)
        {
            ResShaderParam* pParam = GetResShaderParam(idxFlag32 * 32 + bitShift);
            if (pParam->GetOffset() >= 0)
            {
                flag32 |= 0x1 << bitShift;
            }
        }
        m_pDirtyFlagArray[idxFlag32] = flag32;
    }
    m_Flag |= PARAM_DIRTY;
}

} } // namespace nw::g3d
