#ifndef NW_G3D_RES_RESMATERIAL_H_
#define NW_G3D_RES_RESMATERIAL_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/fnd/g3d_GfxObject.h>
#include <nw/g3d/math/g3d_Vector3.h>
#include <nw/g3d/math/g3d_Vector4.h>
#include <nw/g3d/math/g3d_Matrix34.h>
#include <nw/g3d/fnd/g3d_GfxState.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_Binding.h>
#include <nw/g3d/res/g3d_ResTexture.h>
#include <nw/g3d/res/g3d_ResUserData.h>

namespace nw { namespace g3d { namespace res {

class BindCallback;
class ResFile;

struct Srt2d
{
    float sx, sy, r, tx, ty;
};

struct Srt3d
{
    Vec3 scale, rotate, translate;
};

struct TexSrt
{
    enum Mode
    {
        MODE_MAYA,
        MODE_3DSMAX,
        MODE_SOFTIMAGE,
        NUM_MODE
    };

    Mode mode;
    float sx, sy, r, tx, ty;

};

struct TexSrtEx
{
    TexSrt srt;
    union
    {
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
        Mtx34* pEffectMtx;
#endif
        u32 padding;
    };
};

struct ResRenderInfoData
{
    u16 arrayLength;
    u8 type;
    u8 reserved;
    BinString ofsName;
    union
    {
        s32 iValue[1];
        f32 fValue[1];
        BinString ofsString[1];
    };
};

class ResRenderInfo : private ResRenderInfoData
{
    NW_G3D_RES_COMMON(ResRenderInfo);

public:
    enum Type
    {
        INT,
        FLOAT,
        STRING
    };

    NW_G3D_RES_FIELD_STRING_DECL(Name)

    int GetArrayLength() const { return ref().arrayLength; }

    Type GetType() const { return static_cast<Type>(ref().type); }

    const int* GetInt() const
    {
        NW_G3D_ASSERT(ref().type == INT);
        return reinterpret_cast<const int*>(ref().iValue);
    }

    const float* GetFloat() const
    {
        NW_G3D_ASSERT(ref().type == FLOAT);
        return reinterpret_cast<const float*>(ref().fValue);
    }

    const char* GetString(int strIndex) const
    {
        NW_G3D_ASSERT(ref().type == STRING);
        NW_G3D_ASSERT_INDEX_BOUNDS(strIndex, ref().arrayLength);
        return ref().ofsString[strIndex].to_ptr();
    }
};

typedef ResRenderInfoData ResEnvRefData;
typedef ResRenderInfo ResEnvRef;

struct ResRenderStateData
{
    bit32 flag;
    GfxPolygonCtrl_t polygonCtrl;
    GfxDepthCtrl_t depthCtrl;
    GfxAlphaTest_t alphaTest;
    GfxColorCtrl_t colorCtrl;
    GfxBlendCtrl_t blendCtrl;
    GfxBlendColor_t blendColor;
};

class ResRenderState : private ResRenderStateData
{
    NW_G3D_RES_COMMON(ResRenderState);

public:
    enum Flag
    {
        MODE_SHIFT          = 0,
        MODE_MASK           = 0x3 << MODE_SHIFT,
        BLEND_SHIFT         = 4,
        BLEND_MASK          = 0x3 << BLEND_SHIFT
    };

    enum Mode
    {
        MODE_CUSTOM         = 0,
        MODE_OPAQUE         = 1,
        MODE_ALPHAMASK      = 2,
        MODE_TRANSLUCENT    = 3,
    };

    enum BlendMode
    {
        BLEND_NONE          = 0,
        BLEND_COLOR         = 1,
        BLEND_LOGICAL       = 2,
    };

    void AdjustMode(Mode mode);

    void AdjustBlendMode(BlendMode mode);

    void Load() const;

    Mode GetMode() const { return static_cast<Mode>((ref().flag & MODE_MASK) >> MODE_SHIFT); }

    void SetMode(Mode mode)
    {
        ref().flag = (ref().flag & ~MODE_MASK) | (mode << MODE_SHIFT);
    }

    BlendMode GetBlendMode() const
    {
        return static_cast<BlendMode>((ref().flag & BLEND_MASK) >> BLEND_SHIFT);
    }

    void SetBlendMode(BlendMode mode)
    {
        ref().flag = (ref().flag & ~BLEND_MASK) | (mode << BLEND_SHIFT);
    }

    GfxPolygonCtrl& GetPolygonCtrl()
    {
        return GfxPolygonCtrl::DownCast(ref().polygonCtrl);
    }

    const GfxPolygonCtrl& GetPolygonCtrl() const
    {
        return GfxPolygonCtrl::DownCast(ref().polygonCtrl);
    }

    GfxDepthCtrl& GetDepthCtrl()
    {
        return GfxDepthCtrl::DownCast(ref().depthCtrl);
    }

    const GfxDepthCtrl& GetDepthCtrl() const
    {
        return GfxDepthCtrl::DownCast(ref().depthCtrl);
    }

