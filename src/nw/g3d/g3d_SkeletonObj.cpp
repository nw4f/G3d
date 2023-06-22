#include <nw/g3d/g3d_SkeletonObj.h>
#include <nw/g3d/ut/g3d_Flag.h>

#include <nw/g3d/fnd/g3d_GfxManage.h>
#include <nw/g3d/g3d_Billboard.h>

namespace nw { namespace g3d {

class SkeletonObj::Impl
{
public:
    static void (SkeletonObj::* const s_pFuncCalcWorld[])(const Mtx34&);
    static void (SkeletonObj::* const s_pFuncCalcWorldWithCallback[])(const Mtx34&);
};

class CalcWorldNoScale;
class CalcWorldStd;
class CalcWorldMaya;
class CalcWorldSoftimage;

void (SkeletonObj::* const SkeletonObj::Impl::s_pFuncCalcWorld[])(const Mtx34&) = {
    &SkeletonObj::CalcWorldImpl<CalcWorldNoScale, false>,
    &SkeletonObj::CalcWorldImpl<CalcWorldStd, false>,
    &SkeletonObj::CalcWorldImpl<CalcWorldMaya, false>,
    &SkeletonObj::CalcWorldImpl<CalcWorldSoftimage, false>
};

void (SkeletonObj::* const SkeletonObj::Impl::s_pFuncCalcWorldWithCallback[])(const Mtx34&) = {
    &SkeletonObj::CalcWorldImpl<CalcWorldNoScale, true>,
    &SkeletonObj::CalcWorldImpl<CalcWorldStd, true>,
    &SkeletonObj::CalcWorldImpl<CalcWorldMaya, true>,
    &SkeletonObj::CalcWorldImpl<CalcWorldSoftimage, true>
};

void SkeletonObj::Sizer::Calc(const InitArg& arg)
{
    NW_G3D_ASSERT(arg.GetBufferingCount() > 0);
    ResSkeleton* pRes = arg.GetResource();
    int numBone = pRes->GetBoneCount();
    int scaleMode = pRes->GetScaleMode();

    int idx = 0;
    chunk[idx++].size = Align(sizeof(Mtx34) * numBone, LL_CACHE_FETCH_SIZE);
    chunk[idx++].size = Align(sizeof(LocalMtx) * numBone, LL_CACHE_FETCH_SIZE);
    chunk[idx++].size = scaleMode == SCALE_SOFTIMAGE ? sizeof(Vec3) * numBone : 0;
    NW_G3D_ASSERT(idx == NUM_CHUNK);

    CalcOffset(chunk, NUM_CHUNK);
}

size_t SkeletonObj::CalcBufferSize(const InitArg& arg)
{
    Sizer& sizer = arg.GetSizer();
    sizer.Calc(arg);
    return sizer.GetTotalSize();
}

bool SkeletonObj::Init(const InitArg& arg, void* pBuffer, size_t bufferSize)
{
    NW_G3D_ASSERT(bufferSize == 0 || pBuffer);
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

    ResSkeleton* pRes = arg.GetResource();

    void* ptr = pBuffer;
    m_pRes = pRes;
    m_Flag = static_cast<bit16>(pRes->ref().flag |
        BLOCK_BUFFER_SWAP | CACHE_LOCAL_MTX | CACHE_WORLD_MTX);
    m_NumBuffering = static_cast<u8>(arg.GetBufferingCount());
    m_pBoneArray = pRes->ref().ofsBoneArray.to_ptr<ResBoneData>();
    m_pLocalMtxArray = sizer.GetPtr<LocalMtx>(ptr, Sizer::LOCAL_MTX);
    m_pMemLocalMtxArray = m_pLocalMtxArray;
    m_pWorldMtxArray = sizer.GetPtr<Mtx34>(ptr, Sizer::WORLD_MTX);
    m_pMemWorldMtxArray = m_pWorldMtxArray;
    m_pScaleArray = sizer.GetPtr<Vec3>(ptr, Sizer::SCALE);
    m_NumBone = static_cast<u16>(pRes->GetBoneCount());
    m_CallbackBone = ICalcWorldCallback::INVALID_BONE;
    m_pCallback = NULL;

    m_pUserPtr = NULL;
    m_pBufferPtr = pBuffer;
    m_pBlockBuffer = NULL;
    memset(static_cast<GfxBuffer_t*>(&m_MtxBlock), 0, sizeof(GfxBuffer_t));

    ClearLocalMtx();

    return true;
}

size_t SkeletonObj::CalcBlockBufferSize() const
{
    return Align(sizeof(Mtx34) * m_pRes->GetMtxCount(), BLOCK_BUFFER_ALIGNMENT) * m_NumBuffering;
}

bool SkeletonObj::SetupBlockBuffer(void* pBuffer, size_t bufferSize)
{
    NW_G3D_ASSERT(bufferSize == 0 || pBuffer);
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

    m_MtxBlock.SetData(pBuffer, size / m_NumBuffering, m_NumBuffering);
    m_MtxBlock.Setup();
#if NW_G3D_IS_HOST_CAFE
    DCInvalidateRange((m_pLCMtxBlock = m_MtxBlock.pData), size);
#endif

    m_Flag |= BLOCK_BUFFER_VALID;
    return true;
}

void SkeletonObj::ClearLocalMtx()
{
    LocalMtx* pLocalMtxArray = GetLocalMtxArray();
    if (ROT_QUAT != GetRotateMode())
    {
        int numBone = m_pRes->GetBoneCount();
        for (int idxBone = 0; idxBone < numBone; ++idxBone)
        {
            ResBone* pBone = m_pRes->GetBone(idxBone);
            LocalMtx& local = pLocalMtxArray[idxBone];
            local.flag = pBone->ref().flag & (ResBone::TRANSFORM_MASK | ResBone::BILLBOARD_MASK);
            local.scale.Set(pBone->GetScale());
            local.mtxRT.SetR(pBone->GetRotateEuler());
            local.mtxRT.SetT(pBone->GetTranslate());
        }
    }
    else
    {
        int numBone = m_pRes->GetBoneCount();
        for (int idxBone = 0; idxBone < numBone; ++idxBone)
        {
            ResBone* pBone = m_pRes->GetBone(idxBone);
            LocalMtx& local = pLocalMtxArray[idxBone];
            local.flag = pBone->ref().flag & (ResBone::TRANSFORM_MASK | ResBone::BILLBOARD_MASK);
            local.scale.Set(pBone->GetScale());
            local.mtxRT.SetR(pBone->GetRotateQuat());
            local.mtxRT.SetT(pBone->GetTranslate());
        }
    }
}

void SkeletonObj::CalcBillboardMtx(
    Mtx34* pMtx, const Mtx34& cameraMtx, int boneIndex, bool combineWorld) const
{
    NW_G3D_ASSERT(pMtx != NULL);

    const Mtx34* pWorldMtxArray = GetWorldMtxArray();
    bit32 bbMode = GetBone(boneIndex)->GetBillboardMode();
    const Mtx34& world = pWorldMtxArray[boneIndex];

    Mtx34 wv;
    wv.Mul(cameraMtx, world);
    Billboard::Calc(bbMode, &wv, cameraMtx, world, m_pLocalMtxArray[boneIndex].mtxRT);

    if ((bbMode & ~ResBone::BILLBOARD_CHILD) == ResBone::BILLBOARD_SCREEN_VIEWVECTOR)
    {
        const_cast<bit16&>(m_Flag) |= CACHE_LOCAL_MTX | CACHE_WORLD_MTX;
    }
    else
    {
        const_cast<bit16&>(m_Flag) |= CACHE_WORLD_MTX;
    }

    Mtx34 inverse;
    float det;

    inverse.Inverse(&det, cameraMtx);
    pMtx->Mul(inverse, wv);

    if (combineWorld)
    {
        return;
    }

    inverse.Inverse(&det, world);
    pMtx->Mul(*pMtx, inverse);
}

void SkeletonObj::CalcWorldMtx(const Mtx34& baseMtx)
{
    int funcIndex = (m_Flag & SCALE_MASK) >> (SCALE_SHIFT);
    NW_G3D_ASSERT_INDEX_BOUNDS(funcIndex, sizeof(Impl::s_pFuncCalcWorld) / sizeof(*Impl::s_pFuncCalcWorld));

    m_Flag |= CACHE_LOCAL_MTX | CACHE_WORLD_MTX;
#if NW_G3D_IS_HOST_CAFE
    if (m_pWorldMtxArray == m_pMemWorldMtxArray)
    {
        DCZeroRange(m_pWorldMtxArray, sizeof(Mtx34) * GetBoneCount());
    }
#endif
    if (m_pCallback)
    {
        (this->*Impl::s_pFuncCalcWorldWithCallback[funcIndex])(baseMtx);
    }
    else
    {
        (this->*Impl::s_pFuncCalcWorld[funcIndex])(baseMtx);
    }
}

void SkeletonObj::CalcMtxBlock(int bufferIndex /*= 0*/)
{
    GfxBuffer& block = GetMtxBlock();
    if (block.GetSize() == 0)
    {
        return;
    }

    m_Flag |= CACHE_WORLD_MTX;

    const s16* pMtxToBoneTable = m_pRes->ref().ofsMtxToBoneTable.to_ptr<s16>();

    int idxMtx = 0;
    Mtx34* pMtxArray = NULL;
    if (block.pData == m_pLCMtxBlock)
    {
        pMtxArray = static_cast<Mtx34*>(block.GetData(bufferIndex));
#if NW_G3D_IS_HOST_CAFE
        size_t blockSize = block.GetSize();
        if (CPUCache::IsValid(pMtxArray, blockSize))
        {
            DCZeroRange(pMtxArray, blockSize);
        }
#endif
    }
    else
    {
        pMtxArray = static_cast<Mtx34*>(block.pData);
    }

    if (IsBlockSwapEnabled())
    {
        int smoothSkinMtxCount = m_pRes->GetSmoothMtxCount();
        for (; idxMtx < smoothSkinMtxCount; ++idxMtx)
        {
            int idxBone = pMtxToBoneTable[idxMtx];
            const Mtx34& worldMtx = m_pWorldMtxArray[idxBone];
            const Mtx34& invModelMtx = m_pRes->GetBone(idxBone)->GetInvModelMtx();
            Mtx34 mtx;
            mtx.Mul(worldMtx, invModelMtx);
            Copy32<true>(&pMtxArray[idxMtx], &mtx, sizeof(Mtx34) >> 2);
        }

        int mtxCount = m_pRes->GetMtxCount();
        for (; idxMtx < mtxCount; ++idxMtx)
        {
            int idxBone = pMtxToBoneTable[idxMtx];
            Copy32<true>(&pMtxArray[idxMtx], &m_pWorldMtxArray[idxBone], sizeof(Mtx34) >> 2);
        }
    }
    else
    {
        int smoothSkinMtxCount = m_pRes->GetSmoothMtxCount();
        for (; idxMtx < smoothSkinMtxCount; ++idxMtx)
        {
            int idxBone = pMtxToBoneTable[idxMtx];
            const Mtx34& worldMtx = m_pWorldMtxArray[idxBone];
            const Mtx34& invModelMtx = m_pRes->GetBone(idxBone)->GetInvModelMtx();
            pMtxArray[idxMtx].Mul(worldMtx, invModelMtx);
        }

        int mtxCount = m_pRes->GetMtxCount();
        for (; idxMtx < mtxCount; ++idxMtx)
        {
            int idxBone = pMtxToBoneTable[idxMtx];
            pMtxArray[idxMtx] = m_pWorldMtxArray[idxBone];
        }
    }

    if (block.pData == m_pLCMtxBlock)
    {
        block.DCFlush(bufferIndex);
    }
}

template<typename CalcType, bool useCallback>
void SkeletonObj::CalcWorldImpl(const Mtx34& baseMtx)
{
    int numBone = m_NumBone;
    NW_G3D_ASSERT(numBone > 0);

    CalcType op(GetLocalMtxArray(), m_pWorldMtxArray, m_pScaleArray);

    op.CalcRootBone(baseMtx);
    int idxBone = 0;
    ICalcWorldCallback::CallbackArg callbackArg(idxBone, m_CallbackBone);

    if (NW_G3D_STATIC_CONDITION(useCallback) && callbackArg.GetCallbackBoneIndex() == idxBone)
    {
        WorldMtxManip manip(
            &m_pWorldMtxArray[idxBone], op.GetScale(idxBone), &m_pLocalMtxArray[idxBone].flag);
        m_pCallback->Exec(callbackArg, manip);
    }

    for (++idxBone; idxBone < numBone; ++idxBone)
    {
        ResBone* pBone = GetBone(idxBone);

        op.CalcBone(pBone);

        if (NW_G3D_STATIC_CONDITION(useCallback) && callbackArg.GetCallbackBoneIndex() == idxBone)
        {
            WorldMtxManip manip(
                &m_pWorldMtxArray[idxBone], op.GetScale(idxBone), &m_pLocalMtxArray[idxBone].flag);
            m_pCallback->Exec(callbackArg, manip);
        }
    }

    op.CalcScale(numBone);
}

class CalcWorldBase
{
public:
    CalcWorldBase(LocalMtx* pLocalMtxArray, Mtx34* pWorldMtxArray, Vec3* pScaleArray)
        : m_pLocalMtxArray(pLocalMtxArray)
        , m_pWorldMtxArray(pWorldMtxArray)
        , m_pScaleArray(pScaleArray)
    {
    }

