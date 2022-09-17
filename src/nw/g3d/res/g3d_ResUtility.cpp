#include <nw/g3d/g3d_config.h>

#if NW_G3D_IS_HOST_WIN

#include <nw/g3d/res/g3d_ResUtility.h>

#include <nw/g3d/res/g3d_ResFile.h>
#include <nw/g3d/res/g3d_ResModel.h>
#include <nw/g3d/res/g3d_ResTexture.h>
#include <nw/g3d/res/g3d_ResSkeletalAnim.h>
#include <nw/g3d/res/g3d_ResTexPatternAnim.h>
#include <nw/g3d/res/g3d_ResShaderParamAnim.h>
#include <nw/g3d/res/g3d_ResVisibilityAnim.h>
#include <nw/g3d/res/g3d_ResAnimCurve.h>
#include <nw/g3d/res/g3d_ResSkeleton.h>
#include <nw/g3d/res/g3d_ResShape.h>
#include <nw/g3d/res/g3d_ResMaterial.h>
#include <nw/g3d/res/g3d_ResUserData.h>
#include <nw/g3d/res/g3d_ResDictionary.h>

#include <nw/g3d/res/g3d_ResSceneAnim.h>

#include <nw/g3d/ut/g3d_Flag.h>

namespace nw { namespace g3d { namespace res {

template<bool isReversed>
void Endian<isReversed>::Swap(ResFileData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapDic<ResModelData>(&data->ofsModelDic);
        SwapDic<ResTextureData>(&data->ofsTextureDic);
        SwapDic<ResSkeletalAnimData>(&data->ofsSkeletalAnimDic);
        SwapDic<ResShaderParamAnimData>(&data->ofsShaderParamAnimDic);
        SwapDic<ResShaderParamAnimData>(&data->ofsColorAnimDic);
        SwapDic<ResShaderParamAnimData>(&data->ofsTexSrtAnimDic);
        SwapDic<ResTexPatternAnimData>(&data->ofsTexPatternAnimDic);
        SwapDic<ResVisibilityAnimData>(&data->ofsBoneVisAnimDic);
        SwapDic<ResVisibilityAnimData>(&data->ofsMatVisAnimDic);
        SwapDic<ResShapeAnimData>(&data->ofsShapeAnimDic);
        SwapDic<ResSceneAnimData>(&data->ofsSceneAnimDic);
        SwapDic<ResExternalFileData>(&data->ofsExternalFileDic);

        ResNameData* pData = data->ofsStringPool.to_ptr<ResNameData>();
        u32 poolCount = 0;
        while (poolCount < data->sizeStringPool)
        {
            ResName::LengthType length = static_cast<ResName::LengthType>(
                ut::Align(pData->len + sizeof(ResName::LengthType), 4));
            Swap(&pData->len);
            poolCount += length;
            pData = AddOffset<ResNameData>(pData, length);
        }
    }

    Swap(&data->fileHeader.byteOrder);
    Swap(&data->fileHeader.headerSize);
    Swap(&data->fileHeader.fileSize);
    Swap(&data->alignment);
    Swap(&data->ofsName);
    Swap(&data->ofsStringPool);
    Swap(&data->sizeStringPool);

    Swap(&data->ofsModelDic);
    Swap(&data->ofsTextureDic);
    Swap(&data->ofsSkeletalAnimDic);
    Swap(&data->ofsShaderParamAnimDic);
    Swap(&data->ofsColorAnimDic);
    Swap(&data->ofsTexSrtAnimDic);
    Swap(&data->ofsTexPatternAnimDic);
    Swap(&data->ofsBoneVisAnimDic);
    Swap(&data->ofsMatVisAnimDic);
    Swap(&data->ofsShapeAnimDic);
    Swap(&data->ofsSceneAnimDic);
    Swap(&data->ofsExternalFileDic);

    Swap(&data->numModel);
    Swap(&data->numTexture);
    Swap(&data->numSkeletalAnim);
    Swap(&data->numShaderParamAnim);
    Swap(&data->numColorAnim);
    Swap(&data->numTexSrtAnim);
    Swap(&data->numTexPatternAnim);
    Swap(&data->numBoneVisAnim);
    Swap(&data->numMatVisAnim);
    Swap(&data->numShapeAnim);
    Swap(&data->numSceneAnim);
    Swap(&data->numExternalFile);

    Swap(&data->pUserPtr);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapDic<ResModelData>(&data->ofsModelDic);
        SwapDic<ResTextureData>(&data->ofsTextureDic);
        SwapDic<ResSkeletalAnimData>(&data->ofsSkeletalAnimDic);
        SwapDic<ResShaderParamAnimData>(&data->ofsShaderParamAnimDic);
        SwapDic<ResShaderParamAnimData>(&data->ofsColorAnimDic);
        SwapDic<ResShaderParamAnimData>(&data->ofsTexSrtAnimDic);
        SwapDic<ResTexPatternAnimData>(&data->ofsTexPatternAnimDic);
        SwapDic<ResVisibilityAnimData>(&data->ofsBoneVisAnimDic);
        SwapDic<ResVisibilityAnimData>(&data->ofsMatVisAnimDic);
        SwapDic<ResShapeAnimData>(&data->ofsShapeAnimDic);
        SwapDic<ResSceneAnimData>(&data->ofsSceneAnimDic);
        SwapDic<ResExternalFileData>(&data->ofsExternalFileDic);

        ResNameData* pData = data->ofsStringPool.to_ptr<ResNameData>();
        u32 poolCount = 0;
        while (poolCount < data->sizeStringPool)
        {
            Swap(&pData->len);

            ResName::LengthType length = static_cast<ResName::LengthType>(
                ut::Align(pData->len + sizeof(ResName::LengthType), 4));

            poolCount += length;
            pData = AddOffset<ResNameData>(pData, length);
        }
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResExternalFileData* data)
{
    Swap(&data->offset);
    Swap(&data->size);
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResModelData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        Swap(data->ofsSkeleton.to_ptr<ResSkeletonData>());
        SwapArray<ResVertexData>(&data->ofsVertexArray, data->numVertex);
        SwapDic<ResShapeData>(&data->ofsShapeDic);
        SwapDic<ResMaterialData>(&data->ofsMaterialDic);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }

