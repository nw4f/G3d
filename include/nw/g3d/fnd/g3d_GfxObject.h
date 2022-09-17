#ifndef NW_G3D_FND_GFXOBJECT_H_
#define NW_G3D_FND_GFXOBJECT_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/fnd/g3d_GX2Struct.h>

#define NW_G3D_GFX_OBJECT(class_name)                                                              \
public:                                                                                            \
    class_name();                                                                                  \
    typedef class_name self_type;                                                                  \
    typedef class_name##_t base_type;                                                              \
    static self_type* DownCast(base_type* ptr) { return static_cast<self_type*>(ptr); }            \
    static const self_type* DownCast(const base_type* ptr)                                         \
        { return static_cast<const self_type*>(ptr); }                                             \
    static self_type& DownCast(base_type& ref) { return static_cast<self_type&>(ref); }            \
    static const self_type& DownCast(const base_type& ref)                                         \
        { return static_cast<const self_type&>(ref); }                                             \
private:                                                                                           \
    NW_G3D_DISALLOW_COPY_AND_ASSIGN(class_name)                                                    \


namespace nw { namespace g3d { namespace fnd {

struct GfxBuffer_t
{
    union
    {
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
        void* pData;
#endif
        u32 dataUIntPtr;
    };
    u32 size;
    u32 handle;
    u16 stride;
    u16 numBuffering;

    union
    {
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
        GX2StreamOutContext* pCtxPtr;
#endif
        u32 ctxUIntPtr;

#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
        // Custom
        u32 bufferType;
#endif
    };
};

class GfxBuffer : public GfxBuffer_t
{
    NW_G3D_GFX_OBJECT(GfxBuffer);

#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
public:
    enum BufferType : u32
    {
        BUFFER_TYPE_INVALID         = 0,
        BUFFER_TYPE_ELEMENT_ARRAY,
        BUFFER_TYPE_ARRAY,
        BUFFER_TYPE_UNIFORM,
    };
#endif

public:
    void Setup();
    void Cleanup();
    void UpdateRegs();
    void DCFlush(int bufferIndex = 0) const;
    /*
    void FlushExportBuffer(int bufferIndex = 0) const;
    void DCRefresh();
    */

    void LoadIndices() const;
    void LoadVertices(u32 slot) const;
    void LoadVertexUniforms(u32 location, int bufferIndex = 0) const;
    void LoadGeometryUniforms(u32 location, int bufferIndex = 0) const;
    void LoadFragmentUniforms(u32 location, int bufferIndex = 0) const;
    /*
    void LoadComputeUniforms(u32 location, int bufferIndex = 0) const;
    void LoadStreamOutBuffer(u32 location);
    void LoadExportBuffer(int bufferIndex = 0) const;
    */

    void SetData(void* pData, u32 size, int bufferingCount = 1);

    void* GetData(int bufferIndex = 0);
    const void* GetData(int bufferIndex = 0) const;

    int GetBufferingCount() const { return numBuffering; }

    u32 GetSize() const { return size; }

    void SetStride(u32 stride) { this->stride = static_cast<u16>(stride); }
    u32 GetStride() const { return stride; }

    /*
    void SetStreamOutContext(GX2StreamOutContext* pCtx);
    GX2StreamOutContext* GetStreamOutContext();
    const GX2StreamOutContext* GetStreamOutContext() const;
    */
};

struct GfxSampler_t
{
    GX2Sampler gx2Sampler;
    u32 handle;
};

class GfxSampler : public GfxSampler_t
{
    NW_G3D_GFX_OBJECT(GfxSampler);

public:
    void Setup();
    void Cleanup();
    void UpdateRegs();

    void LoadVertexSampler(u32 unit) const;
    void LoadGeometrySampler(u32 unit) const;
    void LoadFragmentSampler(u32 unit) const;
    // void LoadComputeSampler(u32 unit) const;

    GX2Sampler* GetGX2Sampler()
    {
        return reinterpret_cast<GX2Sampler*>(&gx2Sampler);
    }

    const GX2Sampler* GetGX2Sampler() const
    {
        return reinterpret_cast<const GX2Sampler*>(&gx2Sampler);
    }

    /*
    void SetDefault();

    void SetClampX(GX2TexClamp clamp);
    void SetClampY(GX2TexClamp clamp);
    void SetClampZ(GX2TexClamp clamp);
    void SetBorderType(GX2TexBorderType type);
    */

    GX2TexClamp GetClampX() const;
    GX2TexClamp GetClampY() const;
    GX2TexClamp GetClampZ() const;
    GX2TexBorderType GetBorderType() const;

