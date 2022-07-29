#ifndef NW_G3D_MODELOBJ_H_
#define NW_G3D_MODELOBJ_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResModel.h>
#include <nw/g3d/g3d_SkeletonObj.h>
#include <nw/g3d/g3d_ShapeObj.h>
#include <nw/g3d/g3d_MaterialObj.h>
#include <nw/g3d/g3d_Sizer.h>
#include <nw/g3d/ut/g3d_Flag.h>

namespace nw { namespace g3d {

class ModelObj
{
public:
    class InitArg;

    enum Alignment
    {
        BUFFER_ALIGNMENT        = SkeletonObj::BUFFER_ALIGNMENT,

        BLOCK_BUFFER_ALIGNMENT  = GX2_UNIFORM_BLOCK_ALIGNMENT
    };

    ModelObj() : m_pRes(NULL), m_pUserPtr(NULL), m_pBufferPtr(NULL), m_pBlockBuffer(NULL) {}

    static size_t CalcBufferSize(const InitArg& arg);

    bool Init(const InitArg& arg, void* pBuffer, size_t bufferSize);

    size_t CalcBlockBufferSize();

    bool SetupBlockBuffer(void* pBuffer, size_t bufferSize);

    void CleanupBlockBuffer();

    void CalcWorld(const Mtx34& baseMtx);

    void CalcBounding();

    void CalcMtxBlock(int bufferIndex = 0);

    void CalcShape(int bufferIndex = 0);

    void CalcMaterial();

    void CalcView(int viewIndex, const Mtx34& cameraMtx, int bufferIndex = 0);

    ResModel* GetResource() { return m_pRes; }

    const ResModel* GetResource() const { return m_pRes; }

    void* GetBufferPtr() { return m_pBufferPtr; }

    void* GetBlockBufferPtr() { return m_pBlockBuffer; }

    bool IsBlockBufferValid() const { return (m_Flag & BLOCK_BUFFER_VALID) != 0; }

    void EnableBlockSwapAll();

    void DisableBlockSwapAll();

    int GetViewCount() const { return m_NumView; }

    bool IsViewDependent() const { return !!m_ViewDependent; }

    void UpdateViewDependency();

    Sphere* GetBounding() { return m_pBounding; }

    const Sphere* GetBounding() const { return m_pBounding; }

    void SetUserPtr(void* pUserPtr) { m_pUserPtr = pUserPtr; }

    void* GetUserPtr() { return m_pUserPtr; }

    const void* GetUserPtr() const { return m_pUserPtr; }

    template <typename T>
    T* GetUserPtr() { return static_cast<T*>(m_pUserPtr); }

    template <typename T>
    const T* GetUserPtr() const { return static_cast<const T*>(m_pUserPtr); }

    bool IsShapeVisible(int shapeIndex) const
    {
        const ShapeObj* pShape = GetShape(shapeIndex);
        return IsMatVisible(pShape->GetMaterialIndex()) &&
            IsBoneVisible(pShape->GetBoneIndex());
    }

    bool IsBoneVisible(int boneIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(boneIndex, GetSkeleton()->GetBoneCount());
        return IsBitOn<bool>(m_pBoneVisArray, boneIndex);
    }

    void SetBoneVisibility(int boneIndex, bool visible)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(boneIndex, GetSkeleton()->GetBoneCount());
        SetBit(m_pBoneVisArray, boneIndex,  visible ? 1 : 0);
    }

    bit32* GetBoneVisArray() { return m_pBoneVisArray; }

    const bit32* GetBoneVisArray() const { return m_pBoneVisArray; }

    void ClearBoneVisibility();

    bool IsMatVisible(int matIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(matIndex, GetMaterialCount());
        return IsBitOn<bool>(m_pMatVisArray, matIndex);
    }

    void SetMatVisibility(int matIndex, bool visible)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(matIndex, GetMaterialCount());
        SetBit(m_pMatVisArray, matIndex,  visible ? 1 : 0);
    }

    bit32* GetMatVisArray() { return m_pMatVisArray; }

    const bit32* GetMatVisArray() const { return m_pMatVisArray; }

    void ClearMatVisibility();

    SkeletonObj* GetSkeleton() { return m_pSkeleton; }

    const SkeletonObj* GetSkeleton() const { return m_pSkeleton; }

    int GetShapeCount() const { return m_NumShape; }

