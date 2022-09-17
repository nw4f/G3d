#include <nw/g3d/res/g3d_ResFile.h>
#include <nw/g3d/res/g3d_ResUtility.h>

#if NW_G3D_IS_HOST_WIN && !defined( NW_STRIP_GL )
    #include <nw/g3d/fnd/g3d_GX2Utility.h>
#endif

namespace nw { namespace g3d { namespace res {

ResFile* ResFile::ResCast(void* ptr)
{
    NW_G3D_ASSERT_NOT_NULL(ptr);
    ResFileData* pData = static_cast<ResFileData*>(ptr);
    ResFile* pFile = static_cast<ResFile*>(pData);

#if NW_G3D_IS_HOST_WIN
    if (pData != NULL)
    {
        if (pData->fileHeader.byteOrder != BinaryFileHeader::BYTE_ORDER_MARK)
            Endian<true>::Swap(pData);

#if !defined( NW_STRIP_GL )
        for (int idxTex = 0, numTex = pFile->GetTextureCount(); idxTex < numTex; ++idxTex)
        {
            ResTexture* pResTex = pFile->GetTexture(idxTex);
            GfxTexture* pGfxTex = pResTex->GetGfxTexture();
            GX2Surface& surface = pGfxTex->GetGX2Texture()->surface;
            GX2CalcSurfaceSizeAndAlignment(&surface);
            if (surface.tileMode != GX2_TILE_MODE_LINEAR_SPECIAL)
            {
                void* mipData = pResTex->GetMipData();

                pGfxTex->SetImagePtrs(pResTex->GetData(), mipData);

                if (mipData)
                {
                    NW_G3D_ASSERT(mipData == surface.mipPtr);
                }

                ConvertToGLSurface(surface, pGfxTex->arrayLength);

                if (mipData && mipData != surface.mipPtr)
                {
                    ResTextureData& pResTexData = pResTex->ref();
                    pResTexData.ofsMipData = s32(pResTexData.ofsMipData) - ((uintptr_t)mipData - (uintptr_t)surface.mipPtr);
                    NW_G3D_ASSERT(pResTex->GetMipData() == surface.mipPtr);
                }

                pGfxTex->SetImagePtrs(NULL, NULL);
            }
        }
#endif
    }
#elif NW_G3D_IS_HOST_CAFE
    NW_G3D_ASSERT_ADDR_ALIGNMENT(ptr, pData->alignment);
#endif
    return pFile;
}

BindResult ResFile::Bind(const ResFile* pFile, bit32 typeMask)
{
    NW_G3D_ASSERT_NOT_NULL(pFile);

    BindResult result;

    if (typeMask & BIND_TEXTURE)
    {
        for (int idxModel = 0, numModel = GetModelCount(); idxModel < numModel; ++idxModel)
        {
            ResModel* pModel = GetModel(idxModel);
            result |= pModel->Bind(pFile);
        }

        for (int idxTexPat = 0, numTexPat = GetTexPatternAnimCount();
            idxTexPat < numTexPat; ++idxTexPat)
        {
            ResTexPatternAnim* pTexPat = GetTexPatternAnim(idxTexPat);
            result |= pTexPat->Bind(pFile);
        }
    }

    return result;
}

void ResFile::Setup()
{
    int numModel = this->GetModelCount();
    for (int i = 0; i < numModel; ++i)
    {
        this->GetModel(i)->Setup();
    }

    int numTexture = this->GetTextureCount();
    for (int i = 0; i < numTexture; ++i)
    {
        this->GetTexture(i)->Setup();
    }
}

void ResFile::Cleanup()
{
    int numModel = this->GetModelCount();
    for (int i = 0; i < numModel; ++i)
    {
        this->GetModel(i)->Cleanup();
    }

    int numTexture = this->GetTextureCount();
    for (int i = 0; i < numTexture; ++i)
    {
        this->GetTexture(i)->Cleanup();
    }
}

} } } // namespace nw::g3d::res
