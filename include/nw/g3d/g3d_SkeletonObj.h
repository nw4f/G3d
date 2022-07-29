#ifndef NW_G3D_SKELETON_H_
#define NW_G3D_SKELETON_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/fnd/g3d_GfxObject.h>
#include <nw/g3d/res/g3d_ResSkeleton.h>
#include <nw/g3d/g3d_Sizer.h>

namespace nw { namespace g3d {

struct LocalMtxData
{
    bit32 flag;
    Vec3 scale;
    Mtx34 mtxRT;
};

class LocalMtx : public LocalMtxData
{
public:
    LocalMtx() : LocalMtxData() { flag = 0; }

    void EndEdit() { flag &= ~ResBone::IDENTITY; }

    static LocalMtx& DownCast(LocalMtxData& data) { return static_cast<LocalMtx&>(data); }

    static const LocalMtx& DownCast(const LocalMtxData& data)
    {
        return static_cast<const LocalMtx&>(data);
    }

    static LocalMtx* DownCast(LocalMtxData* data) { return static_cast<LocalMtx*>(data); }

    static const LocalMtx* DownCast(const LocalMtxData* data)
    {
        return static_cast<const LocalMtx*>(data);
    }
};

class WorldMtxManip
{
public:
    WorldMtxManip(Mtx34* pMtx, Vec3* pScale, bit32* pFlag)
        : m_pMtx(pMtx), m_pScale(pScale), m_pFlag(pFlag)
    {
        NW_G3D_ASSERT_NOT_NULL(pMtx);
        NW_G3D_ASSERT_NOT_NULL(pFlag);
    }

    const Vec3* GetScale() const { return m_pScale; }

    const Mtx34* GetMtx() const { return m_pMtx; }

    Mtx34* EditMtx()
    {
        *m_pFlag &= ~(ResBone::ROTTRANS_ZERO | ResBone::HI_ROTTRANS_ZERO);
        return m_pMtx;
    }

    bit32 GetFlag() const { return *m_pFlag; }

private:
    Mtx34* m_pMtx;
    Vec3* m_pScale;
    bit32* m_pFlag;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(WorldMtxManip);
};

class ICalcWorldCallback
{
public:
    enum { INVALID_BONE = 0xFFFF };

    virtual ~ICalcWorldCallback() {}

    class CallbackArg
    {
    public:
        CallbackArg(const int& boneIndex, int callbackBone)
            : m_BoneIndex(boneIndex), m_CallbackBone(callbackBone)
        {
        }

        int GetBoneIndex() const { return m_BoneIndex; }

        int GetCallbackBoneIndex() const { return m_CallbackBone; }

        void SetCallbackBoneIndex(int boneIndex) { m_CallbackBone = boneIndex; }

    private:
        const int& m_BoneIndex;
        int m_CallbackBone;

        NW_G3D_DISALLOW_COPY_AND_ASSIGN(CallbackArg);
    };

    virtual void Exec(CallbackArg& arg, WorldMtxManip& manip) = 0;
};

class SkeletonObj
{
public:
    enum Flag
    {
        BLOCK_BUFFER_VALID  = 0x1 << 0,
        BLOCK_BUFFER_SWAP   = 0x1 << 1,
        CACHE_LOCAL_MTX     = 0x1 << 2,
        INVALID_LOCAL_MTX   = 0x1 << 3,
        MASK_LOCAL_MTX      = CACHE_LOCAL_MTX | INVALID_LOCAL_MTX,
        CACHE_WORLD_MTX     = 0x1 << 4,
        INVALID_WORLD_MTX   = 0x1 << 5,
        MASK_WORLD_MTX      = CACHE_WORLD_MTX | INVALID_WORLD_MTX,

