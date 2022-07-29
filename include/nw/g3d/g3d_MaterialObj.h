#ifndef NW_G3D_MATERIALOBJ_H_
#define NW_G3D_MATERIALOBJ_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResMaterial.h>
#include <nw/g3d/g3d_Sizer.h>

namespace nw { namespace g3d {

class MaterialObj
{
public:
    enum
    {
        MAX_TEXTURE = 16
    };

    class InitArg;

    enum Alignment
    {
        BUFFER_ALIGNMENT        = 4,

        BLOCK_BUFFER_ALIGNMENT  = GX2_UNIFORM_BLOCK_ALIGNMENT
    };

    MaterialObj() : m_pRes(NULL), m_pUserPtr(NULL), m_pBufferPtr(NULL), m_pBlockBuffer(NULL) {}

    static size_t CalcBufferSize(const InitArg& arg);

    bool Init(const InitArg& arg, void* pBuffer, size_t bufferSize);

    size_t CalcBlockBufferSize() const { return m_pRes->GetRawParamSize(); }

    bool SetupBlockBuffer(void* pBuffer, size_t bufferSize);

    void CleanupBlockBuffer();

    void CalcMatBlock();

    ResMaterial* GetResource() { return m_pRes; }

    const ResMaterial* GetResource() const { return m_pRes; }

    void* GetBufferPtr() { return m_pBufferPtr; }

    void* GetBlockBufferPtr() { return m_pBlockBuffer; }

    bool IsBlockBufferValid() const { return (m_Flag & BLOCK_BUFFER_VALID) != 0; }

    GfxBuffer& GetMatBlock() { return m_MatBlock; }

    const GfxBuffer& GetMatBlock() const { return m_MatBlock; }

    void EnableBlockSwap() { m_Flag |= BLOCK_BUFFER_SWAP; }

    void DisableBlockSwap() { m_Flag &= ~BLOCK_BUFFER_SWAP; }

    bool IsBlockSwapEnabled() const { return (m_Flag & BLOCK_BUFFER_SWAP) != 0; }

    void SetUserPtr(void* pUserPtr) { m_pUserPtr = pUserPtr; }

    void* GetUserPtr() { return m_pUserPtr; }

    const void* GetUserPtr() const { return m_pUserPtr; }

    template <typename T>
    T* GetUserPtr() { return static_cast<T*>(m_pUserPtr); }

    template <typename T>
    const T* GetUserPtr() const { return static_cast<const T*>(m_pUserPtr); }

    int GetShaderParamCount() const { return m_pRes->GetShaderParamCount(); }

    const char* GetShaderParamName(int paramIndex) const
    {
        return m_pRes->GetShaderParamName(paramIndex);
    }

    int GetShaderParamIndex(const char* name) const { return m_pRes->GetShaderParamIndex(name); }

    ResShaderParam* GetResShaderParam(int paramIndex)
    {
        return m_pRes->GetShaderParam(paramIndex);
    }

    const ResShaderParam* GetResShaderParam(int paramIndex) const
    {
        return m_pRes->GetShaderParam(paramIndex);
    }

    ResShaderParam* GetResShaderParam(const char* name)
    {
        return m_pRes->GetShaderParam(name);
    }

    const ResShaderParam* GetResShaderParam(const char* name) const
    {
        return m_pRes->GetShaderParam(name);
    }

    void* EditShaderParam(int paramIndex)
    {
        ResShaderParam* pParam = GetResShaderParam(paramIndex);
        if (pParam->GetOffset() >= 0 && pParam->GetType() != ResShaderParam::TYPE_TEXSRTEX)
        {
            SetDirtyFlag(paramIndex);
        }
        return AddOffset(m_pSrcParam, pParam->GetSrcOffset());
    }

    const void* GetShaderParam(int paramIndex) const
    {
        return AddOffset(m_pSrcParam, GetResShaderParam(paramIndex)->GetSrcOffset());
    }

    template <typename T>
    T* EditShaderParam(int paramIndex)
    {
        ResShaderParam* pParam = GetResShaderParam(paramIndex);
        NW_G3D_ASSERTMSG(sizeof(T) <= pParam->GetSrcSize(), "Oversized T. %u <= %u",
            sizeof(T), pParam->GetSrcSize());
        if (pParam->GetOffset() >= 0 && pParam->GetType() != ResShaderParam::TYPE_TEXSRTEX)
        {
            SetDirtyFlag(paramIndex);
        }
        return AddOffset<T>(m_pSrcParam, pParam->GetSrcOffset());
    }