    Swap(&data->ofsName);
    Swap(&data->ofsPath);

    Swap(&data->ofsSkeleton);
    Swap(&data->ofsVertexArray);
    Swap(&data->ofsShapeDic);
    Swap(&data->ofsMaterialDic);
    Swap(&data->ofsUserDataDic);

    Swap(&data->numVertex);
    Swap(&data->numShape);
    Swap(&data->numMaterial);
    Swap(&data->numUserData);

    Swap(&data->totalProcessVertex);

    Swap(&data->pUserPtr);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        Swap(data->ofsSkeleton.to_ptr<ResSkeletonData>());
        SwapArray<ResVertexData>(&data->ofsVertexArray, data->numVertex);
        SwapDic<ResShapeData>(&data->ofsShapeDic);
        SwapDic<ResMaterialData>(&data->ofsMaterialDic);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResTextureData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }

    nw::g3d::fnd::internal::GX2SurfaceData& surface = data->gfxTexture.gx2Texture.surface;
    Swap(reinterpret_cast<bit32*>(&surface.dim));
    Swap(&surface.width);
    Swap(&surface.height);
    Swap(&surface.depth);
    Swap(&surface.numMips);
    Swap(reinterpret_cast<bit32*>(&surface.format));
    Swap(reinterpret_cast<bit32*>(&surface.aa));
    Swap(reinterpret_cast<bit32*>(&surface.use));
    Swap(&surface.imageSize);
    Swap(&surface.mipSize);
    Swap(reinterpret_cast<bit32*>(&surface.tileMode));
    Swap(&surface.swizzle);
    Swap(&surface.alignment);
    Swap(&surface.pitch);
    for (int i = 0; i < 13; ++i)
    {
        Swap(&surface.mipOffset[i]);
    }

    Swap(&data->gfxTexture.gx2Texture.viewFirstMip);
    Swap(&data->gfxTexture.gx2Texture.viewNumMips);
    Swap(&data->gfxTexture.gx2Texture.viewFirstSlice);
    Swap(&data->gfxTexture.gx2Texture.viewNumSlices);
    Swap(&data->gfxTexture.gx2Texture.compSel);
    for (int i = 0; i < GX2_NUM_TEXTURE_REGISTERS; ++i)
    {
        Swap(&data->gfxTexture.gx2Texture._regs[i]);
    }

    Swap(&data->ofsName);
    Swap(&data->ofsPath);
    Swap(&data->ofsData);
    Swap(&data->ofsMipData);
    Swap(&data->ofsUserDataDic);

