#include <nw/g3d/fnd/g3d_GfxObject.h>
#include <functional>
#include <algorithm>
#include <nw/g3d/fnd/g3d_GfxManage.h>
#include <nw/g3d/ut/g3d_Inlines.h>
#include <nw/g3d/ut/g3d_Flag.h>
#include <nw/g3d/math/g3d_MathCommon.h>

namespace nw { namespace g3d { namespace fnd {

namespace {

enum RegSampler0
{
    NW_G3D_FLAG_VALUE_DECLARE( 0,  3, CLAMP_X),
    NW_G3D_FLAG_VALUE_DECLARE( 3,  3, CLAMP_Y),
    NW_G3D_FLAG_VALUE_DECLARE( 6,  3, CLAMP_Z),
    NW_G3D_FLAG_VALUE_DECLARE( 9,  2, XY_MAG_FILTER),
    NW_G3D_FLAG_VALUE_DECLARE(12,  2, XY_MIN_FILTER),
    NW_G3D_FLAG_VALUE_DECLARE(15,  2, Z_FILTER),
    NW_G3D_FLAG_VALUE_DECLARE(17,  2, MIP_FILTER),
    NW_G3D_FLAG_VALUE_DECLARE(19,  3, MAX_ANISO_RATIO),
    NW_G3D_FLAG_VALUE_DECLARE(22,  2, BORDER_COLOR_TYPE),
    NW_G3D_FLAG_VALUE_DECLARE(26,  3, DEPTH_COMPARE_FUNCTION)
};

enum RegSampler1
{
    NW_G3D_FLAG_VALUE_DECLARE( 0, 10, MIN_LOD),
    NW_G3D_FLAG_VALUE_DECLARE(10, 10, MAX_LOD),
    NW_G3D_FLAG_VALUE_DECLARE(20, 12, LOD_BIAS)
};

enum RegSampler2
{
    NW_G3D_FLAG_VALUE_DECLARE(30,  1, DEPTH_COMPARE_ENABLE)
};

enum InstCFlow1
{
    NW_G3D_FLAG_VALUE_DECLARE(10,  3, COUNT),
    NW_G3D_FLAG_VALUE_DECLARE(19,  1, COUNT_3)
};

enum InstVFetch0
{
    NW_G3D_FLAG_VALUE_DECLARE( 5,  2, FETCH_TYPE),
    NW_G3D_FLAG_VALUE_DECLARE( 8,  4, BUFFER_ID),
    NW_G3D_FLAG_VALUE_DECLARE(24,  2, SRC_SEL_X),
    NW_G3D_FLAG_VALUE_DECLARE(26,  6, MEGA_FETCH_COUNT)
};

enum InstVFetch1
{
    NW_G3D_FLAG_VALUE_DECLARE( 0,  8, SEMANTIC_ID),
    NW_G3D_FLAG_VALUE_DECLARE( 9,  3, DST_SEL_X),
    NW_G3D_FLAG_VALUE_DECLARE(12,  3, DST_SEL_Y),
    NW_G3D_FLAG_VALUE_DECLARE(15,  3, DST_SEL_Z),
    NW_G3D_FLAG_VALUE_DECLARE(18,  3, DST_SEL_W),
    NW_G3D_FLAG_VALUE_DECLARE(22,  6, DATA_FORMAT),
    NW_G3D_FLAG_VALUE_DECLARE(28,  2, NUM_FORMAT_ALL),
    NW_G3D_FLAG_VALUE_DECLARE(30,  1, FORMAT_COMP_ALL)
};

enum InstVFetch2
{
    NW_G3D_FLAG_VALUE_DECLARE( 0, 16, OFFSET),
    NW_G3D_FLAG_VALUE_DECLARE(16,  2, ENDIAN_SWAP)
};

NW_G3D_FORCE_INLINE
u32 ReadInst(const u32* pInst, int instIndex)
{
    return LoadRevU32(pInst + instIndex);
}

NW_G3D_FORCE_INLINE
void WriteInst(u32* pInst, int instIndex, u32 value)
{
    StoreRevU32(pInst + instIndex, value);
}

NW_G3D_INLINE
float U4_6ToFloat(u32 x)
{
    return FastCast<float>(static_cast<u16>(x)) / 64.0f;
}

NW_G3D_INLINE
float S5_6ToFloat(u32 x)
{
    union
    {
        u32 u;
        s32 s;
    } x32;
    x32.u = x << 20;
    return FastCast<float>(static_cast<s16>(x32.s >> 20)) / 64.0f;
}

#define GX2_SURFACE_FORMAT_T_R32_G32_B32_UINT       0x0000012f
#define GX2_SURFACE_FORMAT_T_R32_G32_B32_SINT       0x0000032f
#define GX2_SURFACE_FORMAT_T_R32_G32_B32_FLOAT      0x00000830

#define NW_G3D_DEF_FMT(attrib, surface, ...) \
    { static_cast<u8>(GX2_ATTRIB_FORMAT_##attrib), static_cast<u8>(GX2_SURFACE_FORMAT_##surface), __VA_ARGS__ }

const struct InternalFormat
{
    u8 attribFormat;
    u8 surfaceFormat;
    u8 fetchSize;
    u8 endianSwap;
    GX2CompSel compSel;
} s_InternalFormat[] = {
    NW_G3D_DEF_FMT(8_UNORM,             TC_R8_UNORM,                0,  0, GX2_COMP_SEL_X001),
    NW_G3D_DEF_FMT(4_4_UNORM,           T_R4_G4_UNORM,              0,  0, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(16_UNORM,            TCD_R16_UNORM,              1,  1, GX2_COMP_SEL_X001),
    NW_G3D_DEF_FMT(16_FLOAT,            TC_R16_FLOAT,               1,  1, GX2_COMP_SEL_X001),
    NW_G3D_DEF_FMT(8_8_UNORM,           TC_R8_G8_UNORM,             1,  0, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(32_UINT,             TC_R32_UINT,                3,  2, GX2_COMP_SEL_X001),
    NW_G3D_DEF_FMT(32_FLOAT,            TCD_R32_FLOAT,              3,  2, GX2_COMP_SEL_X001),
    NW_G3D_DEF_FMT(16_16_UNORM,         TC_R16_G16_UNORM,           3,  1, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(16_16_FLOAT,         TC_R16_G16_FLOAT,           3,  1, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(10_11_11_FLOAT,      TC_R11_G11_B10_FLOAT,       3,  2, GX2_COMP_SEL_XYZ1),
    NW_G3D_DEF_FMT(8_8_8_8_UNORM,       TCS_R8_G8_B8_A8_UNORM,      3,  0, GX2_COMP_SEL_XYZW),
    NW_G3D_DEF_FMT(10_10_10_2_UNORM,    TCS_R10_G10_B10_A2_UNORM,   3,  2, GX2_COMP_SEL_XYZW),
    NW_G3D_DEF_FMT(32_32_UINT,          TC_R32_G32_UINT,            7,  2, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(32_32_FLOAT,         TC_R32_G32_FLOAT,           7,  2, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(16_16_16_16_UNORM,   TC_R16_G16_B16_A16_UNORM,   7,  1, GX2_COMP_SEL_XYZW),
    NW_G3D_DEF_FMT(16_16_16_16_FLOAT,   TC_R16_G16_B16_A16_FLOAT,   7,  1, GX2_COMP_SEL_XYZW),
    NW_G3D_DEF_FMT(32_32_32_UINT,       T_R32_G32_B32_UINT,         11, 2, GX2_COMP_SEL_XYZ1),
    NW_G3D_DEF_FMT(32_32_32_FLOAT,      T_R32_G32_B32_FLOAT,        11, 2, GX2_COMP_SEL_XYZ1),
    NW_G3D_DEF_FMT(32_32_32_32_UINT,    TC_R32_G32_B32_A32_UINT,    15, 2, GX2_COMP_SEL_XYZW),
    NW_G3D_DEF_FMT(32_32_32_32_FLOAT,   TC_R32_G32_B32_A32_FLOAT,   15, 2, GX2_COMP_SEL_XYZW)
};

enum
{
    ATTRIB_BIT_INT          = 0x100,
    ATTRIB_BIT_SIGNED       = 0x200,
    ATTRIB_BIT_FLOAT        = 0x800,
    ATTRIB_FMT_NORMALIZED   = 0,
    ATTRIB_FMT_INT          = 1,
    ATTRIB_FMT_SCALED       = 2
};

}

GfxBuffer::GfxBuffer()
{
    memset(static_cast<GfxBuffer_t*>(this), 0, sizeof(GfxBuffer_t));
}

void GfxBuffer::Setup()
{
}

void GfxBuffer::Cleanup()
{
}

void GfxBuffer::UpdateRegs()
{
}

void GfxBuffer::DCFlush(int bufferIndex /*= 0*/) const
{
    NW_G3D_ASSERT_INDEX_BOUNDS(bufferIndex, numBuffering);
#if NW_G3D_FORCE_PPC_SYNC
    DCFlushRange(GetData(bufferIndex), size);
#else
    DCFlushRangeNoSync(GetData(bufferIndex), size);
#endif // NW_G3D_FORCE_PPC_SYNC
}

void GfxBuffer::LoadIndices() const
{
}

void GfxBuffer::LoadVertices(u32 slot) const
{
    GX2SetAttribBuffer(slot, size, stride, pData);
}

void GfxBuffer::LoadVertexUniforms(u32 location, int bufferIndex /*= 0*/) const
{
    NW_G3D_ASSERT_INDEX_BOUNDS(bufferIndex, numBuffering);
    NW_G3D_ASSERT(size > 0);
    GX2SetVertexUniformBlock(location, size, GetData(bufferIndex));
}

void GfxBuffer::LoadGeometryUniforms(u32 location, int bufferIndex /*= 0*/) const
{
    NW_G3D_ASSERT_INDEX_BOUNDS(bufferIndex, numBuffering);
    NW_G3D_ASSERT(size > 0);
    GX2SetGeometryUniformBlock(location, size, GetData(bufferIndex));
}

void GfxBuffer::LoadFragmentUniforms(u32 location, int bufferIndex /*= 0*/) const
{
    NW_G3D_ASSERT_INDEX_BOUNDS(bufferIndex, numBuffering);
    NW_G3D_ASSERT(size > 0);
    GX2SetPixelUniformBlock(location, size, GetData(bufferIndex));
}

void GfxBuffer::SetData(void* pData, u32 size, int bufferingCount /*= 1*/)
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    NW_G3D_ASSERT(bufferingCount > 0);
    this->pData = size > 0 ? pData : NULL;
    this->size = size;
    this->numBuffering = static_cast<u16>(bufferingCount);
#else
    NW_G3D_UNUSED(pData);
    NW_G3D_UNUSED(size);
    NW_G3D_UNUSED(bufferingCount);
#endif
}

void* GfxBuffer::GetData(int bufferIndex /*= 0*/)
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    NW_G3D_ASSERT_INDEX_BOUNDS( bufferIndex, numBuffering );
    return AddOffset(pData, size * bufferIndex);
#else
    NW_G3D_UNUSED(bufferIndex);
    return NULL;
#endif
}

const void* GfxBuffer::GetData(int bufferIndex /*= 0*/) const
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    NW_G3D_ASSERT_INDEX_BOUNDS( bufferIndex, numBuffering );
    return AddOffset(pData, size * bufferIndex);
#else
    NW_G3D_UNUSED(bufferIndex);
    return NULL;
#endif
}
void GfxSampler::Setup()
{
}

void GfxSampler::Cleanup()
{
}

void GfxSampler::UpdateRegs()
{
}

void GfxSampler::LoadVertexSampler(u32 unit) const
{
    GX2SetVertexSampler(GetGX2Sampler(), unit);
}

void GfxSampler::LoadGeometrySampler(u32 unit) const
{
    GX2SetGeometrySampler(GetGX2Sampler(), unit);
}

void GfxSampler::LoadFragmentSampler(u32 unit) const
{
    GX2SetPixelSampler(GetGX2Sampler(), unit);
}

GX2TexClamp GfxSampler::GetClampX() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], CLAMP_X, GX2TexClamp);
}

GX2TexClamp GfxSampler::GetClampY() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], CLAMP_Y, GX2TexClamp);
}

GX2TexClamp GfxSampler::GetClampZ() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], CLAMP_Z, GX2TexClamp);
}

