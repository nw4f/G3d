#ifndef NW_G3D_RES_RESMODEL_H_
#define NW_G3D_RES_RESMODEL_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_ResSkeleton.h>
#include <nw/g3d/res/g3d_ResShape.h>
#include <nw/g3d/res/g3d_ResMaterial.h>
#include <nw/g3d/res/g3d_ResUserData.h>

namespace nw { namespace g3d { namespace res {

class BindCallback;
class ResFile;

struct ResModelData
{
    BinaryBlockHeader blockHeader;
    BinString ofsName;
    BinString ofsPath;

    Offset ofsSkeleton;
    Offset ofsVertexArray;
    Offset ofsShapeDic;
    Offset ofsMaterialDic;
    Offset ofsUserDataDic;

    u16 numVertex;
    u16 numShape;
    u16 numMaterial;
    u16 numUserData;

    u32 totalProcessVertex;

    BinPtr pUserPtr;
};

class ResModel : private ResModelData
{
    NW_G3D_RES_COMMON(ResModel);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'M', 'D', 'L') };

    void Setup();
    void Cleanup();
    void Reset();

    BindResult Bind(const ResFile* pFile);
    BindResult Bind(const ResFile* pFile, BindCallback* pCallback);
    bool ForceBind(const ResTexture* pTexture, const char* name);

    void Release();

    NW_G3D_RES_FIELD_STRING_DECL(Name)
    NW_G3D_RES_FIELD_STRING_DECL(Path)

    u32 GetTotalProcessVertex() const { return ref().totalProcessVertex; }

    void SetUserPtr(void* pUserPtr) { ref().pUserPtr.set_ptr(pUserPtr); }

    void* GetUserPtr() { return ref().pUserPtr.to_ptr(); }

    const void* GetUserPtr() const { return ref().pUserPtr.to_ptr(); }

    template <typename T>
    T* GetUserPtr() { return ref().pUserPtr.to_ptr<T>(); }

    template <typename T>
    const T* GetUserPtr() const { return ref().pUserPtr.to_ptr<T>(); }

    NW_G3D_RES_FIELD_CLASS_DECL(ResSkeleton, Skeleton)

    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResVertex, Vertex)

    NW_G3D_RES_FIELD_DIC_DECL(ResShape, Shape)

    NW_G3D_RES_FIELD_DIC_DECL(ResMaterial, Material)

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESMODEL_H_