        SCALE_SHIFT         = ResSkeleton::SCALE_SHIFT,
        SCALE_NONE          = ResSkeleton::SCALE_NONE,
        SCALE_STD           = ResSkeleton::SCALE_STD,
        SCALE_MAYA          = ResSkeleton::SCALE_MAYA,
        SCALE_SOFTIMAGE     = ResSkeleton::SCALE_SOFTIMAGE,
        SCALE_MASK          = ResSkeleton::SCALE_MASK,

        ROT_SHIFT           = ResSkeleton::ROT_SHIFT,
        ROT_EULER_XYZ       = ResSkeleton::ROT_EULER_XYZ,
        ROT_QUAT            = ResSkeleton::ROT_QUAT,
        ROT_MASK            = ResSkeleton::ROT_MASK
    };

    class InitArg;

    enum Alignment
    {
        BUFFER_ALIGNMENT        = LL_CACHE_FETCH_SIZE,

        BLOCK_BUFFER_ALIGNMENT  = GX2_UNIFORM_BLOCK_ALIGNMENT
    };

    SkeletonObj() : m_pRes(NULL), m_pUserPtr(NULL), m_pBufferPtr(NULL), m_pBlockBuffer(NULL) {}

    static size_t CalcBufferSize(const InitArg& arg);

    bool Init(const InitArg& arg, void* pBuffer, size_t bufferSize);

    size_t CalcBlockBufferSize() const;

    bool SetupBlockBuffer(void* pBuffer, size_t bufferSize);

    void CleanupBlockBuffer();

    void ClearLocalMtx();

    void CalcWorldMtx(const Mtx34& baseMtx);

    void CalcBillboardMtx(Mtx34* pMtxArray, const Mtx34& cameraMtx, int boneIndex,
        bool combineWorld = false) const;

    void CalcMtxBlock(int bufferIndex = 0);

    ResSkeleton* GetResource() { return m_pRes; }

    const ResSkeleton* GetResource() const { return m_pRes; }

    void* GetBufferPtr() { return m_pBufferPtr; }

    void* GetBlockBufferPtr() { return m_pBlockBuffer; }

    bool IsBlockBufferValid() const { return (m_Flag & BLOCK_BUFFER_VALID) != 0; }

    int GetScaleMode() const { return m_Flag & SCALE_MASK; }

    int GetRotateMode() const { return m_Flag & ROT_MASK; }

    LocalMtx* GetLocalMtxArray()
    {
        m_Flag |= CACHE_LOCAL_MTX;
        return LocalMtx::DownCast(m_pLocalMtxArray);
    }

    const LocalMtx* GetLocalMtxArray() const
    {
        m_Flag |= CACHE_LOCAL_MTX;
        return LocalMtx::DownCast(m_pLocalMtxArray);
    }

    Mtx34* GetWorldMtxArray() {
        m_Flag |= CACHE_WORLD_MTX;
        return m_pWorldMtxArray;
    }

    const Mtx34* GetWorldMtxArray() const
    {
        m_Flag |= CACHE_WORLD_MTX;
        return m_pWorldMtxArray;
    }

    GfxBuffer& GetMtxBlock() { return m_MtxBlock; }

    const GfxBuffer& GetMtxBlock() const { return m_MtxBlock; }

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

    int GetBoneCount() const { return m_NumBone; }

    ResBone* GetBone(int boneIndex)
    {
        return ResBone::ResCast(&m_pBoneArray[boneIndex]);
    }

    const ResBone* GetBone(int boneIndex) const
    {
        return ResBone::ResCast(&m_pBoneArray[boneIndex]);
    }

    ResBone* GetBone(const char* name) { return m_pRes->GetBone(name); }

    const ResBone* GetBone(const char* name) const { return m_pRes->GetBone(name); }

    const char* GetBoneName(int boneIndex) const { return m_pRes->GetBoneName(boneIndex); }

    int GetBoneIndex(const char* name) const { return m_pRes->GetBoneIndex(name); }

    void SetCalcWorldCallback(ICalcWorldCallback* pCallback) { m_pCallback = pCallback; }