    NW_G3D_FORCE_INLINE void CalcBone( const ResBone* /*bone*/) { NW_G3D_ASSERT(false); }

    NW_G3D_FORCE_INLINE
    void CalcRootBone(const Mtx34& baseMtx)
    {
        LocalMtx& local = m_pLocalMtxArray[0];
        if (CheckFlag(local.flag, ResBone::ROTTRANS_ZERO))
        {
            m_pWorldMtxArray[0].Set(baseMtx);
        }
        else
        {
            m_pWorldMtxArray[0].Mul(baseMtx, local.mtxRT);
        }


        local.flag &= ~ResBone::HI_IDENTITY;
    }

    NW_G3D_FORCE_INLINE
    void CalcScale(int numBone)
    {
        for (int idxBone = 0; idxBone < numBone; ++idxBone)
        {
            const LocalMtx& local = m_pLocalMtxArray[idxBone];
            if (!CheckFlag(local.flag, ResBone::SCALE_ONE))
            {
                Mtx34& worldMtx = m_pWorldMtxArray[idxBone];
                worldMtx.ScaleBases(worldMtx, local.scale);
            }
        }
    }

    NW_G3D_FORCE_INLINE
    Vec3* GetScale(int idxBone)
    {
        return &m_pLocalMtxArray[idxBone].scale;
    }

protected:
    NW_G3D_FORCE_INLINE void AccumulateFlag(bit32& flag, bit32 parentFlag)
    {
        flag &= ~ResBone::HI_IDENTITY;
        flag |= (parentFlag & (flag << ResBone::HIERARCHY_SHIFT)) &
            static_cast<bit32>(ResBone::HI_IDENTITY);
    }

