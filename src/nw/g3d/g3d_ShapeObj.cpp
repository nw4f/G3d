#include <nw/g3d/g3d_ShapeObj.h>
#include <nw/g3d/math/g3d_Vector2.h>
#include <nw/g3d/math/g3d_Matrix34.h>
#include <nw/g3d/fnd/g3d_GfxManage.h>
#include <nw/g3d/g3d_SkeletonObj.h>

namespace nw { namespace g3d {

void Sphere::Transform(const Sphere& sphere, const Mtx34& mtx)
{
    this->center.Mul(mtx, sphere.center);
    float scaleX = Mtx34::ExtractBaseScaleSq(mtx, 0);
    float scaleY = Mtx34::ExtractBaseScaleSq(mtx, 1);
    float scaleZ = Mtx34::ExtractBaseScaleSq(mtx, 2);
    float scale = Math::Sqrt(Math::Max(Math::Max(scaleX, scaleY), scaleZ));
    this->radius = sphere.radius * scale;
}

void Sphere::Merge(const Sphere& lhs, const Sphere& rhs)
{
    Vec3 diff;
    diff.Sub(rhs.center, lhs.center);
    float distSq = Vec3::LengthSq(diff);
    float radiusDiff = rhs.radius - lhs.radius;
    float epsilon = Math::Max(radiusDiff * radiusDiff, 0.0001f);
    if (distSq < epsilon)
    {
        *this = lhs.radius > rhs.radius ? lhs : rhs;
    }
    else
    {
        float dist = distSq * Math::RSqrt(distSq);
        float newRadius = 0.5f * (lhs.radius + rhs.radius + dist);
        this->center.Add(lhs.center, diff.Mul(diff, (newRadius - lhs.radius) / dist));
        this->radius = newRadius;
    }
}

void AABB::Set(const Vec3* pPointArray, int count)
{
    Vec3 minP, maxP;
    minP = maxP = pPointArray[0];
    for (int idxPoint = 0; idxPoint < count; ++idxPoint)
    {
        minP.x = Math::Min(minP.x, pPointArray[idxPoint].x);
        maxP.x = Math::Max(maxP.x, pPointArray[idxPoint].x);
        minP.y = Math::Min(minP.y, pPointArray[idxPoint].y);
        maxP.y = Math::Max(maxP.y, pPointArray[idxPoint].y);
        minP.z = Math::Min(minP.z, pPointArray[idxPoint].z);
        maxP.z = Math::Max(maxP.z, pPointArray[idxPoint].z);
    }
    min = minP;
    max = maxP;
}

void AABB::Transform(const Bounding& aabb, const Mtx34& mtx)
{
    Vec3 center, extent;

    center.Mul(mtx, aabb.center);
    Vec3 axis;
    axis.Set(Math::Abs(mtx.m00), Math::Abs(mtx.m10), Math::Abs(mtx.m20));
    extent.Mul(axis, aabb.extent.x);
    axis.Set(Math::Abs(mtx.m01), Math::Abs(mtx.m11), Math::Abs(mtx.m21));
    extent.Mad(axis, aabb.extent.y, extent);
    axis.Set(Math::Abs(mtx.m02), Math::Abs(mtx.m12), Math::Abs(mtx.m22));
    extent.Mad(axis, aabb.extent.z, extent);

    this->min.Sub(center, extent);
    this->max.Add(center, extent);
}

void AABB::Merge(const AABB& lhs, const AABB& rhs)
{
#if defined( __ghs__ )
    this->min.ps[0] = __PS_SEL(__PS_SUB(rhs.min.ps[0], lhs.min.ps[0]), lhs.min.ps[0], rhs.min.ps[0]);
    this->max.ps[0] = __PS_SEL(__PS_SUB(lhs.max.ps[0], rhs.max.ps[0]), lhs.max.ps[0], rhs.max.ps[0]);
#else
    this->min.x = Math::Min(lhs.min.x, rhs.min.x);
    this->max.x = Math::Max(lhs.max.x, rhs.max.x);
    this->min.y = Math::Min(lhs.min.y, rhs.min.y);
    this->max.y = Math::Max(lhs.max.y, rhs.max.y);
#endif
    this->min.z = Math::Min(lhs.min.z, rhs.min.z);
    this->max.z = Math::Max(lhs.max.z, rhs.max.z);
}

void Plane::Set(const Vec3& p0, const Vec3& p1, const Vec3& p2)
{
    Vec3 v0, v1, v2;
    v0.Sub(p2, p0);
    v1.Sub(p1, p0);
    normal.Normalize(v2.Cross(v0, v1));
    dist = -Vec3::Dot(normal, p0);
}

void ViewVolume::SetFrustum(
    float top, float bottom, float left, float right, float zNear, float zFar,
    const Mtx34& viewToWorld)
{
    float nearToFar = zFar * Math::Rcp(zNear);
    float topFar = top * nearToFar;
    float bottomFar = bottom * nearToFar;
    float leftFar = left * nearToFar;
    float rightFar = right * nearToFar;

    Vec3 pt[] = {
        Vec3::Make(left, top, -zNear),
        Vec3::Make(right, top, -zNear),
        Vec3::Make(right, bottom, -zNear),
        Vec3::Make(left, bottom, -zNear),
        Vec3::Make(leftFar, topFar, -zFar),
        Vec3::Make(rightFar, topFar, -zFar),
        Vec3::Make(rightFar, bottomFar, -zFar),
        Vec3::Make(leftFar, bottomFar, -zFar),
    };

    for (int i = 0; i < 8; ++i)
    {
        pt[i].Mul(viewToWorld, pt[i]);
    }
    aabb.Set(pt, 8);

    Vec3 eye;
    eye.Set(viewToWorld.m03, viewToWorld.m13, viewToWorld.m23);
    planes[0].Set(eye, pt[3], pt[0]);
    planes[1].Set(eye, pt[1], pt[2]);
    planes[2].Set(pt[0], pt[1], pt[2]);
    planes[3].Set(pt[4], pt[7], pt[6]);
    planes[4].Set(eye, pt[0], pt[1]);
    planes[5].Set(eye, pt[2], pt[3]);
    numPlanes = 6;
    flag = 0;
}

bool ViewVolume::TestIntersection(const Sphere& sphere) const
{
    for (int idxPlane = 0; idxPlane < this->numPlanes; ++idxPlane)
    {
        const Plane& plane = this->planes[idxPlane];
        float dist = Vec3::Dot(plane.normal, sphere.center) + plane.dist;
        if (dist > sphere.radius)
        {
            return false;
        }
    }
    return true;
}

int ViewVolume::TestIntersectionEx(const Sphere& sphere) const
{
    int result = 1;
    for (int idxPlane = 0; idxPlane < this->numPlanes; ++idxPlane)
    {
        const Plane& plane = this->planes[idxPlane];
        float dist = Vec3::Dot(plane.normal, sphere.center) + plane.dist;
        if (dist > sphere.radius)
        {
            return -1;
        }
        if (dist >= -sphere.radius)
        {
            result = 0;
        }
    }
    return result;
}

bool ViewVolume::TestIntersection(const AABB& aabb) const
{
    if (this->flag)
    {
        if (this->aabb.min.x > aabb.max.x || aabb.min.x > this->aabb.max.x ||
            this->aabb.min.y > aabb.max.y || aabb.min.y > this->aabb.max.y ||
            this->aabb.min.z > aabb.max.z || aabb.min.z > this->aabb.max.z)
        {
            return false;
        }
    }
    for (int idxPlane = 0; idxPlane < this->numPlanes; ++idxPlane)
    {
        const Plane& plane = this->planes[idxPlane];
        Vec3 pos;
#if defined( __ghs__ )
        pos.ps[0] = __PS_SEL(plane.normal.ps[0], aabb.min.ps[0], aabb.max.ps[0]);
#else
        pos.x = Math::Select(plane.normal.x, aabb.min.x, aabb.max.x);
        pos.y = Math::Select(plane.normal.y, aabb.min.y, aabb.max.y);
#endif
        pos.z = Math::Select(plane.normal.z, aabb.min.z, aabb.max.z);

        if (Vec3::Dot(plane.normal, pos) + plane.dist > 0.0f)
        {
            return false;
        }
    }
    return true;
}

int ViewVolume::TestIntersectionEx(const AABB& aabb) const
{
    if (this->flag)
    {
        if (this->aabb.min.x > aabb.max.x || aabb.min.x > this->aabb.max.x ||
            this->aabb.min.y > aabb.max.y || aabb.min.y > this->aabb.max.y ||
            this->aabb.min.z > aabb.max.z || aabb.min.z > this->aabb.max.z)
        {
            return -1;
        }
    }
    int result = 1;
    for (int idxPlane = 0; idxPlane < this->numPlanes; ++idxPlane)
    {
        const Plane& plane = this->planes[idxPlane];
        Vec3 pos, neg;
#if defined( __ghs__ )
        pos.ps[0] = __PS_SEL(plane.normal.ps[0], aabb.min.ps[0], aabb.max.ps[0]);
        neg.ps[0] = __PS_SEL(plane.normal.ps[0], aabb.max.ps[0], aabb.min.ps[0]);
#else
        pos.x = Math::Select(plane.normal.x, aabb.min.x, aabb.max.x);
        neg.x = Math::Select(plane.normal.x, aabb.max.x, aabb.min.x);
        pos.y = Math::Select(plane.normal.y, aabb.min.y, aabb.max.y);
        neg.y = Math::Select(plane.normal.y, aabb.max.y, aabb.min.y);
#endif
        pos.z = Math::Select(plane.normal.z, aabb.min.z, aabb.max.z);
        neg.z = Math::Select(plane.normal.z, aabb.max.z, aabb.min.z);

        if (Vec3::Dot(plane.normal, pos) + plane.dist > 0.0f)
        {
            return -1;
        }
        if (result && Vec3::Dot(plane.normal, neg) + plane.dist >= 0.0f)
        {
            result = 0;
        }
    }
    return result;
}

void ShapeObj::Sizer::Calc(const InitArg& arg)
{
    NW_G3D_ASSERT(arg.GetBufferingCount() > 0);
    ResShape* pRes = arg.GetResource();
    bool boundingEnabled = arg.IsBoundingEnabled();
    size_t subboundingSize = boundingEnabled ?
        Align(sizeof(AABB) * pRes->GetBoundingNodeCount(), CACHE_BLOCK_SIZE) : 0;
    int numView = arg.GetViewCount();
    int numShpBlock = arg.IsViewDependent() ? numView : 1;
    int numKeyShape = pRes->GetKeyShapeCount();

    int idx = 0;
    chunk[idx++].size = subboundingSize;
    chunk[idx++].size = sizeof(GfxBuffer) * numShpBlock;
    chunk[idx++].size = sizeof(float) * numKeyShape;

    chunk[idx++].size = boundingEnabled ? sizeof(Sphere) * NUM_COORD : 0;

    NW_G3D_ASSERT(idx == NUM_CHUNK);

    CalcOffset(chunk, NUM_CHUNK);
}

size_t ShapeObj::CalcBufferSize(const InitArg& arg)
{
    Sizer& sizer = arg.GetSizer();
    sizer.Calc(arg);
    return sizer.GetTotalSize();
}

bool ShapeObj::Init(const InitArg& arg, void* pBuffer, size_t bufferSize)
{
    NW_G3D_ASSERT_NOT_NULL(pBuffer);
    NW_G3D_WARNING(IsAligned(pBuffer, BUFFER_ALIGNMENT), "pBuffer must be aligned.");

    Sizer& sizer = arg.GetSizer();
    if (!sizer.IsValid())
    {
        sizer.Calc(arg);
    }
    if (sizer.GetTotalSize() > bufferSize)
    {
        return false;
    }

    ResShape* pRes = arg.GetResource();
    int numView = arg.GetViewCount();

    void* ptr = pBuffer;
    m_pRes = pRes;
    m_Flag = BLOCK_BUFFER_SWAP;
    m_NumView = static_cast<u8>(numView);
    m_ViewDependent = arg.IsViewDependent() ? 1 : 0;
    m_NumShpBlock = arg.IsViewDependent() ? m_NumView : 1;
    m_NumBuffering = static_cast<u8>(arg.GetBufferingCount());
    m_pShpBlockArray = sizer.GetPtr<GfxBuffer>(ptr, Sizer::SHAPE_BLOCK_ARRAY);
    m_pBlendWeightArray = sizer.GetPtr<float>(ptr, Sizer::BLEND_WEIGHT_ARRAY);

    m_pBounding = sizer.GetPtr<Sphere>(ptr, Sizer::BOUNDING_ARRAY);
    m_pSubBounding = sizer.GetPtr<AABB>(ptr, Sizer::SUBBOUNDING_ARRAY);

    m_pUserPtr = NULL;
    m_pBufferPtr = pBuffer;
    m_pBlockBuffer = NULL;

    if (m_pBounding)
    {
        Sphere& sphere = m_pBounding[LOCAL_COORD];
        const Bounding& bounding = m_pRes->GetBounding();
        sphere.center = bounding.center;
        sphere.radius = m_pRes->GetRadius();
        m_pBounding[WORLD_COORD] = sphere;
    }

    for (int idxShpBlock = 0; idxShpBlock < m_NumShpBlock; ++idxShpBlock)
    {
        new(&m_pShpBlockArray[idxShpBlock]) GfxBuffer();
    }

    ClearBlendWeights();

    return true;
}

size_t ShapeObj::CalcBlockBufferSize() const
{
    return sizeof(ShpBlock) * GetShpBlockCount() * m_NumBuffering;
}

bool ShapeObj::SetupBlockBuffer(void* pBuffer, size_t bufferSize)
{
    NW_G3D_ASSERT_NOT_NULL(pBuffer);
#if NW_G3D_IS_HOST_CAFE
    NW_G3D_ASSERT_ADDR_ALIGNMENT(pBuffer, BLOCK_BUFFER_ALIGNMENT);
#endif // NW_G3D_IS_HOST_CAFE
    NW_G3D_ASSERT((m_Flag & BLOCK_BUFFER_VALID) == 0);

    size_t size = CalcBlockBufferSize();

    if (size > bufferSize)
    {
        return false;
    }

    m_pBlockBuffer = pBuffer;

    ShpBlock* pShpBufferArray = static_cast<ShpBlock*>(pBuffer);
    Mtx34 worldMtx;
    worldMtx.Identity();
    s32 vtxSkinCount = GetVtxSkinCount();

    for (int idxShpBlock = 0, numShpBlock = GetShpBlockCount();
        idxShpBlock < numShpBlock; ++idxShpBlock)
    {
        GfxBuffer& shpBlock = m_pShpBlockArray[idxShpBlock];
        shpBlock.SetData(&pShpBufferArray[idxShpBlock * m_NumBuffering],
            sizeof(ShpBlock), m_NumBuffering);
        shpBlock.Setup();
        for (int idxBuffer = 0; idxBuffer < m_NumBuffering; ++idxBuffer)
        {
            ShpBlock* pShpBuffer = static_cast<ShpBlock*>(shpBlock.GetData(idxBuffer));
            if (IsBlockSwapEnabled())
            {
                Copy32<true>(&pShpBuffer->worldMtx, &worldMtx, sizeof(Mtx34) >> 2);
                Copy32<true>(&pShpBuffer->vtxSkinCount, &vtxSkinCount, sizeof(s32) >> 2);
            }
            else
            {
                Copy32<false>(&pShpBuffer->worldMtx, &worldMtx, sizeof(Mtx34) >> 2);
                Copy32<false>(&pShpBuffer->vtxSkinCount, &vtxSkinCount, sizeof(s32) >> 2);
            }
            shpBlock.DCFlush(idxBuffer);
        }
    }
    m_Flag |= BLOCK_BUFFER_VALID;

    return true;
}

void ShapeObj::CalcBounding(const SkeletonObj* pSkeleton)
{
    NW_G3D_ASSERT_NOT_NULL(pSkeleton);
    if (m_pBounding == NULL)
    {
        return;
    }
    const Mtx34* pWorldMtxArray = pSkeleton->GetWorldMtxArray();
    if (IsRigidBody())
    {
        const Mtx34& worldMtx = pWorldMtxArray[GetBoneIndex()];
        m_pBounding[WORLD_COORD].Transform(m_pBounding[LOCAL_COORD], worldMtx);
    }
    else if (IsRigidSkinning())
    {
        const u16* pSkinBoneIndexArray = m_pRes->GetSkinBoneIndexArray();
        {
            int idxBone = pSkinBoneIndexArray[0];
            const Mtx34& worldMtx = pWorldMtxArray[idxBone];
            m_pBounding[WORLD_COORD].Transform(m_pBounding[LOCAL_COORD], worldMtx);
        }
        for (int idxIndex = 1, numIndex = m_pRes->GetSkinBoneIndexCount();
            idxIndex < numIndex; ++idxIndex)
        {
            Sphere sphere;
            int idxBone = pSkinBoneIndexArray[idxIndex];
            const Mtx34& worldMtx = pWorldMtxArray[idxBone];
            sphere.Transform(m_pBounding[LOCAL_COORD], worldMtx);
            m_pBounding[WORLD_COORD].Merge(m_pBounding[WORLD_COORD], sphere);
        }
    }
    else
    {
        const u16* pSkinBoneIndexArray = m_pRes->GetSkinBoneIndexArray();
        {
            int idxBone = pSkinBoneIndexArray[0];
            {
                const Mtx34& worldMtx = pSkeleton->GetBone(idxBone)->GetInvModelMtx();
                m_pBounding[WORLD_COORD].Transform(m_pBounding[LOCAL_COORD], worldMtx);
            }
            {
                const Mtx34& worldMtx = pWorldMtxArray[idxBone];
                m_pBounding[WORLD_COORD].Transform(m_pBounding[WORLD_COORD], worldMtx);
            }
        }
        for (int idxIndex = 1, numIndex = m_pRes->GetSkinBoneIndexCount();
            idxIndex < numIndex; ++idxIndex)
        {
            Sphere sphere;
            int idxBone = pSkinBoneIndexArray[idxIndex];
            {
                const Mtx34& worldMtx = pSkeleton->GetBone(idxBone)->GetInvModelMtx();
                sphere.Transform(m_pBounding[LOCAL_COORD], worldMtx);
            }
            {
                const Mtx34& worldMtx = pWorldMtxArray[idxBone];
                sphere.Transform(sphere, worldMtx);
            }
            m_pBounding[WORLD_COORD].Merge(m_pBounding[WORLD_COORD], sphere);
        }
    }
}

void ShapeObj::CalcSubBounding(const SkeletonObj* pSkeleton)
{
    NW_G3D_ASSERT_NOT_NULL(pSkeleton);
    if (m_pSubBounding == NULL || !IsRigidBody())
    {
        return;
    }
    const Mtx34* pWorldMtxArray = pSkeleton->GetWorldMtxArray();
    const Mtx34& worldMtx = pWorldMtxArray[GetBoneIndex()];
    const Bounding* pBoundingArray = m_pRes->ref().ofsSubBoundingArray.to_ptr<Bounding>();
#if NW_G3D_IS_HOST_CAFE
    size_t bufferSize = Align(sizeof(AABB) * m_pRes->GetBoundingNodeCount(), CACHE_BLOCK_SIZE);
    DCZeroRange(m_pSubBounding, bufferSize);
#endif
    const BoundingNode* pBoundingNodeArray = m_pRes->GetBoundingNodeArray();
    for (int idxBoundingNode = m_pRes->GetBoundingNodeCount() - 1;
        idxBoundingNode >= 0; --idxBoundingNode)
    {
        const BoundingNode& node = pBoundingNodeArray[idxBoundingNode];
        if (node.idxLeft == idxBoundingNode)
        {
            const Bounding& aabb = pBoundingArray[idxBoundingNode];
            m_pSubBounding[idxBoundingNode].Transform(aabb, worldMtx);
        }
        else
        {
            m_pSubBounding[idxBoundingNode].Merge(m_pSubBounding[node.idxLeft],
                                                  m_pSubBounding[node.idxRight]);
        }
    }
}

void ShapeObj::CalcShpBlock(int viewIndex, const Mtx34& worldMtx, int bufferIndex /*= 0*/)
{
    GfxBuffer& shpBlock = GetShpBlock(viewIndex);
    ShpBlock* pShpBuffer = static_cast<ShpBlock*>(shpBlock.GetData(bufferIndex));
    s32 vtxSkinCount = GetVtxSkinCount();
    const size_t updateSize = 0x100;
    if (CPUCache::IsValid(pShpBuffer, updateSize))
    {
        CPUCache::FillZero(pShpBuffer, updateSize);
    }
    if (IsBlockSwapEnabled())
    {
        Copy32<true>(&pShpBuffer->worldMtx, &worldMtx, sizeof(Mtx34) >> 2);
        Copy32<true>(&pShpBuffer->vtxSkinCount, &vtxSkinCount, sizeof(s32) >> 2);

    }
    else
    {
        Copy32<false>(&pShpBuffer->worldMtx, &worldMtx, sizeof(Mtx34) >> 2);
        Copy32<false>(&pShpBuffer->vtxSkinCount, &vtxSkinCount, sizeof(s32) >> 2);

    }
    shpBlock.DCFlush(bufferIndex);
}

void ShapeObj::ClearBlendWeights()
{
    int numKeyShape = GetResource()->GetKeyShapeCount();
    for (int idxKeyShape = 0; idxKeyShape < numKeyShape; ++idxKeyShape)
    {
        m_pBlendWeightArray[idxKeyShape] = 0.0f;
    }

    m_Flag &= ~BLEND_WEIGHT_VALID;
}

bool ShapeObj::TestSubMeshIntersection(CullingContext* pCtx, const ViewVolume& volume) const
{
    NW_G3D_ASSERT_NOT_NULL(pCtx);
    NW_G3D_ASSERT_NOT_NULL(m_pSubBounding);

    // This function is the definition of pain >.>

    if (pCtx->nodeIndex >= m_pRes->GetBoundingNodeCount())
    {
        return false;
    }

    int idxInnerBound = pCtx->nodeIndex;
    pCtx->submeshIndex = 0;
    pCtx->submeshCount = 0;

    const BoundingNode* pBoundingNodeArray = m_pRes->GetBoundingNodeArray();
    const BoundingNode* pNode;

    {
        bool found;
        while (true)
        {
            found = false;
            int currentNodeIndex = idxInnerBound;
            pNode = &pBoundingNodeArray[currentNodeIndex];
            int intersect = volume.TestIntersectionEx(m_pSubBounding[currentNodeIndex]);
            if (intersect > 0)
            {
                found = true;
            }
            else
            {
                if (intersect < 0)
                {
                    idxInnerBound = pNode->idxNext;
                }
                else if (pNode->idxPrev != idxInnerBound)
                {
                    idxInnerBound = pNode->idxPrev;
                }
                else
                {
                    found = true;
                }
            }
            if (found)
            {
                break;
            }

            if (idxInnerBound == currentNodeIndex)
            {
                break;
            }
        }
        if (!found)
        {
            pCtx->nodeIndex = m_pRes->GetBoundingNodeCount();
            return false;
        }
    }

    int idxOuterBound = pNode->idxNext;
    pCtx->submeshIndex = pNode->idxSubMesh;
    pCtx->submeshCount = pNode->numSubMesh;

    if (idxOuterBound != idxInnerBound)
    {
        bool found;
        while (true)
        {
            found = false;
            int currentNodeIndex = idxOuterBound;
            pNode = &pBoundingNodeArray[currentNodeIndex];
            int intersect = volume.TestIntersectionEx(m_pSubBounding[currentNodeIndex]);
            if (intersect > 0)
            {
                found = true;
            }
            else
            {
                if (intersect < 0)
                {
                    pCtx->nodeIndex = pNode->idxNext;
                    return true;
                }
                else if (pNode->idxPrev != idxOuterBound)
                {
                    idxOuterBound = pNode->idxPrev;
                }
                else
                {
                    found = true;
                }
            }
            if (found)
            {
                idxOuterBound = pNode->idxNext;
                pCtx->submeshCount = pNode->numSubMesh + pNode->idxSubMesh - pCtx->submeshIndex;
            }

            if (idxOuterBound == currentNodeIndex)
            {
                break;
            }
        }
    }

    pCtx->nodeIndex = m_pRes->GetBoundingNodeCount();
    return true;
}

} } // namespace nw::g3d
