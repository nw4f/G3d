#ifndef NW_G3D_RES_RESMODEL_H_
#define NW_G3D_RES_RESMODEL_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_ResSkeleton.h>
#include <nw/g3d/res/g3d_ResShape.h>
#include <nw/g3d/res/g3d_ResMaterial.h>
#include <nw/g3d/res/g3d_ResUserData.h>

#include <SerializedPtr.hpp>

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

    SerializedPtr<void> pUserPtr;
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

    void SetUserPtr(void* pUserPtr) { ref().pUserPtr.set(pUserPtr); }

    void* GetUserPtr() { return ref().pUserPtr.get(); }

    const void* GetUserPtr() const { return ref().pUserPtr.get(); }

    template <typename T>
    T* GetUserPtr() { return static_cast<T*>(ref().pUserPtr.get()); }

    template <typename T>
    const T* GetUserPtr() const { return static_cast<const T*>(ref().pUserPtr.get()); }

    NW_G3D_RES_FIELD_CLASS_DECL(ResSkeleton, Skeleton)

    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResVertex, Vertex)

    NW_G3D_RES_FIELD_DIC_DECL(ResShape, Shape)

    NW_G3D_RES_FIELD_DIC_DECL(ResMaterial, Material)

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESMODEL_H_