    LocalMtx* m_pLocalMtxArray;
    Mtx34* m_pWorldMtxArray;
    Vec3* m_pScaleArray;
};

class CalcWorldNoScale : public CalcWorldBase
{
public:
    CalcWorldNoScale(LocalMtx* pLocalMtxArray, Mtx34* pWorldMtxArray, Vec3* pScaleArray)
        : CalcWorldBase(pLocalMtxArray, pWorldMtxArray, pScaleArray)
    {
    }

    NW_G3D_FORCE_INLINE
    void CalcBone(const ResBone* bone)
    {
        int index = bone->GetIndex();
        int parentIndex = bone->GetParentIndex();
        Mtx34& worldMtx = m_pWorldMtxArray[index];
        LocalMtx& local = m_pLocalMtxArray[index];
        const Mtx34& parentMtx = m_pWorldMtxArray[parentIndex];
        const LocalMtx& parent = m_pLocalMtxArray[parentIndex];
        AccumulateFlag(local.flag, parent.flag);

        if (CheckFlag(local.flag, ResBone::ROTTRANS_ZERO))
        {
            worldMtx.Set(parentMtx);
        }
        else
        {
            worldMtx.Mul(parentMtx, local.mtxRT);
        }
    }

    NW_G3D_FORCE_INLINE
    void CalcScale(int numBone)
    {
        (void)numBone;
    }

