#ifndef NW_G3D_RES_RESVISIBILITYANIM_H_
#define NW_G3D_RES_RESVISIBILITYANIM_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_ResAnimCurve.h>
#include <nw/g3d/res/g3d_ResModel.h>
#include <nw/g3d/res/g3d_Binding.h>

namespace nw { namespace g3d { namespace res {

struct ResVisibilityAnimData
{
    BinaryBlockHeader blockHeader;
    BinString ofsName;
    BinString ofsPath;

    bit16 flag;
    s16 numFrame;
    u16 numAnim;
    u16 numCurve;
    u32 bakedSize;
    u16 numUserData;
    u16 reserved;

    Offset ofsBindModel;
    Offset ofsBindIndexArray;
    Offset ofsNameArray;
    Offset ofsCurveArray;
    Offset ofsBaseValueArray;
    Offset ofsUserDataDic;
};

class ResVisibilityAnim : private ResVisibilityAnimData
{
    NW_G3D_RES_COMMON(ResVisibilityAnim);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'V', 'I', 'S') };

    enum Flag
    {
        CURVE_BAKED         = AnimFlag::CURVE_BAKED,
        PLAYPOLICY_LOOP     = AnimFlag::PLAYPOLICY_LOOP,

        TYPE_SHIFT          = 8,
        BONE_VISIBILITY     = 0x0 << TYPE_SHIFT,
        MAT_VISIBILITY      = 0x1 << TYPE_SHIFT,
        TYPE_MASK           = 0x1 << TYPE_SHIFT
    };

    BindResult PreBind(const ResModel* pModel);

    bool BakeCurve(void* pBuffer, size_t bufferSize);

    void* ResetCurve();

    void Reset();

    int GetVisibilityType() const { return ref().flag & TYPE_MASK; }

    bool IsLooped() const { return 0 != (ref().flag & PLAYPOLICY_LOOP); }

    bool IsCurveBaked() const { return 0 != (ref().flag & CURVE_BAKED); }

    int GetAnimCount() const { return ref().numAnim; }

    int GetFrameCount() const { return ref().numFrame; }

    size_t GetBakedSize() const { return ref().bakedSize; }

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_STRING_DECL(Path)
    NW_G3D_RES_FIELD_CLASS_DECL(ResModel, BindModel)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResAnimCurve, Curve)

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESVISIBILITYANIM_H_