    Swap(&data->numUserData);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResSkeletalAnimData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapArray<ResBoneAnimData>(&data->ofsBoneAnimArray, data->numBoneAnim);
        SwapArray<u16>(&data->ofsBindIndexArray, data->numBoneAnim);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }

    Swap(&data->flag);
    Swap(&data->numFrame);
    Swap(&data->numBoneAnim);
    Swap(&data->numCurve);
    Swap(&data->bakedSize);
    Swap(&data->numUserData);

    Swap(&data->ofsName);
    Swap(&data->ofsPath);
    Swap(&data->ofsBoneAnimArray);
    Swap(&data->ofsBindSkeleton);
    Swap(&data->ofsBindIndexArray);
    Swap(&data->ofsUserDataDic);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<ResBoneAnimData>(&data->ofsBoneAnimArray, data->numBoneAnim);
        SwapArray<u16>(&data->ofsBindIndexArray, data->numBoneAnim);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResBoneAnimData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        int numBaseValue = 0;
        if (data->flag & ResBoneAnim::BASE_SCALE)
        {
            numBaseValue += 3;
        }

        if (data->flag & ResBoneAnim::BASE_ROTATE)
        {
            numBaseValue += 4;
        }

        if (data->flag & ResBoneAnim::BASE_TRANSLATE)
        {
            numBaseValue += 3;
        }
        SwapArray<float>(&data->ofsBaseValueArray, numBaseValue);
    }

    Swap(&data->flag);
    Swap(&data->ofsName);
    Swap(&data->beginRotate);
    Swap(&data->beginTranslate);
    Swap(&data->numCurve);
    Swap(&data->ofsCurveArray);
    Swap(&data->ofsBaseValueArray);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        int numBaseValue = 0;
        if (data->flag & ResBoneAnim::BASE_SCALE)
        {
            numBaseValue += 3;
        }

        if (data->flag & ResBoneAnim::BASE_ROTATE)
        {
            numBaseValue += 4;
        }

        if (data->flag & ResBoneAnim::BASE_TRANSLATE)
        {
            numBaseValue += 3;
        }
        SwapArray<float>(&data->ofsBaseValueArray, numBaseValue);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResTexPatternAnimData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapArray<ResTexPatternMatAnimData>(&data->ofsMatAnimArray, data->numMatAnim);
        SwapArray<u16>(&data->ofsBindIndexArray, data->numMatAnim);
        SwapArray<ResTextureRefData>(&data->ofsTextureRefArray, data->numTextureRef);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }

    Swap(&data->flag);
    Swap(&data->numFrame);
    Swap(&data->numTextureRef);
    Swap(&data->numMatAnim);
    Swap(&data->numPatAnim);
    Swap(&data->numCurve);
    Swap(&data->bakedSize);
    Swap(&data->numUserData);

    Swap(&data->ofsName);
    Swap(&data->ofsPath);

    Swap(&data->ofsBindModel);
    Swap(&data->ofsBindIndexArray);
    Swap(&data->ofsMatAnimArray);
    Swap(&data->ofsTextureRefArray);
    Swap(&data->ofsUserDataDic);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<ResTexPatternMatAnimData>(&data->ofsMatAnimArray, data->numMatAnim);
        SwapArray<u16>(&data->ofsBindIndexArray, data->numMatAnim);
        SwapArray<ResTextureRefData>(&data->ofsTextureRefArray, data->numTextureRef);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResTexPatternMatAnimData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        for (int i = 0; i < data->numPatAnim; ++i)
        {
            ResTexPatternMatAnim::PatAnimInfo& info = data->ofsPatAnimInfoArray.to_ptr<ResTexPatternMatAnim::PatAnimInfo>()[i];
            Swap(&info.curveIndex);
            Swap(&info.subbindIndex);
            Swap(&info.ofsName);
        }

        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        SwapArray<bit16>(&data->ofsBaseValueArray, data->numPatAnim);
    }

    Swap(&data->numPatAnim);
    Swap(&data->numCurve);
    Swap(&data->beginCurve);
    Swap(&data->beginPatAnim);

    Swap(&data->ofsName);

    Swap(&data->ofsPatAnimInfoArray);
    Swap(&data->ofsCurveArray);
    Swap(&data->ofsBaseValueArray);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        for (int i = 0; i < data->numPatAnim; ++i)
        {
            ResTexPatternMatAnim::PatAnimInfo& info = data->ofsPatAnimInfoArray.to_ptr<ResTexPatternMatAnim::PatAnimInfo>()[i];
            Swap(&info.curveIndex);
            Swap(&info.subbindIndex);
            Swap(&info.ofsName);
        }
        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        SwapArray<bit16>(&data->ofsBaseValueArray, data->numPatAnim);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResShaderParamAnimData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapArray<u16>(&data->ofsBindIndexArray, data->numMatAnim);
        SwapArray<ResShaderParamMatAnimData>(&data->ofsMatAnimArray, data->numMatAnim);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }

    Swap(&data->flag);
    Swap(&data->numFrame);
    Swap(&data->numMatAnim);
    Swap(&data->numParamAnim);
    Swap(&data->numCurve);
    Swap(&data->bakedSize);
    Swap(&data->numUserData);

    Swap(&data->ofsName);
    Swap(&data->ofsPath);

    Swap(&data->ofsBindModel);
    Swap(&data->ofsBindIndexArray);
    Swap(&data->ofsMatAnimArray);
    Swap(&data->ofsUserDataDic);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<u16>(&data->ofsBindIndexArray, data->numMatAnim);
        SwapArray<ResShaderParamMatAnimData>(&data->ofsMatAnimArray, data->numMatAnim);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResShaderParamMatAnimData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        for (int i = 0; i < data->numAnimParam; ++i)
        {
            ResShaderParamMatAnim::ParamAnimInfo& info = data->ofsParamAnimInfoArray.to_ptr<ResShaderParamMatAnim::ParamAnimInfo>()[i];
            Swap(&info.beginCurve);
            Swap(&info.numFloatCurve);
            Swap(&info.numIntCurve);
            Swap(&info.beginConstant);
            Swap(&info.numConstant);
            Swap(&info.subbindIndex);
            Swap(&info.ofsName);
        }
        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        SwapArray<ResAnimConstantData>(&data->ofsConstantArray, data->numConstant);
    }

    Swap(&data->numAnimParam);
    Swap(&data->numCurve);
    Swap(&data->numConstant);
    Swap(&data->beginCurve);
    Swap(&data->beginParamAnim);

    Swap(&data->ofsName);

    Swap(&data->ofsParamAnimInfoArray);
    Swap(&data->ofsCurveArray);
    Swap(&data->ofsConstantArray);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        for (int i = 0; i < data->numAnimParam; ++i)
        {
            ResShaderParamMatAnim::ParamAnimInfo& info = data->ofsParamAnimInfoArray.to_ptr<ResShaderParamMatAnim::ParamAnimInfo>()[i];
            Swap(&info.beginCurve);
            Swap(&info.numFloatCurve);
            Swap(&info.numIntCurve);
            Swap(&info.beginConstant);
            Swap(&info.numConstant);
            Swap(&info.subbindIndex);
            Swap(&info.ofsName);
        }
        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        SwapArray<ResAnimConstantData>(&data->ofsConstantArray, data->numConstant);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResVisibilityAnimData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapArray<u16>(&data->ofsBindIndexArray, data->numAnim);
        SwapArray<BinString>(&data->ofsNameArray, data->numAnim);
        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        SwapArray<bit32>(&data->ofsBaseValueArray, static_cast<size_t>(std::ceil(data->numAnim / 32.0f)));
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }

    Swap(&data->flag);
    Swap(&data->numFrame);
    Swap(&data->numAnim);
    Swap(&data->numCurve);
    Swap(&data->bakedSize);
    Swap(&data->numUserData);

    Swap(&data->ofsName);
    Swap(&data->ofsPath);

    Swap(&data->ofsBindModel);
    Swap(&data->ofsBindIndexArray);
    Swap(&data->ofsNameArray);
    Swap(&data->ofsCurveArray);
    Swap(&data->ofsBaseValueArray);
    Swap(&data->ofsUserDataDic);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<u16>(&data->ofsBindIndexArray, data->numAnim);
        SwapArray<BinString>(&data->ofsNameArray, data->numAnim);
        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        SwapArray<bit32>(&data->ofsBaseValueArray, static_cast<size_t>(std::ceil(data->numAnim / 32.0f)));
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResAnimConstantData* data)
{
    Swap(&data->fValue);
    Swap(&data->targetOffset);
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResAnimCurveData* data)
{
    NW_G3D_TABLE_FIELD int typeSize[] = { 4, 2, 1, 1, 1, 1, 1 };
    NW_G3D_TABLE_FIELD int keySize[] = { 4, 2, 1 };

    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        if ((data->flag & ResAnimCurve::FRAME_MASK) == ResAnimCurve::FRAME32)
        {
            SwapArray<f32>(&data->ofsFrameArray, data->numKey);
        }
        else if ((data->flag & ResAnimCurve::FRAME_MASK) == ResAnimCurve::FRAME16)
        {
            SwapArray<s16>(&data->ofsFrameArray, data->numKey);
        }
        else if ((data->flag & ResAnimCurve::FRAME_MASK) == ResAnimCurve::FRAME8)
        {
            SwapArray<u8>(&data->ofsFrameArray, data->numKey);
        }
        else
        {
            NW_G3D_NOT_IMPLEMENTED();
        }

        int curveType = (data->flag & ResAnimCurve::CURVE_MASK) >> ResAnimCurve::CURVE_SHIFT;
        int keyType = (data->flag & ResAnimCurve::KEY_MASK) >> ResAnimCurve::KEY_SHIFT;

        Swap(data->ofsKeyArray.to_ptr(), keySize[keyType], typeSize[curveType] * data->numKey);
    }

    Swap(&data->flag);
    Swap(&data->numKey);
    Swap(&data->targetOffset);
    Swap(&data->startFrame);
    Swap(&data->endFrame);
    Swap(&data->fScale);
    Swap(&data->fOffset);
    Swap(&data->ofsFrameArray);
    Swap(&data->ofsKeyArray);


    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        if ((data->flag & ResAnimCurve::FRAME_MASK) == ResAnimCurve::FRAME32)
        {
            SwapArray<f32>(&data->ofsFrameArray, data->numKey);
        }
        else if ((data->flag & ResAnimCurve::FRAME_MASK) == ResAnimCurve::FRAME16)
        {
            SwapArray<s16>(&data->ofsFrameArray, data->numKey);
        }
        else if ((data->flag & ResAnimCurve::FRAME_MASK) == ResAnimCurve::FRAME8)
        {
            SwapArray<u8>(&data->ofsFrameArray, data->numKey);
        }
        else
        {
            NW_G3D_NOT_IMPLEMENTED();
        }

        int curveType = (data->flag & ResAnimCurve::CURVE_MASK) >> ResAnimCurve::CURVE_SHIFT;
        int keyType = (data->flag & ResAnimCurve::KEY_MASK) >> ResAnimCurve::KEY_SHIFT;

        Swap(data->ofsKeyArray.to_ptr(), keySize[keyType], typeSize[curveType] * data->numKey);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResSkeletonData* data)
{

    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapDic<ResBoneData>(&data->ofsBoneDic);
        SwapArray<u16>(&data->ofsMtxToBoneTable, data->numSmoothMtx + data->numRigidMtx);

    }

    Swap(&data->flag);
    Swap(&data->numBone);
    Swap(&data->numSmoothMtx);
    Swap(&data->numRigidMtx);
    Swap(&data->ofsBoneDic);
    Swap(&data->ofsBoneArray);
    Swap(&data->ofsMtxToBoneTable);
    Swap(&data->pUserPtr);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapDic<ResBoneData>(&data->ofsBoneDic);
        SwapArray<u16>(&data->ofsMtxToBoneTable, data->numSmoothMtx + data->numRigidMtx);

    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResBoneData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }

    Swap(&data->ofsName);
    Swap(&data->index);
    Swap(&data->parentIndex);
    Swap(&data->smoothMtxIndex);
    Swap(&data->rigidMtxIndex);
    Swap(&data->billboardIndex);
    Swap(&data->numUserData);
    Swap(&data->scale);
    Swap(&data->flag);
    Swap(&data->rotate.quat);
    Swap(&data->translate);
    Swap(&data->ofsUserDataDic);
    Swap(&data->invModelMtx);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResVertexData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        if (data->ofsVtxAttribDic.to_ptr() != NULL)
        {
            ResDicPatriciaData* dic = ResDicType::ResCast(data->ofsVtxAttribDic.to_ptr<ResDicType::DataType>())->ptr();
            for (int idxAttrib = 0; idxAttrib < dic->numData + 1; ++idxAttrib)
            {
                ResVtxAttribData* vtxAttribData = dic->node[idxAttrib].ofsData.to_ptr<ResVtxAttribData>();
                if (vtxAttribData != NULL)
                {
                    Swap(vtxAttribData, &data->ofsVtxBufferArray.to_ptr<ResBufferData>()[vtxAttribData->bufferIndex]);
                }
            }
            SwapDic<ResVtxAttribData>(&data->ofsVtxAttribDic);
        }

        SwapArray<ResBufferData>(&data->ofsVtxBufferArray, data->numVtxBuffer);
    }

    Swap(&data->numVtxAttrib);
    Swap(&data->numVtxBuffer);
    Swap(&data->index);
    Swap(&data->count);
    Swap(&data->vtxSkinCount);
    Swap(&data->ofsVtxAttribArray);
    Swap(&data->ofsVtxAttribDic);
    Swap(&data->ofsVtxBufferArray);

    Swap(&data->pUserPtr);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<ResBufferData>(&data->ofsVtxBufferArray, data->numVtxBuffer);

        if (data->ofsVtxAttribDic.to_ptr() != NULL)
        {
            SwapDic<ResVtxAttribData>(&data->ofsVtxAttribDic);
            ResDicPatriciaData* dic = ResDicType::ResCast(data->ofsVtxAttribDic.to_ptr<ResDicType::DataType>())->ptr();
            for (int idxAttrib = 0; idxAttrib < dic->numData + 1; ++idxAttrib)
            {
                ResVtxAttribData* vtxAttribData = dic->node[idxAttrib].ofsData.to_ptr<ResVtxAttribData>();
                if (vtxAttribData != NULL)
                {
                    Swap(vtxAttribData, &data->ofsVtxBufferArray.to_ptr<ResBufferData>()[vtxAttribData->bufferIndex]);
                }
            }
        }
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResShapeData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        ResMeshData* mesh = data->ofsMeshArray.to_ptr<ResMeshData>();
        SwapArray<u16>(&data->ofsSubMeshIndexArray, mesh->numSubMesh);
        SwapArray<BoundingNode>(&data->ofsBoundingNodeArray, data->numBoundingNode);
        SwapArray<Bounding>(&data->ofsSubBoundingArray, data->numBoundingNode);
        SwapDic<ResKeyShapeData>(&data->ofsKeyShapeDic);
        SwapArray<u16>(&data->ofsSkinBoneIndexArray, data->numSkinBoneIndex);
        SwapArray<ResMeshData>(&data->ofsMeshArray, data->numMesh);
    }

    Swap(&data->ofsName);
    Swap(&data->flag);
    Swap(&data->index);
    Swap(&data->materialIndex);
    Swap(&data->boneIndex);
    Swap(&data->vertexIndex);
    Swap(&data->numSkinBoneIndex);
    Swap(&data->vtxSkinCount);
    Swap(&data->numMesh);
    Swap(&data->numKeyShape);
    Swap(&data->numTargetAttrib);
    Swap(&data->numBoundingNode);
    Swap(&data->radius);
    Swap(&data->ofsVertex);
    Swap(&data->ofsMeshArray);
    Swap(&data->ofsSkinBoneIndexArray);
    Swap(&data->ofsKeyShapeDic);
    Swap(&data->ofsBoundingNodeArray);
    Swap(&data->ofsSubBoundingArray);
    Swap(&data->ofsSubMeshIndexArray);
    Swap(&data->pUserPtr);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<ResMeshData>(&data->ofsMeshArray, data->numMesh);
        SwapArray<u16>(&data->ofsSkinBoneIndexArray, data->numSkinBoneIndex);
        SwapDic<ResKeyShapeData>(&data->ofsKeyShapeDic);
        SwapArray<Bounding>(&data->ofsSubBoundingArray, data->numBoundingNode);
        SwapArray<BoundingNode>(&data->ofsBoundingNodeArray, data->numBoundingNode);
        ResMeshData* mesh = data->ofsMeshArray.to_ptr<ResMeshData>();
        SwapArray<u16>(&data->ofsSubMeshIndexArray, mesh->numSubMesh);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResMeshData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        int idxSize = 0;

        if (data->format == GX2_INDEX_FORMAT_U16)
        {
            idxSize = 2;
        }
        else if(data->format == GX2_INDEX_FORMAT_U32)
        {
            idxSize = 4;
        }

        NW_G3D_ASSERTMSG(idxSize != 0, "Invalid index format.");

        ResBufferData* idxBuffer = data->ofsIdxBuffer.to_ptr<ResBufferData>();
        void* idxData = idxBuffer->ofsData.to_ptr();
        Swap(idxData, idxSize, data->count);

        SwapArray<ResSubMeshData>(&data->ofsSubMeshArray, data->numSubMesh);
        Swap(idxBuffer);
    }

    Swap(reinterpret_cast<bit32*>(&data->primType));
    Swap(reinterpret_cast<bit32*>(&data->format));
    Swap(&data->count);
    Swap(&data->numSubMesh);
    Swap(&data->ofsSubMeshArray);
    Swap(&data->ofsIdxBuffer);
    Swap(&data->offset);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<ResSubMeshData>(&data->ofsSubMeshArray, data->numSubMesh);
        Swap(data->ofsIdxBuffer.to_ptr<ResBufferData>());

        int idxSize = 0;

        if (data->format == GX2_INDEX_FORMAT_U16)
        {
            idxSize = 2;
        }
        else if(data->format == GX2_INDEX_FORMAT_U32)
        {
            idxSize = 4;
        }

        NW_G3D_ASSERTMSG(idxSize != 0, "Invalid index format.");

        ResBufferData* idxBuffer = data->ofsIdxBuffer.to_ptr<ResBufferData>();
        void* idxData = idxBuffer->ofsData.to_ptr();
        Swap(idxData, idxSize, data->count);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResSubMeshData* data)
{
    Swap(&data->offset);
    Swap(&data->count);
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResKeyShapeData* data)
{
    (void)data;
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResBufferData* data)
{
    Swap(&data->gfxBuffer.size);
    Swap(&data->gfxBuffer.stride);
    Swap(&data->gfxBuffer.numBuffering);
    Swap(&data->ofsData);
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResShaderAssignData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapDic<bit8>(&data->ofsAttribAssignDic);
        SwapDic<bit8>(&data->ofsSamplerAssignDic);
        SwapDic<bit8>(&data->ofsShaderOptionDic);
    }

    Swap(&data->ofsShaderArchiveName);
    Swap(&data->ofsShadingModelName);
    Swap(&data->revision);
    Swap(&data->numAttribAssign);
    Swap(&data->numSamplerAssign);
    Swap(&data->numShaderOption);
    Swap(&data->ofsAttribAssignDic);
    Swap(&data->ofsSamplerAssignDic);
    Swap(&data->ofsShaderOptionDic);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapDic<bit8>(&data->ofsAttribAssignDic);
        SwapDic<bit8>(&data->ofsSamplerAssignDic);
        SwapDic<bit8>(&data->ofsShaderOptionDic);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResMaterialData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        Swap(data->ofsSrcParam.to_ptr(), sizeof(bit32), data->srcParamSize >> 2);
        SwapDic<ResRenderInfoData>(&data->ofsRenderInfoDic);
        Swap(data->ofsRenderState.to_ptr<ResRenderStateData>());

        if (data->ofsShaderAssign.to_ptr())
        {
            Swap(data->ofsShaderAssign.to_ptr<ResShaderAssignData>());
        }

        SwapArray<ResTextureRefData>(&data->ofsTextureRefArray, data->numSampler);
        SwapDic<ResSamplerData>(&data->ofsSamplerDic);
        SwapDic<ResShaderParamData>(&data->ofsShaderParamDic);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }

    Swap(&data->ofsName);
    Swap(&data->flag);
    Swap(&data->index);
    Swap(&data->numRenderInfo);
    Swap(&data->numSampler);
    Swap(&data->numTexture);
    Swap(&data->numShaderParam);
    Swap(&data->numShaderParamVolatile);
    Swap(&data->srcParamSize);
    Swap(&data->rawParamSize);
    Swap(&data->numUserData);
    Swap(&data->ofsRenderInfoDic);
    Swap(&data->ofsRenderState);
    Swap(&data->ofsShaderAssign);
    Swap(&data->ofsTextureRefArray);
    Swap(&data->ofsSamplerArray);
    Swap(&data->ofsSamplerDic);
    Swap(&data->ofsShaderParamArray);
    Swap(&data->ofsShaderParamDic);
    Swap(&data->ofsSrcParam);
    Swap(&data->ofsUserDataDic);
    Swap(&data->pUserPtr);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapDic<ResRenderInfoData>(&data->ofsRenderInfoDic);
        Swap(data->ofsRenderState.to_ptr<ResRenderStateData>());

        if (data->ofsShaderAssign.to_ptr())
        {
            Swap(data->ofsShaderAssign.to_ptr<ResShaderAssignData>());
        }

        SwapArray<ResTextureRefData>(&data->ofsTextureRefArray, data->numSampler);
        SwapDic<ResSamplerData>(&data->ofsSamplerDic);
        SwapDic<ResShaderParamData>(&data->ofsShaderParamDic);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
        Swap(data->ofsSrcParam.to_ptr(), sizeof(bit32), data->srcParamSize >> 2);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResRenderInfoData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        if (data->type == ResRenderInfo::INT)
        {
            for (int i = 0; i < data->arrayLength; ++i)
            {
                Swap(&data->iValue[i]);
            }
        }
        else if (data->type == ResRenderInfo::FLOAT)
        {
            for (int i = 0; i < data->arrayLength; ++i)
            {
                Swap(&data->fValue[i]);
            }
        }
        else
        {
            for (int i = 0; i < static_cast<int>(data->arrayLength); ++i)
            {
                Swap(&data->ofsString[i]);
            }
        }
    }

    Swap(&data->arrayLength);
    Swap(&data->type);
    Swap(&data->ofsName);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        if (data->type == ResRenderInfo::INT)
        {
            for (int i = 0; i < data->arrayLength; ++i)
            {
                Swap(&data->iValue[i]);
            }
        }
        else if (data->type == ResRenderInfo::FLOAT)
        {
            for (int i = 0; i < data->arrayLength; ++i)
            {
                Swap(&data->fValue[i]);
            }
        }
        else
        {
            for (int i = 0; i < static_cast<int>(data->arrayLength); ++i)
            {
                Swap(&data->ofsString[i]);
            }
        }
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResRenderStateData* data)
{
    Swap(&data->flag);
    Swap(&data->polygonCtrl.gx2PolygonControl.reg);
    Swap(&data->depthCtrl.gx2DepthStencilControl.reg);
    Swap(&data->alphaTest.gx2AlphaTest.reg[0]);
    Swap(&data->alphaTest.gx2AlphaTest.reg[1]);
    Swap(&data->colorCtrl.gx2ColorControl.reg);
    Swap(&data->blendCtrl.gx2BlendControl.reg[0]);
    Swap(&data->blendCtrl.gx2BlendControl.reg[1]);
    Swap(&data->blendColor, sizeof(f32), 4);
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResTextureRefData* data)
{
    Swap(&data->ofsName);
    Swap(&data->ofsRefData);
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResSamplerData* data)
{
    Swap(&data->gfxSampler.gx2Sampler.samplerReg[0]);
    Swap(&data->gfxSampler.gx2Sampler.samplerReg[1]);
    Swap(&data->gfxSampler.gx2Sampler.samplerReg[2]);
    Swap(&data->ofsName);
    Swap(&data->index);
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResShaderParamData* data)
{
    Swap(&data->type);
    Swap(&data->srcSize);
    Swap(&data->offset);
    Swap(&data->srcOffset);
    Swap(&data->ofsId);
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResUserDataData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        if (data->type == ResUserData::INT)
        {
            for (int i = 0; i < data->count; ++i)
            {
                Swap(&data->data.iValue[i]);
            }
        }
        else if (data->type == ResUserData::FLOAT)
        {
            for (int i = 0; i < data->count; ++i)
            {
                Swap(&data->data.fValue[i]);
            }
        }
        else if (data->type == ResUserData::STRING)
        {
            for (int i = 0; i < data->count; ++i)
            {
                Swap(&data->data.ofsString[i]);
            }
        }
        else if (data->type == ResUserData::WSTRING)
        {
            for (int i = 0; i < data->count; ++i)
            {
                char16* str = data->data.ofsString[i].to_ptr<char16>();
                int len = 0;
                char16* temp = str;
                while (NW_G3D_STATIC_CONDITION(1))
                {
                    if (*temp == 0x0)
                    {
                        break;
                    }
                    ++temp;
                    ++len;
                }
                Swap(str, 2, len+1);

                Swap(&data->data.ofsString[i]);
            }
        }
        else if (data->type == ResUserData::STREAM)
        {
            Swap(&data->data.size);
        }
    }

    Swap(&data->ofsName);
    Swap(&data->count);
    Swap(&data->type);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        if (data->type == ResUserData::INT)
        {
            for (int i = 0; i < data->count; ++i)
            {
                Swap(&data->data.iValue[i]);
            }
        }
        else if (data->type == ResUserData::FLOAT)
        {
            for (int i = 0; i < data->count; ++i)
            {
                Swap(&data->data.fValue[i]);
            }
        }
        else if (data->type == ResUserData::STRING)
        {
            for (int i = 0; i < data->count; ++i)
            {
                Swap(&data->data.ofsString[i]);
            }
        }
        else if (data->type == ResUserData::WSTRING)
        {
            for (int i = 0; i < data->count; ++i)
            {
                Swap(&data->data.ofsString[i]);

                char16* str = data->data.ofsString[i].to_ptr<char16>();
                int len = 0;
                char16* temp = str;
                while (NW_G3D_STATIC_CONDITION(1))
                {
                    if (*temp == 0x0)
                    {
                        break;
                    }
                    ++temp;
                    ++len;
                }
                Swap(str, 2, len+1);
            }
        }
        else if (data->type == ResUserData::STREAM)
        {
            Swap(&data->data.size);
        }
    }
}