GX2TexBorderType GfxSampler::GetBorderType() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], BORDER_COLOR_TYPE, GX2TexBorderType);
}

GX2TexXYFilterType GfxSampler::GetMagFilter() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], XY_MAG_FILTER, GX2TexXYFilterType);
}

GX2TexXYFilterType GfxSampler::GetMinFilter() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], XY_MIN_FILTER, GX2TexXYFilterType);
}

GX2TexMipFilterType GfxSampler::GetMipFilter() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], MIP_FILTER, GX2TexMipFilterType);
}

GX2TexAnisoRatio GfxSampler::GetMaxAniso() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], MAX_ANISO_RATIO, GX2TexAnisoRatio);
}

float GfxSampler::GetMinLOD() const
{
    u32 val = NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[1], MIN_LOD, u32);
    return U4_6ToFloat(val);
}

float GfxSampler::GetMaxLOD() const
{
    u32 val = NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[1], MAX_LOD, u32);
    return U4_6ToFloat(val);
}

float GfxSampler::GetLODBias() const
{
    u32 val = NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[1], LOD_BIAS, u32);
    return S5_6ToFloat(val);
}

GX2Boolean GfxSampler::GetCompareEnable() const
{
    return GX2_FALSE;
}

GX2CompareFunction GfxSampler::GetCompareFunc() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0],
        DEPTH_COMPARE_FUNCTION, GX2CompareFunction);
}

