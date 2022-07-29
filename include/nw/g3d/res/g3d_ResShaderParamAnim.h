#ifndef NW_G3D_RES_RESSHADERPARAMANIM_H_
#define NW_G3D_RES_RESSHADERPARAMANIM_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_ResAnimCurve.h>
#include <nw/g3d/res/g3d_ResModel.h>
#include <nw/g3d/res/g3d_Binding.h>

namespace nw { namespace g3d { namespace res {

struct ResShaderParamMatAnimData
{
    u16 numAnimParam;
    u16 numCurve;
    u16 numConstant;
    u16 reserved;
    s32 beginCurve;
    s32 beginParamAnim;

    BinString ofsName;

    Offset ofsParamAnimInfoArray;
    Offset ofsCurveArray;
    Offset ofsConstantArray;
};

class ResShaderParamMatAnim : private ResShaderParamMatAnimData
{
    NW_G3D_RES_COMMON(ResShaderParamMatAnim);

public:
    struct ParamAnimInfo
    {
        u16 beginCurve;
        u16 numFloatCurve;
        u16 numIntCurve;
        u16 beginConstant;
        u16 numConstant;
        u16 subbindIndex;

        BinString ofsName;
    };

    void PreBind(const ResMaterial* target);

    void Eval(void* pResult, float frame, const u16* pSubBindIndex) const
    {
        EvalImpl<false>(pResult, frame, pSubBindIndex, NULL);
    }

    void Eval(void* pResult, float frame, const u16* pSubBindIndex,
        AnimFrameCache* pFrameCache) const
    {
        EvalImpl<true>(pResult, frame, pSubBindIndex, pFrameCache);
    }

    int GetParamAnimCount() const { return ref().numAnimParam; }
    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResAnimCurve, Curve)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResAnimConstant, Constant)

protected:
    template <bool useContext>
    void EvalImpl(void* pResult, float frame, const u16* pSubBindIndex,
        AnimFrameCache* pFrameCache) const;
};

struct ResShaderParamAnimData
{
    BinaryBlockHeader blockHeader;
    BinString ofsName;
    BinString ofsPath;

    bit32 flag;
    s16 numFrame;
    u16 numMatAnim;
    s32 numParamAnim;
    s32 numCurve;
    u32 bakedSize;
    u16 numUserData;
    u16 reserved;

    Offset ofsBindModel;
    Offset ofsBindIndexArray;
    Offset ofsMatAnimArray;
    Offset ofsUserDataDic;
};

class ResShaderParamAnim : private ResShaderParamAnimData
{
    NW_G3D_RES_COMMON(ResShaderParamAnim);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'S', 'H', 'U') };

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

    int GetParamAnimCount() const { return ref().numParamAnim; }

    int GetCurveCount() const { return ref().numCurve; }

    size_t GetBakedSize() const { return ref().bakedSize; }

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_STRING_DECL(Path)
    NW_G3D_RES_FIELD_CLASS_DECL(ResModel, BindModel)
    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResShaderParamMatAnim, MatAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESSHADERPARAMANIM_H_