    ShapeObj* GetShape(int shapeIndex)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(shapeIndex, m_NumShape);
        return &m_pShapeArray[shapeIndex];
    }

    const ShapeObj* GetShape(int shapeIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(shapeIndex, m_NumShape);
        return &m_pShapeArray[shapeIndex];
    }

    ShapeObj* GetShape(const char* name)
    {
        int shapeIndex = m_pRes->GetShapeIndex(name);
        return shapeIndex >= 0 ? GetShape(shapeIndex) : NULL;
    }

    const ShapeObj* GetShape(const char* name) const
    {
        int shapeIndex = m_pRes->GetShapeIndex(name);
        return shapeIndex >= 0 ? GetShape(shapeIndex) : NULL;
    }

    const char* GetShapeName(int shapeIndex) const { return m_pRes->GetShapeName(shapeIndex); }

    int GetShapeIndex(const char* name) const { return m_pRes->GetShapeIndex(name); }

    int GetMaterialCount() const { return m_NumMaterial; }

    MaterialObj* GetMaterial(int matIndex)
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(matIndex, m_NumMaterial);
        return &m_pMaterialArray[matIndex];
    }

    const MaterialObj* GetMaterial(int matIndex) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(matIndex, m_NumMaterial);
        return &m_pMaterialArray[matIndex];
    }

    MaterialObj* GetMaterial(const char* name)
    {
        int matIndex = m_pRes->GetMaterialIndex(name);
        return matIndex >= 0 ? GetMaterial(matIndex) : NULL;
    }

    const MaterialObj* GetMaterial(const char* name) const
    {
        int matIndex = m_pRes->GetMaterialIndex(name);
        return matIndex >= 0 ? GetMaterial(matIndex) : NULL;
    }

    const char* GetMaterialName(int matIndex) const { return m_pRes->GetMaterialName(matIndex); }

    int GetMaterialIndex(const char* name) const { return m_pRes->GetMaterialIndex(name); }

protected:
    class Sizer;

    enum Flag
    {
        BLOCK_BUFFER_VALID  = 0x1 << 0
    };

private:

    ResModel* m_pRes;

    bit32* m_pBoneVisArray;
    bit32* m_pMatVisArray;
    u8 m_NumView;
    u8 m_ViewDependent;
    bit16 m_Flag;
    GfxBuffer* m_pViewBlockArray;

    u16 m_NumShape;
    u16 m_NumMaterial;

    SkeletonObj* m_pSkeleton;
    ShapeObj* m_pShapeArray;
    MaterialObj* m_pMaterialArray;
    Sphere* m_pBounding;

    void* m_pUserPtr;
    void* m_pBufferPtr;
    void* m_pBlockBuffer;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(ModelObj);
};

class ModelObj::Sizer : public nw::g3d::Sizer
{
public:
    Sizer() : nw::g3d::Sizer() {}

    void Calc(const InitArg& arg);

    enum
    {
        SKELETON_BUFFER,
        SHAPE_BUFFER,
        MATERIAL_BUFFER,
        SKELETON,
        SHAPE_ARRAY,
        MATERIAL_ARRAY,
        BONE_VIS_ARRAY,
        MAT_VIS_ARRAY,
        BOUNDING,
        NUM_CHUNK
    };

    Chunk chunk[NUM_CHUNK];
};

class ModelObj::InitArg
{
public:
    explicit InitArg(ResModel* resource)
        : m_pRes(resource)
        , m_pSkeleton(NULL)
        , m_NumSkelBuffering(1)
        , m_NumShpBuffering(1)

        , m_NumView(1)

        , m_BoundingEnabled(false)
    {
        NW_G3D_ASSERT_NOT_NULL(resource);
    }

    void Skeleton(SkeletonObj* pSkeleton) { m_pSkeleton = pSkeleton; m_Sizer.Invalidate(); }

    void SkeletonBufferingCount(int count) { m_NumSkelBuffering = count; m_Sizer.Invalidate(); }

    void ShapeBufferingCount(int count) { m_NumShpBuffering = count; m_Sizer.Invalidate(); }

    void ViewCount(int count) { m_NumView = count; m_Sizer.Invalidate(); }

    void EnableBounding() { m_BoundingEnabled = true; m_Sizer.Invalidate(); }

    void DisableBounding() { m_BoundingEnabled = false; m_Sizer.Invalidate(); }

    ResModel* GetResource() const { return m_pRes; }

    SkeletonObj* GetSkeleton() const { return m_pSkeleton; }

    int GetSkeletonBufferingCount() const { return m_NumSkelBuffering; }

    int GetShapeBufferingCount() const { return m_NumShpBuffering; }

    int GetViewCount() const { return m_NumView; }

    bool IsBoundingEnabled() const { return m_BoundingEnabled; }

    Sizer& GetSizer() const { return m_Sizer; }

private:
    ResModel* m_pRes;
    SkeletonObj* m_pSkeleton;
    int m_NumSkelBuffering;
    int m_NumShpBuffering;

    int m_NumView;

    bool m_BoundingEnabled;
    mutable Sizer m_Sizer;
};

} } // namespace nw::g3d

#endif // NW_G3D_MODELOBJ_H_