void GfxTexture::Setup()
{
    UpdateRegs();
}

void GfxTexture::Cleanup()
{
}

void GfxTexture::UpdateRegs()
{
    GX2InitTextureRegs(GetGX2Texture());
}

void GfxTexture::DCFlush() const
{
#if NW_G3D_FORCE_PPC_SYNC
    DCFlushRange(GetBasePtr(), GetBaseSize());
    if (GetMipPtr())
    {
        DCFlushRange(GetMipPtr(), GetMipSize());
    }
#else
    DCFlushRangeNoSync(GetBasePtr(), GetBaseSize());
    if (GetMipPtr())
    {
        DCFlushRangeNoSync(GetMipPtr(), GetMipSize());
    }
#endif // NW_G3D_FORCE_PPC_SYNC
}

void GfxTexture::SetImagePtrs(void* basePtr, void* mipPtr)
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    GX2Texture* pTexture = GetGX2Texture();
    pTexture->surface.imagePtr = basePtr;
    if (pTexture->surface.numMips > 1 && mipPtr == NULL && basePtr) {
        pTexture->surface.mipPtr = AddOffset(basePtr, pTexture->surface.mipOffset[0]);
    } else {
        pTexture->surface.mipPtr = mipPtr;
    }
#else
    NW_G3D_UNUSED(basePtr);
    NW_G3D_UNUSED(mipPtr);
    gx2Texture.surface.imagePtr = 0;
    gx2Texture.surface.mipPtr = 0;