    /*
    void SetMagFilter(GX2TexXYFilterType filter);
    void SetMinFilter(GX2TexXYFilterType filter);
    void SetZFilter(GX2TexZFilterType filter);
    void SetMipFilter(GX2TexMipFilterType filter);
    void SetMaxAniso(GX2TexAnisoRatio ratio);
    */

    GX2TexXYFilterType GetMagFilter() const;
    GX2TexXYFilterType GetMinFilter() const;
    // GX2TexZFilterType GetZFilter() const;
    GX2TexMipFilterType GetMipFilter() const;
    GX2TexAnisoRatio GetMaxAniso() const;

    /*
    void SetMinLOD(float minLOD);
    void SetMaxLOD(float maxLOD);
    void SetLODBias(float bias);
    */

    float GetMinLOD() const;
    float GetMaxLOD() const;
    float GetLODBias() const;

    // void SetCompareFunc(GX2CompareFunction func);
    GX2CompareFunction GetCompareFunc() const;

    // void SetCompareEnable(GX2Boolean enable);

protected:
    GX2Boolean GetCompareEnable() const;
};

struct GfxTexture_t
{
    internal::GX2TextureData gx2Texture;
    u32 handle;
    u32 arrayLength;
};

class GfxTexture : public GfxTexture_t
{
    NW_G3D_GFX_OBJECT(GfxTexture);

public:
    void Setup();
    void Cleanup();
    void UpdateRegs();
    // void CalcSize();
    void DCFlush() const;
    // void DCRefresh();

    void LoadVertexTexture(u32 unit) const;
    void LoadGeometryTexture(u32 unit) const;
    void LoadFragmentTexture(u32 unit) const;
    // void LoadComputeTexture(u32 unit) const;

    GX2Texture* GetGX2Texture()
    {
        return reinterpret_cast<GX2Texture*>(&gx2Texture);
    }

    const GX2Texture* GetGX2Texture() const
    {
        return reinterpret_cast<const GX2Texture*>(&gx2Texture);
    }

    // void SetDefault();

    GX2SurfaceDim GetDimension() const { return gx2Texture.surface.dim; }
    GX2SurfaceFormat GetFormat() const { return gx2Texture.surface.format; }
    u32 GetWidth() const { return gx2Texture.surface.width; }
    u32 GetHeight() const { return gx2Texture.surface.height; }
    u32 GetDepth() const { return gx2Texture.surface.depth; }
    u32 GetMipLevels() const { return gx2Texture.surface.numMips; }
    u32 GetBaseSize() const { return gx2Texture.surface.imageSize; }
    u32 GetMipSize() const { return gx2Texture.surface.mipSize; }

    void SetImagePtrs(void* basePtr, void* mipPtr);

    void* GetBasePtr();
    const void* GetBasePtr() const;

    void* GetMipPtr();
    const void* GetMipPtr() const;

    // void* GetImagePtr(int mipLevel);
    const void* GetImagePtr(int mipLevel) const;

    u32 GetAlignment() const { return gx2Texture.surface.alignment; }
    u32 GetPitch() const { return gx2Texture.surface.pitch; }
};

struct GfxFetchShader_t
{
    internal::GX2FetchShaderData gx2FetchShader;
    u32 ofsVFInst;
    u32 handle;
};

class GfxFetchShader : public GfxFetchShader_t
{
    NW_G3D_GFX_OBJECT(GfxFetchShader);

    enum
    {
        MAX_INST_PER_FETCH_CLAUSE = 16,
        FETCH_INST_ALIGNMENT = 16,
        CF_INST_SIZE = 8,
        VF_INST_SIZE = 16
    };

public:
    enum
    {
        SHADER_ALIGNMENT = 256 // GX2_SHADER_ALIGNMENT
    };

    void Setup();
    void Cleanup();
    void UpdateRegs();
    void DCFlush() const;
    void CalcSize();

    void Load() const;

    GX2FetchShader* GetGX2FetchShader()
    {
        return reinterpret_cast<GX2FetchShader*>(&gx2FetchShader);
    }

    const GX2FetchShader* GetGX2FetchShader() const
    {
        return reinterpret_cast<const GX2FetchShader*>(&gx2FetchShader);
    }

    void SetAttribCount(u32 count) { gx2FetchShader.numAttribs = count; }
    u32 GetAttribCount() const { return gx2FetchShader.numAttribs; }
    u32 GetShaderSize() const { return gx2FetchShader.shaderSize; }

    void SetShaderPtr(void* ptr);
    void* GetShaderPtr();
    const void* GetShaderPtr() const;

    // void SetDivisors(u32 divisor2, u32 divisor3);

    u32 GetDivisor(int divisorIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(divisorIndex, 4);
        return divisorIndex < 2 ? divisorIndex : gx2FetchShader._divisors[divisorIndex - 2];
    }

