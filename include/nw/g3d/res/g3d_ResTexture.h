#ifndef NW_G3D_RES_RESTEXTURE_H_
#define NW_G3D_RES_RESTEXTURE_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/fnd/g3d_GfxObject.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_Binding.h>
#include <nw/g3d/res/g3d_ResUserData.h>

namespace nw { namespace g3d { namespace res {

class BindCallback;
class ResFile;

struct ResTextureData
{
    BinaryBlockHeader blockHeader;
    GfxTexture_t gfxTexture;

    BinString ofsName;
    BinString ofsPath;
    Offset ofsData;
    Offset ofsMipData;
    Offset ofsUserDataDic;

    u16 numUserData;
    u16 reserved;
};

class ResTexture : private ResTextureData
{
    NW_G3D_RES_COMMON(ResTexture);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'T', 'E', 'X') };

    void Setup();
    void Cleanup();

    size_t GetBaseSize() const { return GetGfxTexture()->GetBaseSize(); }

    size_t GetMipSize() const { return GetGfxTexture()->GetMipSize(); }

    void* GetData() { return ref().ofsData.to_ptr(); }

    const void* GetData() const { return ref().ofsData.to_ptr(); }

    void* GetMipData() { return ref().ofsMipData.to_ptr(); }

    const void* GetMipData() const { return ref().ofsMipData.to_ptr(); }

    GfxTexture* GetGfxTexture() { return GfxTexture::DownCast(&ref().gfxTexture); }

    const GfxTexture* GetGfxTexture() const { return GfxTexture::DownCast(&ref().gfxTexture); }

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_STRING_DECL(Path)

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

struct ResTextureRefData
{
    BinString ofsName;
    Offset ofsRefData;
};

class ResTextureRef : private ResTextureRefData
{
    NW_G3D_RES_COMMON(ResTextureRef);

public:
    bool IsBound() const { return ref().ofsRefData != 0; }

    BindResult Bind(const ResFile* pFile);

    void ForceBind(const ResTexture* pTarget);

    void Release();

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_DEREFERENCE_DECL(Texture)
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESTEXTURE_H_
