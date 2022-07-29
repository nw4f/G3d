#ifndef NW_G3D_RES_RESSKELETALANIM_H_
#define NW_G3D_RES_RESSKELETALANIM_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_ResAnimCurve.h>
#include <nw/g3d/res/g3d_ResSkeleton.h>
#include <nw/g3d/res/g3d_Binding.h>

namespace nw { namespace g3d { namespace res {

struct BoneAnimResult
{
    bit32 flag;
    Vec3 scale;
    Vec3 translate;
    u32 reserved;
    union
    {
        Vec3 euler;
        Quat quat;
    };
    Vec3 axes[2];
};

struct BoneAnimBlendResult
{
    bit32 flag;
    Vec3 scale;
    Vec3 translate;
    float weight;

    union
    {
        Vec3 axes[2];
        Quat rotate;
    };
};

struct ResBoneAnimData
{
    bit32 flag;
    BinString ofsName;
    u8 beginRotate;
    u8 beginTranslate;
    u8 numCurve;
    u8 beginBaseTranslate;
    s32 beginCurve;
    Offset ofsCurveArray;
    Offset ofsBaseValueArray;
};

class ResBoneAnim : private ResBoneAnimData
{
    NW_G3D_RES_COMMON(ResBoneAnim);

public:
    enum Flag
    {
        BASE_SCALE                  = 0x1 << 3,
        BASE_ROTATE                 = 0x1 << 4,
        BASE_TRANSLATE              = 0x1 << 5,
        BASE_MASK                   = BASE_SCALE | BASE_ROTATE | BASE_TRANSLATE,

        CURVE_SCALE_X               = 0x1 << 6,
        CURVE_SCALE_Y               = 0x1 << 7,
        CURVE_SCALE_Z               = 0x1 << 8,
        CURVE_ROTATE_X              = 0x1 << 9,
        CURVE_ROTATE_Y              = 0x1 << 10,
        CURVE_ROTATE_Z              = 0x1 << 11,
        CURVE_ROTATE_W              = 0x1 << 12,
        CURVE_TRANSLATE_X           = 0x1 << 13,
        CURVE_TRANSLATE_Y           = 0x1 << 14,
        CURVE_TRANSLATE_Z           = 0x1 << 15,
        CURVE_SCALE_MASK            = CURVE_SCALE_X | CURVE_SCALE_Y | CURVE_SCALE_Z,
        CURVE_ROTATE_MASK           = CURVE_ROTATE_X | CURVE_ROTATE_Y | CURVE_ROTATE_Z | CURVE_ROTATE_W,
        CURVE_TRANSLATE_MASK        = CURVE_TRANSLATE_X | CURVE_TRANSLATE_Y | CURVE_TRANSLATE_Z,
        CURVE_MASK                  = CURVE_SCALE_MASK | CURVE_ROTATE_MASK | CURVE_TRANSLATE_MASK,

        SEGMENT_SCALE_COMPENSATE    = ResBone::SEGMENT_SCALE_COMPENSATE,
        SCALE_UNIFORM               = ResBone::SCALE_UNIFORM,
        SCALE_VOLUME_ONE            = ResBone::SCALE_VOLUME_ONE,
        ROTATE_ZERO                 = ResBone::ROTATE_ZERO,
        TRANSLATE_ZERO              = ResBone::TRANSLATE_ZERO,
        SCALE_ONE                   = ResBone::SCALE_ONE,
        ROTTRANS_ZERO               = ResBone::ROTTRANS_ZERO,
        IDENTITY                    = ResBone::IDENTITY,
        TRANSFORM_MASK              = ResBone::TRANSFORM_MASK
    };

    void Init(BoneAnimResult* pResult, const ResBone* bone) const;

    void Eval(BoneAnimResult* pResult, float frame) const;

    void Eval(BoneAnimResult* pResult, float frame, AnimFrameCache* pFrameCache) const;

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResAnimCurve, Curve)
};

struct ResSkeletalAnimData
{
    BinaryBlockHeader blockHeader;
    BinString ofsName;
    BinString ofsPath;

    bit32 flag;
    s16 numFrame;
    u16 numBoneAnim;
    s32 numCurve;
    u32 bakedSize;
    u16 numUserData;
    u16 reserved;

    Offset ofsBoneAnimArray;
    Offset ofsBindSkeleton;
    Offset ofsBindIndexArray;
    Offset ofsUserDataDic;
};

class ResSkeletalAnim : private ResSkeletalAnimData
{
    NW_G3D_RES_COMMON(ResSkeletalAnim);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'S', 'K', 'A') };

    enum Flag
    {
        CURVE_BAKED         = AnimFlag::CURVE_BAKED,
        PLAYPOLICY_LOOP     = AnimFlag::PLAYPOLICY_LOOP,

        SCALE_SHIFT         = ResSkeleton::SCALE_SHIFT,
        SCALE_NONE          = ResSkeleton::SCALE_NONE,
        SCALE_STD           = ResSkeleton::SCALE_STD,
        SCALE_MAYA          = ResSkeleton::SCALE_MAYA,
        SCALE_SOFTIMAGE     = ResSkeleton::SCALE_SOFTIMAGE,
        SCALE_MASK          = ResSkeleton::SCALE_MASK,

        ROT_SHIFT           = ResSkeleton::ROT_SHIFT,
        ROT_EULER_XYZ       = ResSkeleton::ROT_EULER_XYZ,
        ROT_QUAT            = ResSkeleton::ROT_QUAT,
        ROT_MASK            = ResSkeleton::ROT_MASK,
        TRANSFORM_MASK      = ResSkeleton::TRANSFORM_MASK
    };

    BindResult PreBind(const ResSkeleton* pSkeleton);

    bool BakeCurve(void* pBuffer, size_t bufferSize);

    void* ResetCurve();

    void Reset();

    bool IsLooped() const { return 0 != (ref().flag & PLAYPOLICY_LOOP); }

    bool IsCurveBaked() const { return 0 != (ref().flag & CURVE_BAKED); }

    int GetScaleMode() const { return ref().flag & SCALE_MASK; }

    int GetRotateMode() const { return ref().flag & ROT_MASK; }

    int GetCurveCount() const { return ref().numCurve; }

    int GetFrameCount() const { return ref().numFrame; }

    size_t GetBakedSize() const { return ref().bakedSize; }

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_STRING_DECL(Path)
    NW_G3D_RES_FIELD_CLASS_DECL(ResSkeleton, BindSkeleton)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResBoneAnim, BoneAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESSKELETALANIM_H_