    template <typename T>
    const T* GetShaderParam(int paramIndex) const
    {
        const ResShaderParam* pParam = GetResShaderParam(paramIndex);
        NW_G3D_ASSERTMSG(sizeof(T) <= pParam->GetSrcSize(), "Oversized T. %u <= %u",
            sizeof(T), pParam->GetSrcSize());
        return AddOffset<T>(m_pSrcParam, pParam->GetSrcOffset());
    }

    void ClearShaderParam()
    {
        ResetDirtyFlags();
        memcpy(m_pSrcParam, m_pRes->ref().ofsSrcParam.to_ptr(), m_pRes->GetSrcParamSize());
        InitDependPointer();
    }

    void* GetSrcParam() { return m_pSrcParam; }

    const void* GetSrcParam() const { return m_pSrcParam; }

    void ResetDirtyFlags();

    bool IsMatBlockDirty() const
    {
        return (m_Flag & PARAM_DIRTY) || (m_pRes->ref().numShaderParamVolatile > 0);
    }

    int GetTextureCount() const { return m_pRes->GetTextureCount(); }

    ResTexture* GetResTexture(int texIndex)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(texIndex, GetTextureCount());
        return m_ppTextureArray[texIndex];
    }

    const ResTexture* GetResTexture(int texIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(texIndex, GetTextureCount());
        return m_ppTextureArray[texIndex];
    }

    void SetResTexture(int texIndex, ResTexture* texture)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(texIndex, GetTextureCount());
        m_ppTextureArray[texIndex] = texture;
    }

    void ClearTexture()
    {
        for (int idxTex = 0, numTex = GetTextureCount(); idxTex < numTex; ++idxTex)
        {
            m_ppTextureArray[idxTex] = m_pRes->GetTextureRef(idxTex)->Get();
        }
    }

    ResRenderState* GetResRenderState() { return m_pRes->GetRenderState(); }

    const ResRenderState* GetResRenderState() const { return m_pRes->GetRenderState(); }

    int GetSamplerCount() const { return m_pRes->GetSamplerCount(); }

    const char* GetSamplerName(int samplerIndex) const
    {
        return m_pRes->GetSamplerName(samplerIndex);
    }

    int GetSamplerIndex(const char* name) const { return m_pRes->GetSamplerIndex(name); }

    ResSampler* GetResSampler(int samplerIndex)
    {
        return m_pRes->GetSampler(samplerIndex);
    }

    const ResSampler* GetResSampler(int samplerIndex) const
    {
        return m_pRes->GetSampler(samplerIndex);
    }

    ResSampler* GetResSampler(const char* name) { return m_pRes->GetSampler(name); }

    const ResSampler* GetResSampler(const char* name) const { return m_pRes->GetSampler(name); }

protected:
    enum Flag
    {
        BLOCK_BUFFER_VALID  = 0x1 << 0,
        BLOCK_BUFFER_SWAP   = 0x1 << 1,
        PARAM_DIRTY         = 0x1 << 2
    };

    void SetDirtyFlag(int paramIndex)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(paramIndex, GetShaderParamCount());
        m_Flag |= PARAM_DIRTY;
        m_pDirtyFlagArray[paramIndex >> 5] |= 0x1 << (paramIndex & 0x1F);
    }

    template <bool swap>
    void ConvertParams(void* pBuffer);

    template <bool swap>
    bool ConvertDirtyParams(void* pBuffer);

    void InitDependPointer();

private:
    class Sizer;
    ResMaterial* m_pRes;

    bit32 m_Flag;

    bit32* m_pDirtyFlagArray;

    GfxBuffer m_MatBlock;
    void* m_pSrcParam;
    ResTexture** m_ppTextureArray;

    void* m_pUserPtr;
    void* m_pBufferPtr;
    void* m_pBlockBuffer;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(MaterialObj);
};

template <>
bool* MaterialObj::EditShaderParam<bool>(int);
template <>
const bool* MaterialObj::GetShaderParam<bool>(int) const;

class MaterialObj::Sizer : public nw::g3d::Sizer
{
public:
    Sizer() : nw::g3d::Sizer() {}

    void Calc(const InitArg& arg);

    enum
    {
        DIRTY_FLAG_ARRAY,

        SRC_PARAM_ARRAY,
        TEXTURE_ARRAY,
        NUM_CHUNK
    };

    Chunk chunk[NUM_CHUNK];
};

class MaterialObj::InitArg
{
public:
    explicit InitArg(ResMaterial* resource)
        : m_pRes(resource)

    {
        NW_G3D_ASSERT_NOT_NULL(resource);
    }

    ResMaterial* GetResource() const { return m_pRes; }

    Sizer& GetSizer() const { return m_Sizer; }

private:
    ResMaterial* m_pRes;

    mutable Sizer m_Sizer;
};

} } // namespace nw::g3d

#endif // NW_G3D_MATERIALOBJ_H_