#endif
}

const void* GfxTexture::GetBasePtr() const
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    return GetGX2Texture()->surface.imagePtr;
#else
    return NULL;
#endif
}

const void* GfxTexture::GetMipPtr() const
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    return GetGX2Texture()->surface.mipPtr;
#else
    return NULL;
#endif
}

void GfxTexture::LoadVertexTexture(u32 unit) const
{
    GX2SetVertexTexture(GetGX2Texture(), unit);
}

void GfxTexture::LoadGeometryTexture(u32 unit) const
{
    GX2SetGeometryTexture(GetGX2Texture(), unit);
}

void GfxTexture::LoadFragmentTexture(u32 unit) const
{
    GX2SetPixelTexture(GetGX2Texture(), unit);
}

GfxFetchShader::GfxFetchShader()
{
    memset(this, 0, sizeof(*this));
}

void GfxFetchShader::Setup()
{
}

void GfxFetchShader::Cleanup()
{
}

void GfxFetchShader::DCFlush() const
{
#if NW_G3D_FORCE_PPC_SYNC
    DCFlushRange(GetShaderPtr(), GetShaderSize());
#else
    DCFlushRangeNoSync(GetShaderPtr(), GetShaderSize());
#endif // NW_G3D_FORCE_PPC_SYNC
}

void GfxFetchShader::CalcSize()
{
    u32 sizeCFInst = static_cast<u32>((Align(gx2FetchShader.numAttribs, MAX_INST_PER_FETCH_CLAUSE) /
        MAX_INST_PER_FETCH_CLAUSE + 1) * CF_INST_SIZE);
    ofsVFInst = static_cast<u32>(Align(sizeCFInst, FETCH_INST_ALIGNMENT));
    gx2FetchShader.shaderSize = ofsVFInst + gx2FetchShader.numAttribs * VF_INST_SIZE;
}

