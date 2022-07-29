#ifndef NW_G3D_RES_RESSHAPE_H_
#define NW_G3D_RES_RESSHAPE_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/math/g3d_Vector3.h>
#include <nw/g3d/fnd/g3d_GfxObject.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>

namespace nw { namespace g3d { namespace res {

class ResFile;

struct ResBufferData
{
    GfxBuffer_t gfxBuffer;
    Offset ofsData;
};

class ResBuffer : private ResBufferData
{
    NW_G3D_RES_COMMON(ResBuffer);

public:
    void Setup();
    void Cleanup();

    GfxBuffer* GetGfxBuffer()
    {
        return GfxBuffer::DownCast(&ref().gfxBuffer);
    }

    const GfxBuffer* GetGfxBuffer() const
    {
        return GfxBuffer::DownCast(&ref().gfxBuffer);
    }

    size_t GetSize() const { return ref().gfxBuffer.size; }

    void* GetData() { return ref().ofsData.to_ptr(); }

    const void* GetData() const { return ref().ofsData.to_ptr(); }
};

struct ResVtxAttribData
{
    BinString ofsName;

    u8 bufferIndex;
    u8 reserved;
    u16 offset;

    GX2AttribFormat format;
};

class ResVtxAttrib : private ResVtxAttribData
{
    NW_G3D_RES_COMMON(ResVtxAttrib);

public:
    NW_G3D_RES_FIELD_STRING_DECL(Name)

    int GetBufferIndex() const { return ref().bufferIndex; }

    GX2AttribFormat GetFormat() const { return ref().format; }

    u32 GetOffset() const { return ref().offset; }
};

struct ResVertexData
{
    BinaryBlockHeader blockHeader;

    u8 numVtxAttrib;
    u8 numVtxBuffer;
    u16 index;
    u32 count;
    u8 vtxSkinCount;
    u8 reserved[3];

    Offset ofsVtxAttribArray;
    Offset ofsVtxAttribDic;
    Offset ofsVtxBufferArray;

    BinPtr pUserPtr;
};

class ResVertex : private ResVertexData
{
    NW_G3D_RES_COMMON(ResVertex);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'V', 'T', 'X') };

    void Setup();
    void Cleanup();
    void Reset();

    int GetIndex() const { return static_cast<int>(ref().index); }

    u32 GetCount() const { return ref().count; }

    int GetVtxSkinCount() const { return ref().vtxSkinCount; }

    void SetUserPtr(void* pUserPtr) { ref().pUserPtr.set_ptr(pUserPtr); }

    void* GetUserPtr() { return ref().pUserPtr.to_ptr(); }

    const void* GetUserPtr() const { return ref().pUserPtr.to_ptr(); }

    template <typename T>
    T* GetUserPtr() { return ref().pUserPtr.to_ptr<T>(); }

    template <typename T>
    const T* GetUserPtr() const { return ref().pUserPtr.to_ptr<T>(); }

    NW_G3D_RES_FIELD_CLASS_NAMED_ARRAY_DECL(ResVtxAttrib, VtxAttrib)

    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResBuffer, VtxBuffer)
};

struct ResSubMeshData
{
    u32 offset;
    u32 count;
};

class ResSubMesh : private ResSubMeshData
{
    NW_G3D_RES_COMMON(ResSubMesh);

public:
    u32 GetOffset() const { return ref().offset; }

    u32 GetCount() const { return ref().count; }
};

struct ResMeshData
{
    GX2PrimitiveType primType;
    GX2IndexFormat format;
    u32 count;

    u16 numSubMesh;
    u16 reserved;
    Offset ofsSubMeshArray;
    Offset ofsIdxBuffer;
    u32 offset; // Unused?
};

class ResMesh : private ResMeshData
{
    NW_G3D_RES_COMMON(ResMesh);

public:
    void Draw(int instances = 1) const
    {
        DrawSubMesh(0, GetSubMeshCount(), instances);
    }

    void DrawSubMesh(int submeshIndex, int submeshCount, int instances = 1) const;

    GX2PrimitiveType GetPrimitiveType() const { return ref().primType; }

    GX2IndexFormat GetIndexFormat() const { return ref().format; }

    u32 GetCount() const { return ref().count; }

    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResSubMesh, SubMesh)

    NW_G3D_RES_FIELD_CLASS_DECL(ResBuffer, IdxBuffer);
};

struct ResKeyShapeData
{
    u8 targetAttribIndices[20];
    u8 ofsTargetAttribIndex[4];
};

class ResKeyShape : private ResKeyShapeData
{
    NW_G3D_RES_COMMON(ResKeyShape);

public:

    enum KeyAttrib
    {
        NUM_KEY_ATTRIB_POSITION = 1,
        NUM_KEY_ATTRIB_NORMAL = 1,
        NUM_KEY_ATTRIB_TANGENT = 4,
        NUM_KEY_ATTRIB_BINORMAL = 4,
        NUM_KEY_ATTRIB_COLOR = 8,

        KEY_ATTRIB_POSITION_OFFSET = 0,
        KEY_ATTRIB_NORMAL_OFFSET = KEY_ATTRIB_POSITION_OFFSET + NUM_KEY_ATTRIB_POSITION,
        KEY_ATTRIB_TANGENT_OFFSET = KEY_ATTRIB_NORMAL_OFFSET + NUM_KEY_ATTRIB_NORMAL,
        KEY_ATTRIB_BINORMAL_OFFSET = KEY_ATTRIB_TANGENT_OFFSET + NUM_KEY_ATTRIB_TANGENT,
        KEY_ATTRIB_COLOR_OFFSET = KEY_ATTRIB_BINORMAL_OFFSET + NUM_KEY_ATTRIB_BINORMAL
    };