template<bool isReversed>
template<class T>
void Endian<isReversed>::Swap(ResDicPatriciaData* data, bool isContextSwap)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        for (int i = 0; i < data->numData + 1; ++i)
        {
            T* ofsData = data->node[i].ofsData.to_ptr<T>();
            if (ofsData != NULL && isContextSwap)
            {
                Swap(ofsData);
            }

            Swap(&data->node[i].refBit);
            Swap(&data->node[i].idxLeft);
            Swap(&data->node[i].idxRight);
            Swap(&data->node[i].ofsName);
            Swap(&data->node[i].ofsData);
        }
    }

    Swap(&data->size);
    Swap(&data->numData);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        for (int i = 0; i < data->numData + 1; ++i)
        {
            Swap(&data->node[i].refBit);
            Swap(&data->node[i].idxLeft);
            Swap(&data->node[i].idxRight);
            Swap(&data->node[i].ofsName);
            Swap(&data->node[i].ofsData);

            T* ofsData = data->node[i].ofsData.to_ptr<T>();
            if (ofsData != NULL && isContextSwap)
            {
                Swap(ofsData);
            }
        }
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResVtxAttribData* vtxAttribData, ResBufferData* vtxBufferData)
{
    NW_G3D_TABLE_FIELD int sizeTable[] =
    {
        1, 1, // 0-1
        2, 2, 1, // 2-4
        4, 4, 2, 2, 4, 1, 4, // 5-b
        4, 4, 2, 2, // c-f
        4, 4, // 10-11
        4, 4 // 12-13
    };

    NW_G3D_TABLE_FIELD int countTable[] =
    {
        1, 1, // 0-1
        1, 1, 2, // 2-4
        1, 1, 2, 2, 1, 4, 1, // 5-b
        2, 2, 4, 4, // c-f
        3, 3, // 10-11
        4, 4 // 12-13
    };

    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        int typeSize = sizeTable[vtxAttribData->format & 0xff];
        int elemCount = countTable[vtxAttribData->format & 0xff];

        NW_G3D_ASSERTMSG(typeSize * elemCount != 0, "NW: Not implemented yet.\n");

        int size = vtxBufferData->gfxBuffer.size;
        int stride = vtxBufferData->gfxBuffer.stride;
        void* pData = AddOffset(vtxBufferData->ofsData.to_ptr(), vtxAttribData->offset);
        int totalSize = 0;
        while(totalSize < size)
        {
            Swap(pData, typeSize, elemCount);
            pData = AddOffset(pData, stride);
            totalSize += stride;
        }
    }

    Swap(&vtxAttribData->ofsName);
    Swap(&vtxAttribData->bufferIndex);
    Swap(&vtxAttribData->offset);
    Swap(reinterpret_cast<bit32*>(&vtxAttribData->format));

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        int typeSize = sizeTable[vtxAttribData->format & 0xff];
        int elemCount = countTable[vtxAttribData->format & 0xff];

        NW_G3D_ASSERTMSG(typeSize * elemCount != 0, "NW: Not implemented yet.\n");
        int size = vtxBufferData->gfxBuffer.size;
        int stride = vtxBufferData->gfxBuffer.stride;
        void* pData = AddOffset(vtxBufferData->ofsData.to_ptr(), vtxAttribData->offset);
        int totalSize = 0;
        while(totalSize < size)
        {
            Swap(pData, typeSize, elemCount);

            pData = AddOffset(pData, stride);
            totalSize += stride;
        }
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResShapeAnimData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapArray<u16>(&data->ofsBindIndexArray, data->numVertexShapeAnim);
        SwapArray<ResVertexShapeAnimData>(&data->ofsVertexShapeAnimArray, data->numVertexShapeAnim);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }

    Swap(&data->ofsName);
    Swap(&data->ofsPath);
    Swap(&data->flag);
    Swap(&data->numFrame);
    Swap(&data->numVertexShapeAnim);
    Swap(&data->numKeyShapeAnim);
    Swap(&data->numCurve);
    Swap(&data->bakedSize);
    Swap(&data->numUserData);
    Swap(&data->ofsBindModel);
    Swap(&data->ofsBindIndexArray);
    Swap(&data->ofsVertexShapeAnimArray);
    Swap(&data->ofsUserDataDic);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<u16>(&data->ofsBindIndexArray, data->numVertexShapeAnim);
        SwapArray<ResVertexShapeAnimData>(&data->ofsVertexShapeAnimArray, data->numVertexShapeAnim);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResVertexShapeAnimData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        for (int i = 0; i < data->numKeyShapeAnim; ++i)
        {
            ResVertexShapeAnim::KeyShapeAnimInfo& info = data->ofsKeyShapeAnimInfoArray.to_ptr<ResVertexShapeAnim::KeyShapeAnimInfo>()[i];
            Swap(&info.curveIndex);
            Swap(&info.subbindIndex);
            Swap(&info.ofsName);
        }

        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        SwapArray<f32>(&data->ofsBaseValueArray, data->numKeyShapeAnim - 1);
    }

    Swap(&data->numCurve);
    Swap(&data->numKeyShapeAnim);
    Swap(&data->beginCurve);
    Swap(&data->beginKeyShapeAnim);
    Swap(&data->ofsName);
    Swap(&data->ofsKeyShapeAnimInfoArray);
    Swap(&data->ofsCurveArray);
    Swap(&data->ofsBaseValueArray);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        for (int i = 0; i < data->numKeyShapeAnim; ++i)
        {
            ResVertexShapeAnim::KeyShapeAnimInfo& info = data->ofsKeyShapeAnimInfoArray.to_ptr<ResVertexShapeAnim::KeyShapeAnimInfo>()[i];
            Swap(&info.curveIndex);
            Swap(&info.subbindIndex);
            Swap(&info.ofsName);
        }
        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        SwapArray<f32>(&data->ofsBaseValueArray, data->numKeyShapeAnim - 1);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResSceneAnimData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapDic<ResCameraAnimData>(&data->ofsCameraAnimDic);
      //SwapDic<ResLightAnimData>(&data->ofsLightAnimDic);
      //SwapDic<ResFogAnimData>(&data->ofsFogAnimDic);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }

    Swap(&data->ofsName);
    Swap(&data->ofsPath);
    Swap(&data->numCameraAnim);
    Swap(&data->numLightAnim);
    Swap(&data->numFogAnim);
    Swap(&data->numUserData);
    Swap(&data->ofsCameraAnimDic);
    Swap(&data->ofsLightAnimDic);
    Swap(&data->ofsFogAnimDic);
    Swap(&data->ofsUserDataDic);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapDic<ResCameraAnimData>(&data->ofsCameraAnimDic);
      //SwapDic<ResLightAnimData>(&data->ofsLightAnimDic);
      //SwapDic<ResFogAnimData>(&data->ofsFogAnimDic);
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(ResCameraAnimData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        SwapArray<f32>(&data->ofsBaseValueArray, sizeof(nw::g3d::res::CameraAnimResult) / sizeof(float));
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }

    Swap(&data->flag);
    Swap(&data->numFrame);
    Swap(&data->numCurve);
    Swap(&data->bakedSize);
    Swap(&data->numUserData);
    Swap(&data->ofsName);
    Swap(&data->ofsCurveArray);
    Swap(&data->ofsBaseValueArray);
    Swap(&data->ofsUserDataDic);

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<ResAnimCurveData>(&data->ofsCurveArray, data->numCurve);
        SwapArray<f32>(&data->ofsBaseValueArray, sizeof(nw::g3d::res::CameraAnimResult) / sizeof(float));
        SwapDic<ResUserDataData>(&data->ofsUserDataDic);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(Vec3* data)
{
    Swap(&data->a, sizeof(f32), sizeof(Vec3) / sizeof(f32));
}