void GfxFetchShader::Load() const
{
    GX2SetFetchShader(GetGX2FetchShader());
}

void GfxFetchShader::SetVertexBuffer(int attribIndex, const GfxBuffer* pBuffer)
{
    (void)pBuffer;
    u32* pVFetchInst = GetVFInst(attribIndex);
    pVFetchInst[3] = 0;
}

const GfxBuffer* GfxFetchShader::GetVertexBuffer(int attribIndex) const
{
    const u32* pVFetchInst = GetVFInst(attribIndex);
    return reinterpret_cast<const GfxBuffer*>(pVFetchInst[3]);
}

void GfxFetchShader::SetShaderPtr(void* ptr)
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    NW_G3D_ASSERT_ADDR_ALIGNMENT(ptr, SHADER_ALIGNMENT);
    GetGX2FetchShader()->shaderPtr = ptr;
#else
    NW_G3D_UNUSED(ptr);
    gx2FetchShader.shaderPtr = 0;
#endif
}

void* GfxFetchShader::GetShaderPtr()
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    return GetGX2FetchShader()->shaderPtr;
#else
    return NULL;
#endif
}

const void* GfxFetchShader::GetShaderPtr() const
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    return GetGX2FetchShader()->shaderPtr;
#else
    return NULL;
#endif
}

void GfxFetchShader::SetDefault(void* pShader)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    NW_G3D_ASSERT_ADDR_ALIGNMENT(pShader, SHADER_ALIGNMENT);

    u32 numAttrib = gx2FetchShader.numAttribs;
    u32 numCFlowInst = (numAttrib + MAX_INST_PER_FETCH_CLAUSE - 1) >> 4;
    u32* pCFlowInst = static_cast<u32*>(pShader);
    for (u32 idxCFlowInst = 0; idxCFlowInst < numCFlowInst; ++idxCFlowInst)
    {
        u32 inst0 = (ofsVFInst + idxCFlowInst * MAX_INST_PER_FETCH_CLAUSE * VF_INST_SIZE) >> 3;
        WriteInst(pCFlowInst++, 0, inst0);
        if (numAttrib > (idxCFlowInst + 1) * MAX_INST_PER_FETCH_CLAUSE)
        {
            WriteInst(pCFlowInst++, 0, 0x01881C00);
        }
        else
        {
            u32 inst1 = 0x01800000;
            NW_G3D_SET_FLAG_VALUE(inst1, COUNT, (numAttrib - 1));
            NW_G3D_SET_FLAG_VALUE(inst1, COUNT_3, (numAttrib - 1) >> 3);
            WriteInst(pCFlowInst++, 0, inst1);
        }
    }
    WriteInst(pCFlowInst++, 0, 0);
    WriteInst(pCFlowInst++, 0, 0x8A000000);
    memset(pCFlowInst, 0, CF_INST_SIZE);

    for (u32 idxAttrib = 0; idxAttrib < numAttrib; ++idxAttrib)
    {
        u32* pVFetchInst = GetVFInst(pShader, idxAttrib);
        WriteInst(pVFetchInst, 0, 0x3C00A001);
        WriteInst(pVFetchInst, 1, 0x28D64800);
        WriteInst(pVFetchInst, 2, 0x000A0000);
        WriteInst(pVFetchInst, 3, 0);
    }
}

void GfxFetchShader::SetLocation(void* pShader, int attribIndex, u32 location)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst1 = ReadInst(pVFetchInst, 1);
    NW_G3D_SET_FLAG_VALUE(inst1, SEMANTIC_ID, location);
    WriteInst(pVFetchInst, 1, inst1);
}

u32 GfxFetchShader::GetLocation(const void* pShader, int attribIndex) const
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    const u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst1 = ReadInst(pVFetchInst, 1);
    return NW_G3D_GET_FLAG_VALUE(inst1, SEMANTIC_ID, u32);
}