    int GetPositionAttribIndex() const
    {
        return ref().targetAttribIndices[KEY_ATTRIB_POSITION_OFFSET] - 1;
    }

    int GetNormalAttribIndex() const
    {
        return ref().targetAttribIndices[KEY_ATTRIB_NORMAL_OFFSET] - 1;
    }

    int GetTangentAttribIndex(int index = 0) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(index, NUM_KEY_ATTRIB_TANGENT);
        return ref().targetAttribIndices[KEY_ATTRIB_TANGENT_OFFSET + index] - 1;
    }

    int GetBinormalAttribIndex(int index = 0) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(index, NUM_KEY_ATTRIB_BINORMAL);
        return ref().targetAttribIndices[KEY_ATTRIB_BINORMAL_OFFSET + index] - 1;
    }

    int GetColorAttribIndex(int index = 0) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(index, NUM_KEY_ATTRIB_COLOR);
        return ref().targetAttribIndices[KEY_ATTRIB_COLOR_OFFSET + index] - 1;
    }

    NW_G3D_DEPRECATED_FUNCTION(int GetTargetAttribIndex(int attribIndex) const)
    {
        return ref().ofsTargetAttribIndex[attribIndex] - 1;
    }
};

struct BoundingNode
{
    u16 idxLeft;
    u16 idxRight;
    u16 idxPrev;
    u16 idxNext;
    u16 idxSubMesh;
    u16 numSubMesh;
};

struct Bounding
{
    Vec3 center;
    Vec3 extent;
};

struct ResShapeData
{
    BinaryBlockHeader blockHeader;

    BinString ofsName;

    bit32 flag;
    u16 index;
    u16 materialIndex;
    u16 boneIndex;
    u16 vertexIndex;
    u16 numSkinBoneIndex;
    u8 vtxSkinCount;
    u8 numMesh;
    u8 numKeyShape;
    u8 numTargetAttrib;
    u16 numBoundingNode;
    f32 radius;

    Offset ofsVertex;
    Offset ofsMeshArray;
    Offset ofsSkinBoneIndexArray;
    Offset ofsKeyShapeDic;
    Offset ofsBoundingNodeArray;
    Offset ofsSubBoundingArray;
    Offset ofsSubMeshIndexArray;

    BinPtr pUserPtr;
};

class ResShape : private ResShapeData
{
    NW_G3D_RES_COMMON(ResShape);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'S', 'H', 'P') };

    enum Flag
    {
        OWN_VERTEX          = 0x1 << 1,
    };

    void Setup();
    void Cleanup();
    void Reset();

    int GetIndex() const { return ref().index; }

    NW_G3D_RES_FIELD_STRING_DECL(Name)

    int GetMaterialIndex() const { return ref().materialIndex; }

    int GetBoneIndex() const { return ref().boneIndex; }

    int GetVertexIndex() const { return ref().vertexIndex; }

    bool IsVertexOwner() const { return 0 != (ref().flag & OWN_VERTEX); }

    int GetSkinBoneIndexCount() const { return ref().numSkinBoneIndex; }

    const u16* GetSkinBoneIndexArray() const { return ref().ofsSkinBoneIndexArray.to_ptr<u16>(); }

    int GetVtxSkinCount() const { return ref().vtxSkinCount; }

    bool IsRigidBody() const { return ref().vtxSkinCount == 0; }

    bool IsRigidSkinning() const { return ref().vtxSkinCount == 1; }

    bool IsSmoothSkinning() const { return ref().vtxSkinCount > 1; }

    void SetUserPtr(void* pUserPtr) { ref().pUserPtr.set_ptr(pUserPtr); }

    void* GetUserPtr() { return ref().pUserPtr.to_ptr(); }

    const void* GetUserPtr() const { return ref().pUserPtr.to_ptr(); }

    template <typename T>
    T* GetUserPtr() { return ref().pUserPtr.to_ptr<T>(); }

    template <typename T>
    const T* GetUserPtr() const { return ref().pUserPtr.to_ptr<T>(); }

    NW_G3D_RES_FIELD_CLASS_DECL(ResVertex, Vertex);

    NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(ResMesh, Mesh);

    ResMesh* GetMesh() { return ResMesh::ResCast(ref().ofsMeshArray.to_ptr<ResMeshData>()); }

    const ResMesh* GetMesh() const { return ResMesh::ResCast(ref().ofsMeshArray.to_ptr<ResMeshData>()); }

    int GetSubMeshCount() const { return ref().ofsMeshArray.to_ptr<ResMeshData>()->numSubMesh; }

    NW_G3D_RES_FIELD_DIC_DECL(ResKeyShape, KeyShape);

    int GetTargetAttribCount() const { return ref().numTargetAttrib; }

    const BoundingNode* GetBoundingNodeArray() const
    {
        return ref().ofsBoundingNodeArray.to_ptr<BoundingNode>();
    }

    int GetBoundingNodeCount() const { return ref().numBoundingNode; }

    const Bounding& GetBounding() const
    {
        return *(ref().ofsSubBoundingArray.to_ptr<Bounding>());
    }

    const u16* GetSubMeshIndexArray() const
    {
        return ref().ofsSubMeshIndexArray.to_ptr<u16>();
    }

    float GetRadius() const { return ref().radius; }
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESSHAPE_H_
