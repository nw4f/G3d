#ifndef NW_G3D_RES_RESSCENEANIM_H_
#define NW_G3D_RES_RESSCENEANIM_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_ResAnimCurve.h>
#include <nw/g3d/res/g3d_Binding.h>
#include <nw/g3d/res/g3d_ResUserData.h>

namespace nw { namespace g3d { namespace res {

struct CameraAnimResult
{
    struct Projection
    {
        float nearZ;
        float farZ;
        float aspect;
        union
        {
            float height;
            float fovy;
        };
    } proj;
    struct View
    {
        float pos[3];
        union
        {
            float aim[3];
            float rotate[3];
        };
        float twist;
    } view;
};

class BindFuncTable;

struct ResCameraAnimData
{
    BinaryBlockHeader blockHeader;
    bit16 flag;

    s16 numFrame;
    u8 numCurve;
    u8 reserved;
    u16 numUserData;
    u32 bakedSize;
    BinString ofsName;
    Offset ofsCurveArray;
    Offset ofsBaseValueArray;
    Offset ofsUserDataDic;
};

class ResCameraAnim : private ResCameraAnimData
{
    NW_G3D_RES_COMMON(ResCameraAnim);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'C', 'A', 'M') };

    enum Flag
    {
        CURVE_BAKED         = AnimFlag::CURVE_BAKED,
        PLAYPOLICY_LOOP     = AnimFlag::PLAYPOLICY_LOOP,

        ROT_SHIFT           = 8,
        ROT_AIM             = 0x0 << ROT_SHIFT,
        ROT_EULER_ZXY       = 0x1 << ROT_SHIFT,
        ROT_MASK            = 0x1 << ROT_SHIFT,

        PROJ_SHIFT          = 10,
        PROJ_ORTHO          = 0x0 << PROJ_SHIFT,
        PROJ_PERSP          = 0x1 << PROJ_SHIFT,
        PROJ_MASK           = 0x1 << PROJ_SHIFT,
    };

    void Init(CameraAnimResult* pResult) const;

    bool BakeCurve(void* pBuffer, size_t bufferSize);

    void* ResetCurve();

    void Reset();

    void Eval(CameraAnimResult* pResult, float frame) const;

    void Eval(CameraAnimResult* pResult, float frame, AnimFrameCache* pFrameCache) const;

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResAnimCurve, Curve)

    bool IsLooped() const { return 0 != (ref().flag & PLAYPOLICY_LOOP); }

    bool IsCurveBaked() const { return 0 != (ref().flag & CURVE_BAKED); }

    int GetFrameCount() const { return ref().numFrame; }

    bit32 GetRotateMode() const { return ref().flag & ROT_MASK; }

    bit32 GetProjectionMode() const { return ref().flag & PROJ_MASK; }

    size_t GetBakedSize() const { return ref().bakedSize; }

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

struct ResLightAnimData
{
    // Unused in NSMBU
};

class ResLightAnim : private ResLightAnimData
{
    NW_G3D_RES_COMMON(ResLightAnim);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'L', 'I', 'T') };
};

struct ResFogAnimData
{
    // Unused in NSMBU
};

class ResFogAnim : private ResFogAnimData
{
    NW_G3D_RES_COMMON(ResFogAnim);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'F', 'O', 'G') };
};

struct ResSceneAnimData
{
    BinaryBlockHeader blockHeader;
    BinString ofsName;
    BinString ofsPath;

    u16 numUserData;
    u8 numCameraAnim;
    u8 reserved;      // ... ?
    u16 numLightAnim;
    u16 numFogAnim;

    Offset ofsCameraAnimDic;
    Offset ofsLightAnimDic;
    Offset ofsFogAnimDic;
    Offset ofsUserDataDic;
};

class ResSceneAnim : private ResSceneAnimData
{
    NW_G3D_RES_COMMON(ResSceneAnim);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'S', 'C', 'N') };

    // BindResult Bind(const BindFuncTable& funcTable);

    void Release();

    void Reset();

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_STRING_DECL(Path)

    NW_G3D_RES_FIELD_DIC_DECL(ResCameraAnim, CameraAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResLightAnim, LightAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResFogAnim, FogAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

class BindFuncTable
{
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESSCENEANIM_H_
