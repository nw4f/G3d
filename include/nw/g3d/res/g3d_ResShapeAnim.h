#ifndef NW_G3D_RES_RESSHAPEANIM_H_
#define NW_G3D_RES_RESSHAPEANIM_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_ResAnimCurve.h>
#include <nw/g3d/res/g3d_ResModel.h>
#include <nw/g3d/res/g3d_Binding.h>

namespace nw { namespace g3d { namespace res {

class BindCallback;
class ResFile;

struct ResVertexShapeAnimData
{
    u8 numCurve;
    u8 numKeyShapeAnim;
    u16 reserved;
    s32 beginCurve;
    s32 beginKeyShapeAnim;

    BinString ofsName;

    Offset ofsKeyShapeAnimInfoArray;
    Offset ofsCurveArray;
    Offset ofsBaseValueArray;
};

class ResVertexShapeAnim : private ResVertexShapeAnimData
{
    NW_G3D_RES_COMMON(ResVertexShapeAnim);

public:
    struct KeyShapeAnimInfo
    {
        s8 curveIndex;
        s8 subbindIndex;
        u16 reserved;
        BinString ofsName;
    };

    void PreBind(const ResShape* target);

    void Init(float* pResult) const;

    void Eval(float* pResult, float frame, const s8* pSubBindIndex) const;

    void Eval(float* pResult, float frame, const s8* pSubBindIndex,
        AnimFrameCache* pFrameCache) const;

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResAnimCurve, Curve)

    int GetKeyShapeAnimCount() const { return ref().numKeyShapeAnim; }
};

struct ResShapeAnimData
{
    BinaryBlockHeader blockHeader;
    BinString ofsName;
    BinString ofsPath;

    bit16 flag;
    s16 numFrame;
    u16 numVertexShapeAnim;
    u8 numKeyShapeAnim;
    u8 numCurve;
    u32 bakedSize;
    u16 numUserData;
    u16 reserved;

    Offset ofsBindModel;
    Offset ofsBindIndexArray;
    Offset ofsVertexShapeAnimArray;
    Offset ofsUserDataDic;
};

class ResShapeAnim : private ResShapeAnimData
{
    NW_G3D_RES_COMMON(ResShapeAnim);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'S', 'H', 'A') };

    enum Flag
    {
        CURVE_BAKED         = AnimFlag::CURVE_BAKED,
        PLAYPOLICY_LOOP     = AnimFlag::PLAYPOLICY_LOOP
    };

    BindResult PreBind(const ResModel* pModel);

    bool BakeCurve(void* pBuffer, size_t bufferSize);

    void* ResetCurve();

    void Reset();

    bool IsLooped() const { return 0 != (ref().flag & PLAYPOLICY_LOOP); }

    bool IsCurveBaked() const { return 0 != (ref().flag & CURVE_BAKED); }

    int GetFrameCount() const { return ref().numFrame; }

    int GetKeyShapeAnimCount() const { return ref().numKeyShapeAnim; }

    int GetCurveCount() const { return ref().numCurve; }

    size_t GetBakedSize() const { return ref().bakedSize; }

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_STRING_DECL(Path)
    NW_G3D_RES_FIELD_CLASS_DECL(ResModel, BindModel)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResVertexShapeAnim, VertexShapeAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESSHAPEANIM_H_