void GfxFetchShader::SetBufferSlot(void* pShader, int attribIndex, u32 slot)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    NW_G3D_ASSERT_INDEX_BOUNDS(slot, GX2_MAX_ATTRIB_BUFFERS);
    u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst0 = ReadInst(pVFetchInst, 0);
    NW_G3D_SET_FLAG_VALUE(inst0, BUFFER_ID, slot);
    WriteInst(pVFetchInst, 0, inst0);
}

void GfxFetchShader::SetFormat(void* pShader, int attribIndex, GX2AttribFormat format)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    const InternalFormat& fmt = s_InternalFormat[format & 0x3F];

    u32 inst0 = ReadInst(pVFetchInst, 0);
    NW_G3D_SET_FLAG_VALUE(inst0, MEGA_FETCH_COUNT, fmt.fetchSize);
    WriteInst(pVFetchInst, 0, inst0);

    u32 inst1 = ReadInst(pVFetchInst, 1);
    u32 fmtType = (format & ATTRIB_BIT_FLOAT) ? ATTRIB_FMT_SCALED :
        (format & ATTRIB_BIT_INT) ? ATTRIB_FMT_INT : ATTRIB_FMT_NORMALIZED;
    NW_G3D_SET_FLAG_VALUE(inst1, NUM_FORMAT_ALL, fmtType);
    NW_G3D_SET_FLAG_VALUE(inst1, DST_SEL_X, GX2_GET_COMPONENT_X_R(fmt.compSel));
    NW_G3D_SET_FLAG_VALUE(inst1, DST_SEL_Y, GX2_GET_COMPONENT_Y_G(fmt.compSel));
    NW_G3D_SET_FLAG_VALUE(inst1, DST_SEL_Z, GX2_GET_COMPONENT_Z_B(fmt.compSel));
    NW_G3D_SET_FLAG_VALUE(inst1, DST_SEL_W, GX2_GET_COMPONENT_W_A(fmt.compSel));
    NW_G3D_SET_FLAG_VALUE(inst1, FORMAT_COMP_ALL,  format & ATTRIB_BIT_SIGNED ? 1 : 0);
    NW_G3D_SET_FLAG_VALUE(inst1, DATA_FORMAT, fmt.surfaceFormat);
    WriteInst(pVFetchInst, 1, inst1);

    u32 inst2 = ReadInst(pVFetchInst, 2);
    NW_G3D_SET_FLAG_VALUE(inst2, ENDIAN_SWAP, fmt.endianSwap);
    WriteInst(pVFetchInst, 2, inst2);
}

u32 GfxFetchShader::GetDivisorSlot(const void* pShader, int attribIndex) const
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    static const u8 tblDivisor[] = { 0, 2, 3, 1 };
    const u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst0 = ReadInst(pVFetchInst, 0);
    return tblDivisor[NW_G3D_GET_FLAG_VALUE(inst0, SRC_SEL_X, u32)];
}

void GfxFetchShader::SetOffset(void* pShader, int attribIndex, u32 offset)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst2 = ReadInst(pVFetchInst, 2);
    NW_G3D_SET_FLAG_VALUE(inst2, OFFSET, offset);
    WriteInst(pVFetchInst, 2, inst2);
}

u32 GfxFetchShader::GetOffset(const void* pShader, int attribIndex) const
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    const u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst2 = ReadInst(pVFetchInst, 2);
    return NW_G3D_GET_FLAG_VALUE(inst2, OFFSET, u32);
}

u32* GfxFetchShader::GetVFInst(void* pShader, int attribIndex)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    return AddOffset<u32>(pShader, ofsVFInst + attribIndex * VF_INST_SIZE);
}

const u32* GfxFetchShader::GetVFInst(const void* pShader, int attribIndex) const
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    return AddOffset<u32>(pShader, ofsVFInst + attribIndex * VF_INST_SIZE);
}

void GfxFetchShader::LoadVertexAttribArray(u32 idxAttrib, const GfxBuffer* pBuffer) const
{
    NW_G3D_UNUSED(idxAttrib);
    NW_G3D_UNUSED(pBuffer);
}

void GfxFetchShader::LoadVertexAttribArray() const
{
}

void GfxFetchShader::LoadVertexAttribValue() const
{
}

void SetPrimitiveRestartIndex(u32 restartIndex)
{
    GX2SetPrimitiveRestartIndex(restartIndex);
}

} } } // namespace nw::g3d::fnd