template<bool isReversed>
void Endian<isReversed>::Swap(Vec4* data)
{
    Swap(&data->a, sizeof(f32), sizeof(Vec4) / sizeof(f32));
}

template<bool isReversed>
void Endian<isReversed>::Swap(Quat* data)
{
    Swap(&data->a, sizeof(f32), sizeof(Quat) / sizeof(f32));
}

template<bool isReversed>
void Endian<isReversed>::Swap(Mtx34* data)
{
    Swap(&data->a, sizeof(f32), sizeof(Mtx34) / sizeof(f32));
}

template<bool isReversed>
void Endian<isReversed>::Swap( nw::g3d::res::Bounding* data )
{
    Swap(&data->center);
    Swap(&data->extent);
}

template<bool isReversed>
void Endian<isReversed>::Swap(nw::g3d::fnd::internal::GX2LoopVarData* data)
{
    for (int i = 0; i < GX2_NUM_LOOP_VAR_U32_WORDS; ++i)
    {
        Swap(&data->_regs[i]);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap( nw::g3d::res::BoundingNode* data )
{
    Swap(&data->idxLeft);
    Swap(&data->idxRight);
    Swap(&data->idxPrev);
    Swap(&data->idxNext);
    Swap(&data->idxSubMesh);
    Swap(&data->numSubMesh);
}


template<bool isReversed>
void Endian<isReversed>::Swap(nw::g3d::fnd::internal::GX2VertexShaderData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapArray<fnd::internal::GX2LoopVarData>(&reinterpret_cast<Offset&>(data->_loopVars), data->_numLoops);
    }

    for (int i = 0; i < GX2_NUM_VERTEX_SHADER_REGISTERS; ++i)
    {
        Swap(&data->_regs[i]);
    }
    Swap(&data->shaderSize);
    Swap(&reinterpret_cast<Offset&>(data->shaderPtr));
    Swap(reinterpret_cast<bit32*>(&data->shaderMode));
    Swap(&data->_numLoops);
    Swap(&reinterpret_cast<Offset&>(data->_loopVars));
    Swap(&data->ringItemsize);
    Swap(reinterpret_cast<bit32*>(&data->hasStreamOut));
    for (int i = 0; i < GX2_MAX_STREAMOUT_BUFFERS; ++i)
    {
        Swap(&data->streamOutVertexStride[i]);
    }

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<fnd::internal::GX2LoopVarData>(&reinterpret_cast<Offset&>(data->_loopVars), data->_numLoops);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(nw::g3d::fnd::internal::GX2GeometryShaderData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapArray<fnd::internal::GX2LoopVarData>(&reinterpret_cast<Offset&>(data->_loopVars), data->_numLoops);
    }

    for (int i = 0; i < GX2_NUM_GEOMETRY_SHADER_REGISTERS; ++i)
    {
        Swap(&data->_regs[i]);
    }
    Swap(&data->shaderSize);
    Swap(&reinterpret_cast<Offset&>(data->shaderPtr));
    Swap(&data->copyShaderSize);
    Swap(&reinterpret_cast<Offset&>(data->copyShaderPtr));
    Swap(reinterpret_cast<bit32*>(&data->shaderMode));
    Swap(&data->_numLoops);
    Swap(&reinterpret_cast<Offset&>(data->_loopVars));
    Swap(&data->ringItemsize);
    Swap(reinterpret_cast<bit32*>(&data->hasStreamOut));
    for (int i = 0; i < GX2_MAX_STREAMOUT_BUFFERS; ++i)
    {
        Swap(&data->streamOutVertexStride[i]);
    }

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<fnd::internal::GX2LoopVarData>(&reinterpret_cast<Offset&>(data->_loopVars), data->_numLoops);
    }
}

template<bool isReversed>
void Endian<isReversed>::Swap(nw::g3d::fnd::internal::GX2PixelShaderData* data)
{
    if (NW_G3D_STATIC_CONDITION(!isReversed))
    {
        SwapArray<fnd::internal::GX2LoopVarData>(&reinterpret_cast<Offset&>(data->_loopVars), data->_numLoops);
    }

    for (int i = 0; i < GX2_NUM_PIXEL_SHADER_REGISTERS; ++i)
    {
        Swap(&data->_regs[i]);
    }
    Swap(&data->shaderSize);
    Swap(&reinterpret_cast<Offset&>(data->shaderPtr));
    Swap(reinterpret_cast<bit32*>(&data->shaderMode));
    Swap(&data->_numLoops);
    Swap(&reinterpret_cast<Offset&>(data->_loopVars));

    if (NW_G3D_STATIC_CONDITION(isReversed))
    {
        SwapArray<fnd::internal::GX2LoopVarData>(&reinterpret_cast<Offset&>(data->_loopVars), data->_numLoops);
    }
}

template class Endian<true>;
template class Endian<false>;

} } } // namespace nw::g3d::res

#endif // NW_G3D_IS_HOST_WIN