    NW_G3D_FORCE_INLINE
    Vec3* GetScale(int idxBone)
    {
        (void)idxBone;
        return NULL;
    }
};

class CalcWorldStd : public CalcWorldBase
{
public:
    CalcWorldStd(LocalMtx* pLocalMtxArray, Mtx34* pWorldMtxArray, Vec3* pScaleArray)
        : CalcWorldBase(pLocalMtxArray, pWorldMtxArray, pScaleArray)
    {
    }

    NW_G3D_FORCE_INLINE
    void CalcBone(const ResBone* bone)
    {
        int index = bone->GetIndex();
        int parentIndex = bone->GetParentIndex();
        Mtx34& worldMtx = m_pWorldMtxArray[index];
        LocalMtx& local = m_pLocalMtxArray[index];
        const Mtx34& parentMtx = m_pWorldMtxArray[parentIndex];
        const LocalMtx& parent = m_pLocalMtxArray[parentIndex];
        AccumulateFlag(local.flag, parent.flag);

        if (CheckFlag(parent.flag, ResBone::SCALE_ONE))
        {
            if (CheckFlag(local.flag, ResBone::ROTTRANS_ZERO))
            {
                worldMtx.Set(parentMtx);
            }
            else
            {
                worldMtx.Mul(parentMtx, local.mtxRT);
            }
        }
        else
        {
            worldMtx.ScaleBases(parentMtx, parent.scale);
            if (!CheckFlag(local.flag, ResBone::ROTTRANS_ZERO))
            {
                worldMtx.Mul(worldMtx, local.mtxRT);
            }
        }
    }
};

class CalcWorldMaya : public CalcWorldBase
{
public:
    CalcWorldMaya(LocalMtx* pLocalMtxArray, Mtx34* pWorldMtxArray, Vec3* pScaleArray)
        : CalcWorldBase(pLocalMtxArray, pWorldMtxArray, pScaleArray)
    {
    }