    GfxAlphaTest& GetAlphaTest()
    {
        return GfxAlphaTest::DownCast(ref().alphaTest);
    }

    const GfxAlphaTest& GetAlphaTest() const
    {
        return GfxAlphaTest::DownCast(ref().alphaTest);
    }

    GfxColorCtrl& GetColorCtrl()
    {
        return GfxColorCtrl::DownCast(ref().colorCtrl);
    }

    const GfxColorCtrl& GetColorCtrl() const
    {
        return GfxColorCtrl::DownCast(ref().colorCtrl);
    }

    GfxBlendCtrl& GetBlendCtrl()
    {
        return GfxBlendCtrl::DownCast(ref().blendCtrl);
    }

    const GfxBlendCtrl& GetBlendCtrl() const
    {
        return GfxBlendCtrl::DownCast(ref().blendCtrl);
    }

    GfxBlendColor& GetBlendColor()
    {
        return GfxBlendColor::DownCast(ref().blendColor);
    }

    const GfxBlendColor& GetBlendColor() const
    {
        return GfxBlendColor::DownCast(ref().blendColor);
    }
};

struct ResShaderAssignData
{
    BinString ofsShaderArchiveName;
    BinString ofsShadingModelName;
    u32 revision;
    u8 numAttribAssign;
    u8 numSamplerAssign;
    u16 numShaderOption;
    Offset ofsAttribAssignDic;
    Offset ofsSamplerAssignDic;
    Offset ofsShaderOptionDic;
};

class ResShaderAssign : private ResShaderAssignData
{
    NW_G3D_RES_COMMON(ResShaderAssign);

public:
    NW_G3D_RES_FIELD_STRING_DECL(ShaderArchiveName)
    NW_G3D_RES_FIELD_STRING_DECL(ShadingModelName)

    NW_G3D_RES_FIELD_STR_DIC_DECL(AttribAssign)

    NW_G3D_RES_FIELD_STR_DIC_DECL(SamplerAssign)

    NW_G3D_RES_FIELD_STR_DIC_DECL(ShaderOption)
};

struct ResSamplerData
{
    GfxSampler_t gfxSampler;
    BinString ofsName;
    u8 index;
    u8 reserved0;
    u16 reserved1;
};

class ResSampler : private ResSamplerData
{
    NW_G3D_RES_COMMON(ResSampler);

public:
    NW_G3D_RES_FIELD_STRING_DECL(Name)

    void Setup();
    void Cleanup();

    int GetIndex() const { return ref().index; }

    GfxSampler* GetGfxSampler() { return GfxSampler::DownCast(&ref().gfxSampler); }

    const GfxSampler* GetGfxSampler() const { return GfxSampler::DownCast(&ref().gfxSampler); }
};

struct ResShaderParamData
{
    u8 type;
    u8 srcSize;
    u16 srcOffset;
    s32 offset;

    BinString ofsId;
};

class ResShaderParam : private ResShaderParamData
{
    NW_G3D_RES_COMMON(ResShaderParam);

public:
    enum Type
    {
        TYPE_BOOL, TYPE_BOOL2, TYPE_BOOL3, TYPE_BOOL4,
        TYPE_INT, TYPE_INT2, TYPE_INT3, TYPE_INT4,
        TYPE_UINT, TYPE_UINT2, TYPE_UINT3, TYPE_UINT4,
        TYPE_FLOAT, TYPE_FLOAT2, TYPE_FLOAT3, TYPE_FLOAT4,
        TYPE_RESERVED2, TYPE_FLOAT2x2, TYPE_FLOAT2x3, TYPE_FLOAT2x4,
        TYPE_RESERVED3, TYPE_FLOAT3x2, TYPE_FLOAT3x3, TYPE_FLOAT3x4,
        TYPE_RESERVED4, TYPE_FLOAT4x2, TYPE_FLOAT4x3, TYPE_FLOAT4x4,
        TYPE_SRT2D, TYPE_SRT3D, TYPE_TEXSRT, TYPE_TEXSRTEX,
        NUM_TYPE
    };

    typedef size_t (*ConvertParamCallback)(void* pDst, const void* pSrc, Type srcType, const void *pUserPtr);

    NW_G3D_RES_FIELD_STRING_DECL(Id);

    template <bool swap>
    void Convert(void* pDst, const void* pSrc) const;

    Type GetType() const { return static_cast<Type>(ref().type); }

    s32 GetOffset() const { return ref().offset; }

    void SetOffset(s32 offset) { ref().offset = offset; }

    s32 GetSrcOffset() const { return ref().srcOffset; }

    // size_t GetSize() const { return GetSize(GetType()); }

    size_t GetSrcSize() const { return ref().srcSize; }

    // static size_t GetSize(Type type);

    // static size_t GetSrcSize(Type type);
};

struct ResMaterialData
{
    BinaryBlockHeader blockHeader;