    ICalcWorldCallback* GetCalcWorldCallback() { return m_pCallback; }

    const ICalcWorldCallback* GetCalcWorldCallback() const { return m_pCallback; }

    void SetCallbackBoneIndex(int boneIndex)
    {
        m_CallbackBone = (boneIndex >= 0 && boneIndex < m_NumBone) ?
            static_cast<u16>(boneIndex) : ICalcWorldCallback::INVALID_BONE;
    }

    int GetCallbackBoneIndex() const
    {
        return m_CallbackBone >= ICalcWorldCallback::INVALID_BONE ? -1 : m_CallbackBone;
    }

    size_t CalcLocalMtxLCSize() const
    {
        return Align(sizeof(LocalMtx) * m_NumBone, LL_CACHE_FETCH_SIZE);
    }

    size_t LCMountLocalMtx(void* pLC, size_t size, bool load);

    void LCUnmountLocalMtx(bool store);

    bool IsLocalMtxOnCache() const { return !!(m_Flag & MASK_LOCAL_MTX); }

    size_t CalcWorldMtxLCSize() const
    {
        return Align(sizeof(Mtx34) * m_NumBone, LL_CACHE_FETCH_SIZE);
    }

    size_t LCMountWorldMtx(void* pLC, size_t size, bool load);

    void LCUnmountWorldMtx(bool store);

    bool IsWorldMtxOnCache() const { return !!(m_Flag & MASK_WORLD_MTX); }

    size_t CalcMtxBlockLCSize() const
    {
        return Align(sizeof(Mtx34) * m_pRes->GetMtxCount(), LL_CACHE_FETCH_SIZE);
    }

    size_t LCMountMtxBlock(void* pLC, size_t size, bool load, int bufferIndex = 0);

    void LCUnmountMtxBlock(bool store, int bufferIndex = 0);

protected:
    class Impl;

    template<typename CalcType, bool useCallback>
    void CalcWorldImpl(const Mtx34& baseMtx);

private:
    class Sizer;

    ResSkeleton* m_pRes;

    mutable bit16 m_Flag;
    u8 m_NumBuffering;
    u8 reserved;

    ResBoneData* m_pBoneArray;
    LocalMtxData* m_pLocalMtxArray;
    Mtx34* m_pWorldMtxArray;
    Vec3* m_pScaleArray;

    GfxBuffer m_MtxBlock;

    u16 m_NumBone;
    u16 m_CallbackBone;
    ICalcWorldCallback* m_pCallback;

    void* m_pMemLocalMtxArray;
    void* m_pMemWorldMtxArray;
    void* m_pLCMtxBlock;
    void* m_pUserPtr;
    void* m_pBufferPtr;
    void* m_pBlockBuffer;

    NW_G3D_DISALLOW_COPY_AND_ASSIGN(SkeletonObj);
};

class SkeletonObj::Sizer : public nw::g3d::Sizer
{
public:
    Sizer() : nw::g3d::Sizer() {}

    void Calc(const InitArg& arg);

    enum
    {
        WORLD_MTX,
        LOCAL_MTX,
        SCALE,
        NUM_CHUNK
    };

    Chunk chunk[NUM_CHUNK];
};

class SkeletonObj::InitArg
{
public:
    explicit InitArg(ResSkeleton* resource)
        : m_pRes(resource)
        , m_NumBuffering(1)
    {
        NW_G3D_ASSERT_NOT_NULL(resource);
    }

    void BufferingCount(int count) { m_NumBuffering = count; m_Sizer.Invalidate(); }

    int GetBufferingCount() const { return m_NumBuffering; }

    ResSkeleton* GetResource() const { return m_pRes; }

    Sizer& GetSizer() const { return m_Sizer; }

private:
    ResSkeleton* m_pRes;
    int m_NumBuffering;
    mutable Sizer m_Sizer;
};

} } // namespace nw::g3d

#endif // NW_G3D_SKELETON_H_
