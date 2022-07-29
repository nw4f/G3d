#ifndef NW_G3D_RES_RESFILE_H_
#define NW_G3D_RES_RESFILE_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_Binding.h>
#include <nw/g3d/res/g3d_ResModel.h>
#include <nw/g3d/res/g3d_ResSkeleton.h>
#include <nw/g3d/res/g3d_ResShape.h>
#include <nw/g3d/res/g3d_ResMaterial.h>
#include <nw/g3d/res/g3d_ResTexture.h>
#include <nw/g3d/res/g3d_ResSkeletalAnim.h>
#include <nw/g3d/res/g3d_ResTexPatternAnim.h>
#include <nw/g3d/res/g3d_ResShaderParamAnim.h>
#include <nw/g3d/res/g3d_ResVisibilityAnim.h>
#include <nw/g3d/res/g3d_ResShapeAnim.h>
#include <nw/g3d/res/g3d_ResSceneAnim.h>

namespace nw { namespace g3d { namespace res {

class BindCallback;

struct ResFileData
{
    BinaryFileHeader fileHeader;
    u32 alignment;
    BinString ofsName;
    u32 sizeStringPool;
    Offset ofsStringPool;

    Offset ofsModelDic;
    Offset ofsTextureDic;
    Offset ofsSkeletalAnimDic;
    Offset ofsShaderParamAnimDic;
    Offset ofsColorAnimDic;
    Offset ofsTexSrtAnimDic;
    Offset ofsTexPatternAnimDic;
    Offset ofsBoneVisAnimDic;
    Offset ofsMatVisAnimDic;
    Offset ofsShapeAnimDic;
    Offset ofsSceneAnimDic;
    Offset ofsExternalFileDic;

    u16 numModel;
    u16 numTexture;
    u16 numSkeletalAnim;
    u16 numShaderParamAnim;
    u16 numColorAnim;
    u16 numTexSrtAnim;
    u16 numTexPatternAnim;
    u16 numBoneVisAnim;
    u16 numMatVisAnim;
    u16 numShapeAnim;
    u16 numSceneAnim;
    u16 numExternalFile;

    BinPtr pUserPtr;
};

class ResFile : private ResFileData
{
public:
    typedef ResFileData DataType;

    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'R', 'E', 'S') };

    static ResFile* ResCast(void* ptr);

    void Setup();
    void Cleanup();
    void Reset();

    BindResult Bind(const ResFile* pFile, bit32 typeMask);
    BindResult Bind(const ResFile* pFile, bit32 typeMask, BindCallback* pCallback);
    BindResult Bind(const BindFuncTable& funcTable);

    void Release(bit32 typeMask);

    static const char* GetClassName() { return "ResFile"; }

    DataType& ref() { NW_G3D_VALIDITY_ASSERT; return *ptr(); }
    const DataType& ref() const { NW_G3D_VALIDITY_ASSERT; return *ptr(); }

    DataType* ptr() { return this; }
    const DataType* ptr() const { return this; }

    static bool IsValid(const void* ptr);

    const BinaryFileHeader* GetFileHeader() const { return &ref().fileHeader; }

    NW_G3D_RES_FIELD_STRING_DECL(Name)

    void SetUserPtr(void* pUserPtr) { ref().pUserPtr.set_ptr(pUserPtr); }

    void* GetUserPtr() { return ref().pUserPtr.to_ptr(); }
    const void* GetUserPtr() const { return ref().pUserPtr.to_ptr(); }

    template <typename T>
    T* GetUserPtr() { return ref().pUserPtr.to_ptr<T>(); }

    template <typename T>
    const T* GetUserPtr() const { return ref().pUserPtr.to_ptr<T>(); }

    NW_G3D_RES_FIELD_DIC_DECL(ResModel, Model)

    NW_G3D_RES_FIELD_DIC_DECL(ResTexture, Texture)

    NW_G3D_RES_FIELD_DIC_DECL(ResSkeletalAnim, SkeletalAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResShaderParamAnim, ShaderParamAnim)
    NW_G3D_RES_FIELD_DIC_DECL(ResShaderParamAnim, ColorAnim)
    NW_G3D_RES_FIELD_DIC_DECL(ResShaderParamAnim, TexSrtAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResTexPatternAnim, TexPatternAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResVisibilityAnim, BoneVisAnim)
    NW_G3D_RES_FIELD_DIC_DECL(ResVisibilityAnim, MatVisAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResShapeAnim, ShapeAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResSceneAnim, SceneAnim)

    NW_G3D_RES_FIELD_DIC_DECL(ResExternalFile, ExternalFile)

private:
    NW_G3D_DISALLOW_COPY_AND_ASSIGN(ResFile);
};

class BindCallback
{
public:
    BindCallback() : m_pFile(NULL), m_pModel(NULL), m_pMaterial(NULL), m_pTexPatternAnim(NULL) {}

    virtual ~BindCallback() {}

    virtual BindResult ModelTexNotBound(ResTextureRef* textureRef)
    {
        (void)textureRef;
        return BindResult::NotBound(BIND_TEXTURE);
    }

    virtual BindResult AnimTexNotBound(ResTextureRef* textureRef)
    {
        (void)textureRef;
        return BindResult::NotBound(BIND_TEXTURE);
    }

    ResFile* GetFile() { return m_pFile; }

    ResModel* GetModel() { return m_pModel; }

    ResMaterial* GetMaterial() { return m_pMaterial; }

    ResTexPatternAnim* GetTexPatternAnim() { return m_pTexPatternAnim; }

private:
    ResFile* m_pFile;
    ResModel* m_pModel;
    ResMaterial* m_pMaterial;
    ResTexPatternAnim* m_pTexPatternAnim;

    friend class ResFile;
    friend class ResModel;
};

} } } // namespace nw::g3d

#endif // NW_G3D_RES_RESFILE_H_
