#ifndef NW_G3D_RES_RESUTILITY_H_
#define NW_G3D_RES_RESUTILITY_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>

namespace nw { namespace g3d { namespace math {

class Vec3;
class Vec4;
class Quat;
class Mtx34;

} } }

#include <nw/g3d/fnd/g3d_GX2Struct.h>

namespace nw { namespace g3d { namespace res {

struct ResFileData;
struct ResExternalFileData;
struct ResModelData;
struct ResTextureData;
struct ResSkeletalAnimData;
struct ResBoneAnimData;
struct ResTexPatternAnimData;
struct ResTexPatternMatAnimData;
struct ResShaderParamAnimData;
struct ResShaderParamMatAnimData;
struct ResVisibilityAnimData;
struct ResAnimConstantData;
struct ResAnimBindInfoData;
struct ResAnimCurveData;
struct ResRenderInfoData;
struct ResSkeletonData;
struct ResBoneData;
struct ResVertexData;
struct ResVtxAttribData;
struct ResBufferData;
struct ResShapeData;
struct ResMeshData;
struct ResSubMeshData;
struct ResKeyShapeData;
struct ResMaterialData;
struct ResShaderAssignData;
struct ResRenderStateData;
struct ResTextureRefData;
struct ResSamplerData;
struct ResShaderParamData;
struct ResUserDataData;
struct ResDicPatriciaData;
struct ResShapeAnimData;
struct ResVertexShapeAnimData;
struct ResSceneAnimData;
struct ResCameraAnimData;
struct ResLightAnimData;
struct ResFogAnimData;
struct BoundingNode;
struct Bounding;

template<bool isReversed>
class Endian
{
public:
    static void Swap(ResFileData* data);

private:
    static void Swap(ResExternalFileData* data);
    static void Swap(ResModelData* data);
    static void Swap(ResTextureData* data);
    static void Swap(ResSkeletalAnimData* data);
    static void Swap(ResBoneAnimData* data);
    static void Swap(ResTexPatternAnimData* data);
    static void Swap(ResTexPatternMatAnimData* data);
    static void Swap(ResShaderParamAnimData* data);
    static void Swap(ResShaderParamMatAnimData* data);
    static void Swap(ResVisibilityAnimData* data);
    static void Swap(ResAnimConstantData* data);
    static void Swap(ResAnimCurveData* data);
    static void Swap(ResSkeletonData* data);
    static void Swap(ResBoneData* data);
    static void Swap(ResVertexData* data);
    static void Swap(ResShapeData* data);
    static void Swap(ResMeshData* data);
    static void Swap(ResSubMeshData* data);
    static void Swap(ResKeyShapeData* data);
    static void Swap(ResBufferData* data);
    static void Swap(ResShaderAssignData* data);
    static void Swap(ResMaterialData* data);
    static void Swap(ResRenderInfoData* data);
    static void Swap(ResRenderStateData* data);
    static void Swap(ResTextureRefData* data);
    static void Swap(ResSamplerData* data);
    static void Swap(ResShaderParamData* data);
    static void Swap(ResUserDataData* data);
    static void Swap(ResVtxAttribData* vtxAttribData, ResBufferData* vtxBufferData);
    static void Swap(ResVtxAttribData*) { }
    static void Swap(Bounding* data);
    static void Swap(BoundingNode* data);

    template <class T>
    static void Swap(ResDicPatriciaData* data, bool isContextSwap);

    static void Swap(ResShapeAnimData* data);
    static void Swap(ResVertexShapeAnimData* data);
    static void Swap(ResSceneAnimData* data);
    static void Swap(ResCameraAnimData* data);
    static void Swap(ResLightAnimData* data);
    static void Swap(ResFogAnimData* data);

    static void Swap(nw::g3d::fnd::internal::GX2LoopVarData* data);
    static void Swap(nw::g3d::fnd::internal::GX2VertexShaderData* data);
    static void Swap(nw::g3d::fnd::internal::GX2GeometryShaderData* data);
    static void Swap(nw::g3d::fnd::internal::GX2PixelShaderData* data);

    static void Swap(void* data, int typeSize, int elemCount)
    {
        if (typeSize == 4)
        {
            for (u32* pData = static_cast<u32*>(data), *pEnd = pData + elemCount;
                pData != pEnd; ++pData)
            {
                StoreRevU32(pData, *pData);
            }
        }
        else if (typeSize == 2)
        {
            for (u16* pData = static_cast<u16*>(data), *pEnd = pData + elemCount;
                pData != pEnd; ++pData)
            {
                StoreRevU16(pData, *pData);
            }
        }
        else if (typeSize != 1)
        {
            NW_G3D_NOT_SUPPORTED();
        }
    }

    static void Swap(bool*) {}
    static void Swap(s8*) {}
    static void Swap(bit8*) {}

    static void Swap(s16* data)
    {
        Swap(reinterpret_cast<bit16*>(data));
    }

    static void Swap(bit16* data)
    {
        StoreRevU16(data, *data);
    }

    static void Swap(s32* data)
    {
        Swap(reinterpret_cast<bit32*>(data));
    }

    static void Swap(f32* data)
    {
        Swap(reinterpret_cast<bit32*>(data));
    }

    static void Swap(bit32* data)
    {
        StoreRevU32(data, *data);
    }

    static void Swap(Vec3* data);
    static void Swap(Vec4* data);
    static void Swap(Quat* data);
    static void Swap(Mtx34* data);

    static void Swap(BinString* data)
    {
        Swap(&data->offset);
    }

    static void Swap(Offset* data)
    {
        s32 offset = *data;
        Swap(&offset);
        *data = offset;
    }

    static void Swap(BinPtr* data)
    {
        Swap(&data->addr);
    }

    template<class T>
    static void SwapArray(Offset* data, int size)
    {
        for (int i = 0; i < size; ++i)
        {
            Swap(&data->to_ptr<T>()[i]);
        }
    }

    template<class T>
    static void SwapDic(Offset* data, bool isContextSwap = true)
    {
        if (data->to_ptr() != NULL)
        {
            Swap<T>(ResDicType::ResCast(data->to_ptr<ResDicType::DataType>())->ptr(), isContextSwap);
        }
    }
};

}}} // namespace nw::g3d::res

#endif // NW_G3D_RES_RESUTILITY_H_