    void SetVertexBuffer(int attribIndex, const GfxBuffer* pBuffer);
    const GfxBuffer* GetVertexBuffer(int attribIndex) const;
    /*
    void ReplaceVertexBuffer(u32 slot, const GfxBuffer* pBuffer) const;
    void ResetVertexBuffer() const;
    */

#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
public:
    void SetDefault() { SetDefault(GetGX2FetchShader()->shaderPtr); }

    void SetLocation(int attribIndex, u32 location)
    {
        SetLocation(GetGX2FetchShader()->shaderPtr, attribIndex, location);
    }

    u32 GetLocation(int attribIndex) const
    {
        return GetLocation(GetGX2FetchShader()->shaderPtr, attribIndex);
    }

    void SetBufferSlot(int attribIndex, u32 slot)
    {
        SetBufferSlot(GetGX2FetchShader()->shaderPtr, attribIndex, slot);
    }

    /*
    u32 GetBufferSlot(int attribIndex) const
    {
        return GetBufferSlot(GetGX2FetchShader()->shaderPtr, attribIndex);
    }*/

    void SetFormat(int attribIndex, GX2AttribFormat format)
    {
        SetFormat(GetGX2FetchShader()->shaderPtr, attribIndex, format);
    }

    /*
    GX2AttribFormat GetFormat(int attribIndex) const
    {
        return GetFormat(GetGX2FetchShader()->shaderPtr, attribIndex);
    }

    void SetEndianSwapMode(int attribIndex, GX2EndianSwapMode mode)
    {
        SetEndianSwapMode(GetGX2FetchShader()->shaderPtr, attribIndex, mode);
    }

    GX2EndianSwapMode GetEndianSwapMode(int attribIndex) const
    {
        return GetEndianSwapMode(GetGX2FetchShader()->shaderPtr, attribIndex);
    }

    void SetDivisorSlot(int attribIndex, u32 slot)
    {
        SetDivisorSlot(GetGX2FetchShader()->shaderPtr, attribIndex, slot);
    }
    */

    u32 GetDivisorSlot(int attribIndex) const
    {
        return GetDivisorSlot(GetGX2FetchShader()->shaderPtr, attribIndex);
    }

    void SetOffset(int attribIndex, u32 offset)
    {
        SetOffset(GetGX2FetchShader()->shaderPtr, attribIndex, offset);
    }

    u32 GetOffset(int attribIndex) const
    {
        return GetOffset(GetGX2FetchShader()->shaderPtr, attribIndex);
    }

protected:
    u32* GetVFInst(int attribIndex)
    {
        return GetVFInst(GetGX2FetchShader()->shaderPtr, attribIndex);
    }

    const u32* GetVFInst(int attribIndex) const
    {
        return GetVFInst(GetGX2FetchShader()->shaderPtr, attribIndex);
    }
#endif // NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE

public:
    void SetDefault(void* pShader);

    void SetLocation(void* pShader, int attribIndex, u32 location);
    u32 GetLocation(const void* pShader, int attribIndex) const;

    void SetBufferSlot(void* pShader, int attribIndex, u32 slot);
    // u32 GetBufferSlot(const void* pShader, int attribIndex) const;

    void SetFormat(void* pShader, int attribIndex, GX2AttribFormat format);
    /*
    GX2AttribFormat GetFormat(const void* pShader, int attribIndex) const;

    void SetEndianSwapMode(void* pShader, int attribIndex, GX2EndianSwapMode mode);
    GX2EndianSwapMode GetEndianSwapMode(const void* pShader, int attribIndex) const;

    void SetDivisorSlot(void* pShader, int attribIndex, u32 slot);
    */
    u32 GetDivisorSlot(const void* pShader, int attribIndex) const;

    void SetOffset(void* pShader, int attribIndex, u32 offset);
    u32 GetOffset(const void* pShader, int attribIndex) const;

protected:
    u32* GetVFInst(void* pShader, int attribIndex);
    const u32* GetVFInst(const void* pShader, int attribIndex) const;

    void LoadVertexAttribArray(u32 idxAttrib, const GfxBuffer* pBuffer) const;
    void LoadVertexAttribArray() const;
    void LoadVertexAttribValue() const;
};

/*
void SetStreamOutEnable(GX2Boolean enable);

void SetRasterizerClipControl(GX2Boolean rasterizerEnable, GX2Boolean zClipEnable);
*/

void SetPrimitiveRestartIndex(u32 restartIndex);

/*
void ClearBuffers(GfxColorBuffer* pColorBuffer, GfxDepthBuffer* pDepthBuffer,
    float r, float g, float b, float a, GX2ClearMode mode);
*/

}}} // namespace nw::g3d::fnd

#endif // NW_G3D_FND_GFXOBJECT_H_
