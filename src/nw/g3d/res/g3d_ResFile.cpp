#include <nw/g3d/res/g3d_ResFile.h>

namespace nw { namespace g3d { namespace res {

ResFile* ResFile::ResCast(void* ptr)
{
    NW_G3D_ASSERT_NOT_NULL(ptr);
    ResFileData* pData = static_cast<ResFileData*>(ptr);
    ResFile* pFile = static_cast<ResFile*>(pData);
    NW_G3D_ASSERT_ADDR_ALIGNMENT(ptr, pData->alignment);
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
