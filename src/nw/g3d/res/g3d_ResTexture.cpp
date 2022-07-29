#include <nw/g3d/res/g3d_ResTexture.h>
#include <nw/g3d/res/g3d_ResFile.h>
#include <string>

namespace nw { namespace g3d { namespace res {

void ResTexture::Setup()
{
    GfxTexture* texture = this->GetGfxTexture();
    texture->SetImagePtrs(this->GetData(), this->GetMipData());

    texture->Setup();
    texture->DCFlush();
}

void ResTexture::Cleanup()
{
    GfxTexture* texture = this->GetGfxTexture();
    texture->SetImagePtrs(NULL, NULL);

    texture->Cleanup();
}

BindResult ResTextureRef::Bind(const ResFile* pFile)
{
    NW_G3D_ASSERT_NOT_NULL(pFile);

    if (ref().ofsRefData == 0)
    {
        const ResTexture* pTexture = pFile->GetTexture(ref().ofsName.GetResName());
        if (pTexture != NULL)
        {
            ref().ofsRefData.set_ptr(pTexture->ptr());
            return BindResult::Bound(BIND_TEXTURE);
        }
        else
        {
            return BindResult::NotBound(BIND_TEXTURE);
        }
    }
    return BindResult();
}

void ResTextureRef::ForceBind(const ResTexture* pTarget)
{
    NW_G3D_ASSERT_NOT_NULL(pTarget);

    ref().ofsRefData.set_ptr(pTarget->ptr());
}

}}} // namespace nw::g3d::res