    NW_G3D_FORCE_INLINE
    void CalcBone(const ResBone* bone)
    {
        int index = bone->GetIndex();
        int parentIndex = bone->GetParentIndex();
        Mtx34& worldMtx = m_pWorldMtxArray[index];
        LocalMtx& local = m_pLocalMtxArray[index];
        const Mtx34& parentMtx = m_pWorldMtxArray[parentIndex];
        const LocalMtx& parent = m_pLocalMtxArray[parentIndex];
        AccumulateFlag(local.flag, parent.flag);

        if (CheckFlag(local.flag, ResBone::ROTTRANS_ZERO))
        {
            if ((local.flag & ResBone::SEGMENT_SCALE_COMPENSATE) ||
                CheckFlag(parent.flag, ResBone::SCALE_ONE))
            {
                worldMtx.Set(parentMtx);
            }
            else
            {
                worldMtx.ScaleBases(parentMtx, parent.scale);
            }
        }
        else
        {
            if (CheckFlag(parent.flag, ResBone::SCALE_ONE))
            {
                worldMtx.Mul(parentMtx, local.mtxRT);
            }
            else if (local.flag & ResBone::SEGMENT_SCALE_COMPENSATE)
            {
                worldMtx.Set(local.mtxRT);
                worldMtx.m03 *= parent.scale.x;
                worldMtx.m13 *= parent.scale.y;
                worldMtx.m23 *= parent.scale.z;
                worldMtx.Mul(parentMtx, worldMtx);
            }
            else
            {
                worldMtx.ScaleBases(parentMtx, parent.scale);
                worldMtx.Mul(worldMtx, local.mtxRT);
            }
        }
    }
};

class CalcWorldSoftimage : public CalcWorldBase
{
public:
    CalcWorldSoftimage(
        LocalMtx* pLocalMtxArray, Mtx34* pWorldMtxArray, Vec3* pScaleArray)
        : CalcWorldBase(pLocalMtxArray, pWorldMtxArray, pScaleArray)
    {
    }

