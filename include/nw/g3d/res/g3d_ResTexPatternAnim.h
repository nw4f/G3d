#ifndef NW_G3D_RES_RESTEXPATTERNANIM_H_
#define NW_G3D_RES_RESTEXPATTERNANIM_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_ResAnimCurve.h>
#include <nw/g3d/res/g3d_ResModel.h>
#include <nw/g3d/res/g3d_Binding.h>

namespace nw { namespace g3d { namespace res {

class BindCallback;
class ResFile;

struct ResTexPatternMatAnimData
{
    u16 numPatAnim;
    u16 numCurve;
    s32 beginCurve;
    s32 beginPatAnim;

    BinString ofsName;

    Offset ofsPatAnimInfoArray;
    Offset ofsCurveArray;
    Offset ofsBaseValueArray;
};

class ResTexPatternMatAnim : private ResTexPatternMatAnimData
{
    NW_G3D_RES_COMMON(ResTexPatternMatAnim);

public:
    struct PatAnimInfo
    {
        s8 curveIndex;
        s8 subbindIndex;
        u16 reserved;

        BinString ofsName;
    };

    void PreBind(const ResMaterial* target);

    void Init(u16* pResult) const;

    void Eval(u16* pResult, float frame, const s8* pSubBindIndex) const;

    void Eval(u16* pResult, float frame, const s8* pSubBindIndex,
        AnimFrameCache* pFrameCache) const;

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResAnimCurve, Curve)

    int GetPatAnimCount() const { return ref().numPatAnim; }
};

struct ResTexPatternAnimData
{
    BinaryBlockHeader blockHeader;
    BinString ofsName;
    BinString ofsPath;

    bit16 flag;
    s16 numFrame;
    u16 numTextureRef;
    u16 numMatAnim;
    s32 numPatAnim;
    s32 numCurve;
    u32 bakedSize;
    u16 numUserData;
    u16 reserved;

    Offset ofsBindModel;
    Offset ofsBindIndexArray;
    Offset ofsMatAnimArray;
    Offset ofsTextureRefArray;
    Offset ofsUserDataDic;
};

class ResTexPatternAnim : private ResTexPatternAnimData
{
    NW_G3D_RES_COMMON(ResTexPatternAnim);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'T', 'X', 'P') };

    enum Flag
    {
        CURVE_BAKED         = AnimFlag::CURVE_BAKED,
        PLAYPOLICY_LOOP     = AnimFlag::PLAYPOLICY_LOOP
    };

    BindResult Bind(const ResFile* pFile);

    BindResult Bind(const ResFile* pFile, BindCallback* pCallback);

    bool ForceBind(const ResTexture* pTexture, const char* name);

    void Release();

    BindResult PreBind(const ResModel* pModel);

    bool BakeCurve(void* pBuffer, size_t bufferSize);

    void* ResetCurve();

    void Reset();

    bool IsLooped() const { return 0 != (ref().flag & PLAYPOLICY_LOOP); }

    bool IsCurveBaked() const { return 0 != (ref().flag & CURVE_BAKED); }

    int GetFrameCount() const { return ref().numFrame; }

    int GetPatAnimCount() const { return ref().numPatAnim; }

    int GetCurveCount() const { return ref().numCurve; }

    size_t GetBakedSize() const { return ref().bakedSize; }

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_STRING_DECL(Path)
    NW_G3D_RES_FIELD_CLASS_DECL(ResModel, BindModel)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResTexPatternMatAnim, MatAnim)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResTextureRef, TextureRef)

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESTEXPATTERNANIM_H_