    BinString ofsName;

    bit32 flag;
    u16 index;
    u16 numRenderInfo;
    u8 numSampler;
    u8 numTexture;

    u16 numShaderParam;
    u16 numShaderParamVolatile;
    u16 srcParamSize;
    u16 rawParamSize;
    u16 numUserData;

    Offset ofsRenderInfoDic;
    Offset ofsRenderState;
    Offset ofsShaderAssign;
    Offset ofsTextureRefArray;
    Offset ofsSamplerArray;
    Offset ofsSamplerDic;
    Offset ofsShaderParamArray;
    Offset ofsShaderParamDic;
    Offset ofsSrcParam;
    Offset ofsUserDataDic;

    BinPtr pUserPtr;
};

class ResMaterial : private ResMaterialData
{
    NW_G3D_RES_COMMON(ResMaterial);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'M', 'A', 'T') };

    enum Flag
    {
        VISIBILITY                  = 0x1 << 0
    };

    void Setup();
    void Cleanup();
    void Reset();

    BindResult Bind(const ResFile* pFile);
    BindResult Bind(const ResFile* pFile, BindCallback* pCallback);
    bool ForceBind(const ResTexture* pTexture, const char* name);

    void Release();

    int GetIndex() const { return static_cast<int>(ref().index); }

    NW_G3D_RES_FIELD_STRING_DECL(Name)

    void SetUserPtr(void* pUserPtr) { ref().pUserPtr.set_ptr(pUserPtr); }

    void* GetUserPtr() { return ref().pUserPtr.to_ptr(); }

    const void* GetUserPtr() const { return ref().pUserPtr.to_ptr(); }

    template <typename T>
    T* GetUserPtr() { return ref().pUserPtr.to_ptr<T>(); }

    template <typename T>
    const T* GetUserPtr() const { return ref().pUserPtr.to_ptr<T>(); }

    int GetShaderParamVolatileCount() const { return ref().numShaderParamVolatile; }

    bool HasVolatile() const { return ref().numShaderParamVolatile > 0; }

    NW_G3D_RES_FIELD_DIC_DECL(ResRenderInfo, RenderInfo)

    NW_G3D_RES_FIELD_CLASS_DECL(ResRenderState, RenderState)

    void SetTextureCount(int textureCount)
    {
        NW_G3D_ASSERT(GetSamplerCount() <= textureCount);
        ref().numTexture = static_cast<u8>(textureCount);
    }

    int GetTextureCount() const { return ref().numTexture; }

    ResTextureRef* GetTextureRef(int texIndex)
    {
        NW_G3D_RES_ASSERT_INDEX(Sampler, texIndex);
        ResTextureRefData* pArray = ref().ofsTextureRefArray.to_ptr<ResTextureRefData>();
        return ResTextureRef::ResCast(&pArray[texIndex]);
    }

    const ResTextureRef* GetTextureRef(int texIndex) const
    {
        NW_G3D_RES_ASSERT_INDEX(Sampler, texIndex);
        const ResTextureRefData* pArray = ref().ofsTextureRefArray.to_ptr<ResTextureRefData>();
        return ResTextureRef::ResCast(&pArray[texIndex]);
    }

    NW_G3D_RES_FIELD_CLASS_NAMED_ARRAY_DECL(ResSampler, Sampler)

    void* GetSrcParam(int paramIndex)
    {
        return AddOffset(ref().ofsSrcParam.to_ptr(), GetShaderParam(paramIndex)->GetSrcOffset());
    }

    const void* GetSrcParam(int paramIndex) const
    {
        return AddOffset(ref().ofsSrcParam.to_ptr(), GetShaderParam(paramIndex)->GetSrcOffset());
    }

    template <typename T>
    T* GetSrcParam(int paramIndex)
    {
        const ResShaderParam* pParam = GetShaderParam(paramIndex);
        NW_G3D_ASSERT(sizeof(T) <= pParam->GetSrcSize());
        return AddOffset<T>(ref().ofsSrcParam.to_ptr(), pParam->GetSrcOffset());
    }

    template <typename T>
    const T* GetSrcParam(int paramIndex) const
    {
        const ResShaderParam* pParam = GetShaderParam(paramIndex);
        NW_G3D_ASSERT(sizeof(T) <= pParam->GetSrcSize());
        return AddOffset<T>(ref().ofsSrcParam.to_ptr(), pParam->GetSrcOffset());
    }

    u32 GetSrcParamSize() const { return ref().srcParamSize; }

    u32 GetRawParamSize() const { return ref().rawParamSize; }

    void SetRawParamSize(u32 size) { ref().rawParamSize = static_cast<u16>(size); }

    NW_G3D_RES_FIELD_CLASS_NAMED_ARRAY_DECL(ResShaderParam, ShaderParam)

    void BuildShaderParam();

    NW_G3D_RES_FIELD_CLASS_DECL(ResShaderAssign, ShaderAssign)

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESMATERIAL_H_
