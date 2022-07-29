
#ifndef NW_G3D_SHAPEOBJ_H_
#define NW_G3D_SHAPEOBJ_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/fnd/g3d_GfxObject.h>
#include <nw/g3d/res/g3d_ResShape.h>
#include <nw/g3d/g3d_Sizer.h>

namespace nw { namespace g3d {

namespace math {

class Vec2;
class Mtx34;

} // namespace math

class SkeletonObj;

struct ShpBlock
{
    Mtx34 worldMtx;
    s32 vtxSkinCount;
    u32 reserved[51];
};

struct Sphere
{
    void Transform(const Sphere& sphere, const Mtx34& mtx);

    void Merge(const Sphere& lhs, const Sphere& rhs);

    Vec3 center;
    float radius;
};

struct AABB
{
    void Set(const Vec3* pPointArray, int count);

    void Transform(const Bounding& aabb, const Mtx34& mtx);

    void Merge(const AABB& lhs, const AABB& rhs);

    Vec3 min;
    Vec3 max;
};

struct Plane
{
    void Set(const Vec3& p0, const Vec3& p1, const Vec3& p2);

    Vec3 normal;
    float dist;
};

struct ViewVolume
{
    void SetPerspective(float fovy, float aspect, float zNear, float zFar, const Mtx34& viewToWorld);
    void SetPerspectiveOffset(float fovy, float aspect, float zNear, float zFar, const Mtx34& viewToWorld, const Vec2& offset);
    void SetFrustum(float top, float bottom, float left, float right, float zNear, float zFar,
        const Mtx34& viewToWorld);
    void SetOrtho(float top, float bottom, float left, float right, float zNear, float zFar,
        const Mtx34& viewToWorld);

    bool TestIntersection(const Sphere& sphere) const;

    int TestIntersectionEx(const Sphere& sphere) const;

    bool TestIntersection(const AABB& aabb) const;

    int TestIntersectionEx(const AABB& aabb) const;

    void EnableAABB() { flag = 1; }
    void DisableAABB() { flag = 0; }
    bool IsAABBEnabled() const { return !!flag; }

    AABB aabb;
    Plane planes[6];
    int numPlanes;
    int flag;
};

struct CullingContext
{
    CullingContext() : nodeIndex(0), submeshIndex(0), submeshCount(0) {}

    int nodeIndex;
    int submeshIndex;
    int submeshCount;
};

class ShapeObj
{
public:
    class InitArg;

    enum Alignment
    {
        BUFFER_ALIGNMENT        = CACHE_BLOCK_SIZE,

        BLOCK_BUFFER_ALIGNMENT  = GX2_UNIFORM_BLOCK_ALIGNMENT
    };

    ShapeObj() : m_pRes(NULL), m_pUserPtr(NULL), m_pBufferPtr(NULL), m_pBlockBuffer(NULL) {}

    static size_t CalcBufferSize(const InitArg& arg);

    bool Init(const InitArg& arg, void* pBuffer, size_t bufferSize);

    size_t CalcBlockBufferSize() const;

    bool SetupBlockBuffer(void* pBuffer, size_t bufferSize);

    void CleanupBlockBuffer();

    void CalcBounding(const SkeletonObj* pSkeleton);

    void CalcSubBounding(const SkeletonObj* pSkeleton);

    void CalcShpBlock(int viewIndex, const Mtx34& worldMtx, int bufferIndex = 0);

    ResShape* GetResource() { return m_pRes; }

    const ResShape* GetResource() const { return m_pRes; }

    void* GetBufferPtr() { return m_pBufferPtr; }

    void* GetBlockBufferPtr() { return m_pBlockBuffer; }

    bool IsBlockBufferValid() const { return (m_Flag & BLOCK_BUFFER_VALID) != 0; }

    int GetMaterialIndex() const { return m_pRes->GetMaterialIndex(); }

    int GetBoneIndex() const { return m_pRes->GetBoneIndex(); }

    int GetVertexIndex() const { return m_pRes->GetVertexIndex(); }

    int GetVtxSkinCount() const { return m_pRes->GetVtxSkinCount(); }

    bool IsRigidBody() const { return m_pRes->IsRigidBody(); }

    bool IsRigidSkinning() const { return m_pRes->IsRigidSkinning(); }

