#include <nw/g3d/g3d_ModelObj.h>

namespace nw { namespace g3d {

void ModelObj::Sizer::Calc(const InitArg& arg)
{
    NW_G3D_ASSERT(arg.GetViewCount() > 0);
    NW_G3D_ASSERT(arg.GetSkeletonBufferingCount() > 0);
    NW_G3D_ASSERT(arg.GetShapeBufferingCount() > 0);

    ResModel* pRes = arg.GetResource();

    size_t sizeSkeleton = 0;
    ResSkeleton* pResSkeleton = pRes->GetSkeleton();
    SkeletonObj* pSkeleton = arg.GetSkeleton();
    if (pSkeleton)
    {
        NW_G3D_ASSERT(pResSkeleton == pSkeleton->GetResource());
    }
    else
    {
        SkeletonObj::InitArg argSkeleton(pResSkeleton);
        argSkeleton.BufferingCount(arg.GetSkeletonBufferingCount());
        size_t size = SkeletonObj::CalcBufferSize(argSkeleton);
        sizeSkeleton = Align(size, SkeletonObj::BUFFER_ALIGNMENT);
    }

    int numShape = pRes->GetShapeCount();
    int numMaterial = pRes->GetMaterialCount();
    int numBone = pResSkeleton->GetBoneCount();
    int numView = arg.GetViewCount();

    size_t sizeShape = 0;
    for (int idxShape = 0; idxShape < numShape; ++idxShape)
    {
        ResShape* pResShape = pRes->GetShape(idxShape);
        ShapeObj::InitArg argShape(pResShape);
        argShape.BufferingCount(arg.GetShapeBufferingCount());
        ResBone* pResBone = pResSkeleton->GetBone(pResShape->GetBoneIndex());
        if (pResBone->GetBillboardMode() == ResBone::BILLBOARD_NONE)
        {
            argShape.ViewIndependent();
        }
        else
        {
            argShape.ViewDependent();
        }
        argShape.ViewCount(numView);
        if (arg.IsBoundingEnabled())
        {
            argShape.EnableBounding();
        }
        else
        {
            argShape.DisableBounding();
        }

        size_t size = ShapeObj::CalcBufferSize(argShape);
        sizeShape += Align(size, ShapeObj::BUFFER_ALIGNMENT);
    }

    size_t sizeMaterial = 0;
    for (int idxMaterial = 0; idxMaterial < numMaterial; ++idxMaterial)
    {
        ResMaterial* pResMaterial = pRes->GetMaterial(idxMaterial);
        MaterialObj::InitArg argMaterial(pResMaterial);

        size_t size = MaterialObj::CalcBufferSize(argMaterial);
        sizeMaterial += Align(size, MaterialObj::BUFFER_ALIGNMENT);
    }

    int idx = 0;
    chunk[idx++].size = sizeSkeleton;
    chunk[idx++].size = sizeShape;
    chunk[idx++].size = sizeMaterial;
    chunk[idx++].size = sizeof(SkeletonObj);
    chunk[idx++].size = sizeof(ShapeObj) * numShape;
    chunk[idx++].size = sizeof(MaterialObj) * numMaterial;
    chunk[idx++].size = Align(numBone, 32) >> 3;
    chunk[idx++].size = Align(numMaterial, 32) >> 3;
    chunk[idx++].size = arg.IsBoundingEnabled() ? sizeof(Sphere) : 0;
    NW_G3D_ASSERT(idx == NUM_CHUNK);

    CalcOffset(chunk, NUM_CHUNK);
}

size_t ModelObj::CalcBufferSize(const InitArg& arg)
{
    Sizer& sizer = arg.GetSizer();
    sizer.Calc(arg);
    return sizer.GetTotalSize();
}

bool ModelObj::Init(const InitArg& arg, void* pBuffer, size_t bufferSize)
{
    NW_G3D_ASSERT_NOT_NULL(pBuffer);
#if NW_G3D_IS_HOST_CAFE
    NW_G3D_WARNING(IsAligned(pBuffer, BUFFER_ALIGNMENT), "pBuffer must be aligned.");
#endif // NW_G3D_IS_HOST_CAFE

    Sizer& sizer = arg.GetSizer();
    if (!sizer.IsValid())
    {
        sizer.Calc(arg);
    }
    if (sizer.GetTotalSize() > bufferSize)
    {
        return false;
    }

    ResModel* pRes = arg.GetResource();
    ResSkeleton* pResSkeleton = pRes->GetSkeleton();
    int numShape = pRes->GetShapeCount();
    int numMaterial = pRes->GetMaterialCount();
    int numView = arg.GetViewCount();

    void* ptr = pBuffer;
    m_pRes = pRes;
    m_pBoneVisArray = sizer.GetPtr<bit32>(ptr, Sizer::BONE_VIS_ARRAY);
    m_pMatVisArray = sizer.GetPtr<bit32>(ptr, Sizer::MAT_VIS_ARRAY);
    m_NumView = static_cast<u8>(numView);
    m_Flag = 0;
    m_NumShape = static_cast<u16>(numShape);
    m_NumMaterial = static_cast<u16>(numMaterial);
    m_pSkeleton = NULL;
    m_pShapeArray = sizer.GetPtr<ShapeObj>(ptr, Sizer::SHAPE_ARRAY);
    m_pMaterialArray = sizer.GetPtr<MaterialObj>(ptr, Sizer::MATERIAL_ARRAY);
    m_pBounding = sizer.GetPtr<Sphere>(ptr, Sizer::BOUNDING);
    m_pUserPtr = NULL;
    m_pBufferPtr = pBuffer;
    m_pBlockBuffer = NULL;

    SkeletonObj* pSkeleton = arg.GetSkeleton();
    if (pSkeleton)
    {
        m_pSkeleton = pSkeleton;
    }
    else
    {
        SkeletonObj::InitArg argSkeleton(pResSkeleton);
        argSkeleton.BufferingCount(arg.GetSkeletonBufferingCount());
        size_t size = SkeletonObj::CalcBufferSize(argSkeleton);

        m_pSkeleton = new(sizer.GetPtr(ptr, Sizer::SKELETON)) SkeletonObj();
        m_pSkeleton->Init(argSkeleton, sizer.GetPtr(ptr, Sizer::SKELETON_BUFFER), size);
    }

    void* pShapeBuffer = sizer.GetPtr<void>(ptr, Sizer::SHAPE_BUFFER);
    int viewDependent = 0;
    for (int idxShape = 0; idxShape < numShape; ++idxShape)
    {
        ResShape* pResShape = pRes->GetShape(idxShape);

        ShapeObj::InitArg argShape(pResShape);
        argShape.BufferingCount(arg.GetShapeBufferingCount());
        argShape.ViewCount(numView);
        ResBone* pResBone = m_pSkeleton->GetBone(pResShape->GetBoneIndex());
        if (pResBone->GetBillboardMode() == ResBone::BILLBOARD_NONE)
        {
            argShape.ViewIndependent();
        }
        else
        {
            argShape.ViewDependent();
            viewDependent = 1;
        }
        if (arg.IsBoundingEnabled())
        {
            argShape.EnableBounding();
        }
        else
        {
            argShape.DisableBounding();
        }

        size_t size = ShapeObj::CalcBufferSize(argShape);
        size = Align(size, ShapeObj::BUFFER_ALIGNMENT);

        ShapeObj* pShape = new(&m_pShapeArray[idxShape]) ShapeObj();
        pShape->Init(argShape, pShapeBuffer, size);

        pShapeBuffer = AddOffset(pShapeBuffer, size);
    }
    m_ViewDependent = static_cast<u8>(viewDependent);

    void* pMaterialBuffer = sizer.GetPtr<void>(ptr, Sizer::MATERIAL_BUFFER);
    for (int idxMaterial = 0; idxMaterial < numMaterial; ++idxMaterial)
    {
        ResMaterial* pResMaterial = pRes->GetMaterial(idxMaterial);

        MaterialObj::InitArg argMaterial(pResMaterial);

        size_t size = MaterialObj::CalcBufferSize(argMaterial);
        size = Align(size, MaterialObj::BUFFER_ALIGNMENT);

        MaterialObj* pMaterial = new(&m_pMaterialArray[idxMaterial]) MaterialObj();
        pMaterial->Init(argMaterial, pMaterialBuffer, size);

        pMaterialBuffer = AddOffset(pMaterialBuffer, size);
    }

    ClearBoneVisibility();
    ClearMatVisibility();

    if (m_pBounding)
    {
        memset(m_pBounding, 0, sizeof(*m_pBounding));
    }

    return true;
}

size_t ModelObj::CalcBlockBufferSize()
{
    size_t size = 0;

    if (m_pSkeleton && !m_pSkeleton->IsBlockBufferValid())
    {
        size += Align(m_pSkeleton->CalcBlockBufferSize(), SkeletonObj::BLOCK_BUFFER_ALIGNMENT);
    }

    for (int idxShape = 0; idxShape < m_NumShape; ++idxShape)
    {
        ShapeObj& shape = m_pShapeArray[idxShape];
        if (!shape.IsBlockBufferValid())
        {
            size += Align(shape.CalcBlockBufferSize(), ShapeObj::BLOCK_BUFFER_ALIGNMENT);
        }
    }

    for (int idxMaterial = 0; idxMaterial < m_NumMaterial; ++idxMaterial)
    {
        MaterialObj& material = m_pMaterialArray[idxMaterial];
        if (!material.IsBlockBufferValid())
        {
            size += Align(material.CalcBlockBufferSize(), MaterialObj::BLOCK_BUFFER_ALIGNMENT);
        }
    }

    return size;
}

bool ModelObj::SetupBlockBuffer(void* pBuffer, size_t bufferSize)
{
    NW_G3D_ASSERT(bufferSize == 0 || pBuffer);
#if NW_G3D_IS_HOST_CAFE
    NW_G3D_ASSERT_ADDR_ALIGNMENT(pBuffer, BLOCK_BUFFER_ALIGNMENT);
#endif // NW_G3D_IS_HOST_CAFE
    NW_G3D_ASSERT((m_Flag & BLOCK_BUFFER_VALID) == 0);

    if (CalcBlockBufferSize() > bufferSize)
    {
        return false;
    }

    m_pBlockBuffer = pBuffer;

    bool success = true;

    if (m_pSkeleton && !m_pSkeleton->IsBlockBufferValid())
    {
        size_t size = m_pSkeleton->CalcBlockBufferSize();
        size_t sizeAligned = Align(size, SkeletonObj::BLOCK_BUFFER_ALIGNMENT);
        success = m_pSkeleton->SetupBlockBuffer(pBuffer, size) && success;
        pBuffer = AddOffset(pBuffer, sizeAligned);
    }

    for (int idxShape = 0; idxShape < m_NumShape; ++idxShape)
    {
        ShapeObj& shape = m_pShapeArray[idxShape];
        if (!shape.IsBlockBufferValid())
        {
            size_t size = shape.CalcBlockBufferSize();
            size_t sizeAligned = Align(size, ShapeObj::BLOCK_BUFFER_ALIGNMENT);
            success = shape.SetupBlockBuffer(pBuffer, size) && success;
            pBuffer = AddOffset(pBuffer, sizeAligned);
        }
    }

    for (int idxMaterial = 0; idxMaterial < m_NumMaterial; ++idxMaterial)
    {
        MaterialObj& material = m_pMaterialArray[idxMaterial];
        if (!material.IsBlockBufferValid())
        {
            size_t size = material.CalcBlockBufferSize();
            size_t sizeAligned = Align(size, MaterialObj::BLOCK_BUFFER_ALIGNMENT);
            success = material.SetupBlockBuffer(pBuffer, size) && success;
            pBuffer = AddOffset(pBuffer, sizeAligned);
        }
    }

    if (success)
    {
        m_Flag |= BLOCK_BUFFER_VALID;
    }

    return success;
}

void ModelObj::CalcWorld(const Mtx34& baseMtx)
{
    m_pSkeleton->CalcWorldMtx(baseMtx);
}

void ModelObj::CalcBounding()
{
    if (m_pBounding == NULL)
    {
        return;
    }
    Sphere& bounding = *m_pBounding;
    if (GetShapeCount() >= 1)
    {
        ShapeObj& shape = m_pShapeArray[0];
        shape.CalcBounding(m_pSkeleton);
        bounding = *shape.GetBounding();
    }
    for (int idxShape = 1, numShape = GetShapeCount(); idxShape < numShape; ++idxShape)
    {
        ShapeObj& shape = m_pShapeArray[idxShape];
        shape.CalcBounding(m_pSkeleton);
        bounding.Merge(bounding, *shape.GetBounding());
    }
}

void ModelObj::CalcMtxBlock(int bufferIndex /*= 0*/)
{
    m_pSkeleton->CalcMtxBlock(bufferIndex);
}

void ModelObj::CalcView(int viewIndex, const Mtx34& cameraMtx, int bufferIndex /*= 0*/)
{
    NW_G3D_ASSERT_INDEX_BOUNDS(viewIndex, m_NumView);

    if (!IsViewDependent())
    {
        return;
    }

    int lastBoneIndex = -1;
    Mtx34 bbMtx;

    const Mtx34* pWorldMtxArray = m_pSkeleton->GetWorldMtxArray();
    for (int idxShape = 0; idxShape < m_NumShape; ++idxShape)
    {
        ShapeObj& shape = m_pShapeArray[idxShape];
        if (shape.IsRigidBody() && shape.IsViewDependent())
        {
            int idxBone = shape.GetBoneIndex();
            const ResBone* pBone = m_pSkeleton->GetBone(idxBone);

            bit32 bbMode = pBone->GetBillboardMode();
            NW_G3D_ASSERT(bbMode <= ResBone::BILLBOARD_MAX);
            if (bbMode == ResBone::BILLBOARD_NONE)
            {
                shape.CalcShpBlock(viewIndex, pWorldMtxArray[idxBone], bufferIndex);
                continue;
            }

            int bbBoneIndex = pBone->ref().billboardIndex;
            if (bbBoneIndex == ResBone::BB_INDEX_NONE)
            {
                if (idxBone != lastBoneIndex)
                {
                    m_pSkeleton->CalcBillboardMtx(&bbMtx, cameraMtx, idxBone, true);
                    lastBoneIndex = idxBone;
                }
                shape.CalcShpBlock(viewIndex, bbMtx, bufferIndex);
                continue;
            }

            if (bbBoneIndex != lastBoneIndex)
            {
                m_pSkeleton->CalcBillboardMtx(&bbMtx, cameraMtx, bbBoneIndex, false);
                lastBoneIndex = bbBoneIndex;
            }

            Mtx34 worldMtx;
            worldMtx.Mul(bbMtx, pWorldMtxArray[idxBone]);
            shape.CalcShpBlock(viewIndex, worldMtx, bufferIndex);
        }
    }
}

void ModelObj::CalcShape(int bufferIndex /*= 0*/)
{
    const Mtx34* pWorldMtxArray = m_pSkeleton->GetWorldMtxArray();
    for (int idxShape = 0; idxShape < m_NumShape; ++idxShape)
    {
        ShapeObj& shape = m_pShapeArray[idxShape];
        if (shape.IsRigidBody() && !shape.IsViewDependent())
        {
            int idxBone = shape.GetBoneIndex();
            shape.CalcShpBlock(0, pWorldMtxArray[idxBone], bufferIndex);
        }
    }
}

void ModelObj::CalcMaterial()
{
    for (int idxMaterial = 0; idxMaterial < m_NumMaterial; ++idxMaterial)
    {
        MaterialObj& material = m_pMaterialArray[idxMaterial];
        material.CalcMatBlock();
    }
}

void ModelObj::ClearBoneVisibility()
{
    for (int idxBone = 0, numBone = m_pSkeleton->GetBoneCount(); idxBone < numBone; ++idxBone)
    {
        ResBone* pBone = m_pSkeleton->GetBone(idxBone);
        SetBit(m_pBoneVisArray, idxBone, pBone->ref().flag & ResBone::VISIBILITY);
    }
}

void ModelObj::ClearMatVisibility()
{
    for (int idxMat = 0, numMat = GetMaterialCount(); idxMat < numMat; ++idxMat)
    {
        ResMaterial* pResMaterial = m_pRes->GetMaterial(idxMat);
        SetBit(m_pMatVisArray, idxMat, pResMaterial->ref().flag & ResMaterial::VISIBILITY);
    }
}

} } // namespace nw::g3d