    NW_G3D_FORCE_INLINE
    void CalcRootBone(const Mtx34& baseMtx)
    {
        CalcWorldBase::CalcRootBone(baseMtx);
        LocalMtx& local = m_pLocalMtxArray[0];
        if (CheckFlag(local.flag, ResBone::SCALE_ONE))
        {
            m_pScaleArray[0].Set(1.0f, 1.0f, 1.0f);
        }
        else
        {
            m_pScaleArray[0].Set(local.scale);
        }
    }

    NW_G3D_FORCE_INLINE
    void CalcBone(const ResBone* bone)
    {
        int index = bone->GetIndex();
        int parentIndex = bone->GetParentIndex();
        Vec3& scale = m_pScaleArray[index];
        Mtx34& worldMtx = m_pWorldMtxArray[index];
        LocalMtx& local = m_pLocalMtxArray[index];
        const Vec3& parentScale = m_pScaleArray[parentIndex];
        const Mtx34& parentMtx = m_pWorldMtxArray[parentIndex];
        const LocalMtx& parent = m_pLocalMtxArray[parentIndex];
        AccumulateFlag(local.flag, parent.flag);

        if (CheckFlag(local.flag, ResBone::ROTTRANS_ZERO))
        {
            worldMtx.Set(parentMtx);
        }
        else
        {
            if (CheckFlag(parent.flag, ResBone::HI_SCALE_ONE))
            {
                worldMtx.Mul(parentMtx, local.mtxRT);
            }
            else
            {
                worldMtx.Set(local.mtxRT);
                worldMtx.m03 *= parentScale.x;
                worldMtx.m13 *= parentScale.y;
                worldMtx.m23 *= parentScale.z;
                worldMtx.Mul(parentMtx, worldMtx);
            }
        }

        if (CheckFlag(local.flag, ResBone::SCALE_ONE))
        {
            scale.Set(parentScale);
        }
        else
        {
            scale.Mul(parentScale, local.scale);
        }
    }

    NW_G3D_FORCE_INLINE
    void CalcScale(int numBone)
    {
        for (int idxBone = 0; idxBone < numBone; ++idxBone)
        {
            const LocalMtx& local = m_pLocalMtxArray[idxBone];
            if (!CheckFlag(local.flag, ResBone::HI_SCALE_ONE))
            {
                Mtx34& worldMtx = m_pWorldMtxArray[idxBone];
                worldMtx.ScaleBases(worldMtx, m_pScaleArray[idxBone]);
            }
        }
    }

    NW_G3D_FORCE_INLINE
    Vec3* GetScale(int idxBone)
    {
        return &m_pScaleArray[idxBone];
    }
};

} } // namespace nw::g3d