    bool IsSmoothSkinning() const { return m_pRes->IsSmoothSkinning(); }

    ResMesh* GetResMesh(int meshIndex = 0) { return m_pRes->GetMesh(meshIndex); }

    const ResMesh* GetResMesh(int meshIndex = 0) const { return m_pRes->GetMesh(meshIndex); }

    int GetMeshCount() const { return m_pRes->GetMeshCount(); }

    int GetViewCount() const { return m_NumView; }

    int GetSubMeshCount() const { return m_pRes->GetSubMeshCount(); }

    bool IsViewDependent() const { return m_ViewDependent != 0; }

    int GetShpBlockCount() const { return m_NumShpBlock; }

    GfxBuffer& GetShpBlock(int viewIndex)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(viewIndex, m_NumView);
        return m_pShpBlockArray[viewIndex * m_ViewDependent];
    }

    const GfxBuffer& GetShpBlock(int viewIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(viewIndex, m_NumView);
        return m_pShpBlockArray[viewIndex * m_ViewDependent];
    }

    int GetBufferingCount() const { return m_NumBuffering; }

    void EnableBlockSwap() { m_Flag |= BLOCK_BUFFER_SWAP; }

    void DisableBlockSwap() { m_Flag &= ~BLOCK_BUFFER_SWAP; }

    bool IsBlockSwapEnabled() const { return (m_Flag & BLOCK_BUFFER_SWAP) != 0; }

    void SetUserPtr(void* pUserPtr) { m_pUserPtr = pUserPtr; }

    void* GetUserPtr() { return m_pUserPtr; }

    const void* GetUserPtr() const { return m_pUserPtr; }

    template <typename T>
    T* GetUserPtr() { return static_cast<T*>(m_pUserPtr); }

    template <typename T>
    const T* GetUserPtr() const { return static_cast<const T*>(m_pUserPtr); }

    ResVertex* GetResVertex() { return m_pRes->GetVertex(); }

    const ResVertex* GetResVertex() const { return m_pRes->GetVertex(); }

    int GetVtxAttribCount() const { return GetResVertex()->GetVtxAttribCount(); }

    ResVtxAttrib* GetResVtxAttrib(int attribIndex)
    {
        return GetResVertex()->GetVtxAttrib(attribIndex);
    }

    const ResVtxAttrib* GetResVtxAttrib(int attribIndex) const
    {
        return GetResVertex()->GetVtxAttrib(attribIndex);
    }

    ResVtxAttrib* GetResVtxAttrib(const char* name)
    {
        return GetResVertex()->GetVtxAttrib(name);
    }

    const ResVtxAttrib* GetResVtxAttrib(const char* name) const
    {
        return GetResVertex()->GetVtxAttrib(name);
    }

    const char* GetVtxAttribName(int attribIndex) const
    {
        return GetResVertex()->GetVtxAttribName(attribIndex);
    }

    int GetVtxAttribIndex(const char* name) const
    {
        return GetResVertex()->GetVtxAttribIndex(name);
    }

    int GetVertexBufferCount() const { return GetResVertex()->GetVtxBufferCount(); }

    GfxBuffer& GetVertexBuffer(int bufferIndex)
    {
        return *GetResVertex()->GetVtxBuffer(bufferIndex)->GetGfxBuffer();
    }

    const GfxBuffer& GetVertexBuffer(int bufferIndex) const
    {
        return *GetResVertex()->GetVtxBuffer(bufferIndex)->GetGfxBuffer();
    }

    Sphere* GetBounding() { return m_pBounding; }

    const Sphere* GetBounding() const { return m_pBounding; }

    const AABB* GetSubBoundingArray() const { return m_pSubBounding; }

    const u16* GetSubMeshIndexArray() const { return m_pRes->GetSubMeshIndexArray(); }

    bool TestSubMeshIntersection(CullingContext* pCtx, const ViewVolume& volume) const;

    int GetKeyShapeCount() const { return m_pRes->GetKeyShapeCount(); }

    ResKeyShape* GetResKeyShape(int keyShapeIndex)
    {
        return m_pRes->GetKeyShape(keyShapeIndex);
    }

    const ResKeyShape* GetResKeyShape(int keyShapeIndex) const
    {
        return m_pRes->GetKeyShape(keyShapeIndex);
    }

    ResKeyShape* GetResKeyShape(const char* name)
    {
        return m_pRes->GetKeyShape(name);
    }

    const ResKeyShape* GetResKeyShape(const char* name) const
    {
        return m_pRes->GetKeyShape(name);
    }

    const char* GetKeyShapeName(int keyShapeIndex) const
    {
        return m_pRes->GetKeyShapeName(keyShapeIndex);
    }

    int GetKeyShapeIndex(const char* name) const
    {
        return m_pRes->GetKeyShapeIndex(name);
    }

    int GetTargetAttribCount() const { return m_pRes->GetTargetAttribCount(); }

    void ClearBlendWeights();

    NW_G3D_DEPRECATED_FUNCTION(bool IsBlendWeightValid() const) { return 0 != (m_Flag & BLEND_WEIGHT_VALID); }

    bool HasValidBlendWeight() const { return 0 != (m_Flag & BLEND_WEIGHT_VALID); }

    void SetBlendWeight(int keyShapeIndex, float weight)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(keyShapeIndex, GetKeyShapeCount());
        m_pBlendWeightArray[keyShapeIndex] = weight;
        m_Flag |= BLEND_WEIGHT_VALID;

    }

    float GetBlendWeight(int keyShapeIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(keyShapeIndex, GetKeyShapeCount());
        return m_pBlendWeightArray[keyShapeIndex];
    }

protected:
    enum Flag
    {
        BLOCK_BUFFER_VALID  = 0x1 << 0,
        BLOCK_BUFFER_SWAP   = 0x1 << 1,
        BLEND_WEIGHT_VALID  = 0x1 << 2
    };

    enum BoundingCoord
    {
        WORLD_COORD     = 0,
        LOCAL_COORD     = 1,
        NUM_COORD
    };

private:
    class Sizer;
    ResShape* m_pRes;

    bit32 m_Flag;

    u8 m_NumView;
    u8 m_ViewDependent;
    u8 m_NumShpBlock;
    u8 m_NumBuffering;
    GfxBuffer* m_pShpBlockArray;
    float* m_pBlendWeightArray;

    Sphere* m_pBounding;
    AABB* m_pSubBounding;

    void* m_pUserPtr;
    void* m_pBufferPtr;
    void* m_pBlockBuffer;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(ShapeObj);
};

class ShapeObj::Sizer : public nw::g3d::Sizer
{
public:
    Sizer() : nw::g3d::Sizer() {}

    void Calc(const InitArg& arg);

    enum
    {
        SUBBOUNDING_ARRAY,
        SHAPE_BLOCK_ARRAY,
        BLEND_WEIGHT_ARRAY,

        BOUNDING_ARRAY,

        NUM_CHUNK
    };

    Chunk chunk[NUM_CHUNK];
};

class ShapeObj::InitArg
{
public:
    explicit InitArg(ResShape* resource)
        : m_pRes(resource)
        , m_NumBuffering(1)
        , m_NumView(1)
        , m_ViewDependent(false)
        , m_BoundingEnabled(false)

    {
        NW_G3D_ASSERT_NOT_NULL(resource);
    }

    void BufferingCount(int count) { m_NumBuffering = count; m_Sizer.Invalidate(); }

    void ViewCount(int count) { m_NumView = count; m_Sizer.Invalidate(); }

    void ViewDependent() { m_ViewDependent = true; m_Sizer.Invalidate(); }

    void ViewIndependent() { m_ViewDependent = false; m_Sizer.Invalidate(); }

    void EnableBounding() { m_BoundingEnabled = true; m_Sizer.Invalidate(); }

    void DisableBounding() { m_BoundingEnabled = false; m_Sizer.Invalidate(); }

    int GetBufferingCount() const { return m_NumBuffering; }

    int GetViewCount() const { return m_NumView; }

    bool IsViewDependent() const { return m_ViewDependent; }

    bool IsBoundingEnabled() const { return m_BoundingEnabled; }

    ResShape* GetResource() const { return m_pRes; }

    Sizer& GetSizer() const { return m_Sizer; }

private:
    ResShape* m_pRes;
    int m_NumBuffering;
    int m_NumView;
    bool m_ViewDependent;
    bool m_BoundingEnabled;

    mutable Sizer m_Sizer;
};

} } // namespace nw::g3d

#endif // NW_G3D_SHAPEOBJ_H_
