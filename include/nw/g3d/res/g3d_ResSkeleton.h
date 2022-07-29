#ifndef NW_G3D_RES_RESSKELETON_H_
#define NW_G3D_RES_RESSKELETON_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/math/g3d_Vector3.h>
#include <nw/g3d/math/g3d_Matrix34.h>
#include <nw/g3d/math/g3d_Quaternion.h>
#include <nw/g3d/res/g3d_ResCommon.h>
#include <nw/g3d/res/g3d_ResDictionary.h>
#include <nw/g3d/res/g3d_ResUserData.h>

namespace nw { namespace g3d {namespace res {

class ResFile;

struct ResBoneData
{
    BinString ofsName;
    u16 index;
    u16 parentIndex;
    s16 smoothMtxIndex;
    s16 rigidMtxIndex;
    u16 billboardIndex;
    u16 numUserData;

    bit32 flag;
    Vec3 scale;
    union
    {
        Quat quat;
        Vec3 euler;
    } rotate;
    Vec3 translate;

    Offset ofsUserDataDic;

    Mtx34 invModelMtx;
};

class ResBone : private ResBoneData
{
    NW_G3D_RES_COMMON(ResBone);

public:
    enum Flag
    {
        VISIBILITY                  = 0x1 << 0,

        ROT_SHIFT                   = 12,
        ROT_QUAT                    = 0x0 << ROT_SHIFT,
        ROT_EULER_XYZ               = 0x1 << ROT_SHIFT,
        ROT_MASK                    = 0x7 << ROT_SHIFT,

        BILLBOARD_SHIFT             = 16,
        BILLBOARD_NONE              = 0x0 << BILLBOARD_SHIFT,
        BILLBOARD_CHILD             = 0x1 << BILLBOARD_SHIFT,
        BILLBOARD_WORLD_VIEWVECTOR  = 0x2 << BILLBOARD_SHIFT,
        BILLBOARD_WORLD_VIEWPOINT   = 0x3 << BILLBOARD_SHIFT,
        BILLBOARD_SCREEN_VIEWVECTOR = 0x4 << BILLBOARD_SHIFT,
        BILLBOARD_SCREEN_VIEWPOINT  = 0x5 << BILLBOARD_SHIFT,
        BILLBOARD_YAXIS_VIEWVECTOR  = 0x6 << BILLBOARD_SHIFT,
        BILLBOARD_YAXIS_VIEWPOINT   = 0x7 << BILLBOARD_SHIFT,
        BILLBOARD_MAX               = BILLBOARD_YAXIS_VIEWPOINT,
        BILLBOARD_MASK              = 0x7 << BILLBOARD_SHIFT,

        SEGMENT_SCALE_COMPENSATE    = 0x1 << 23,
        SCALE_UNIFORM               = 0x1 << 24,
        SCALE_VOLUME_ONE            = 0x1 << 25,
        ROTATE_ZERO                 = 0x1 << 26,
        TRANSLATE_ZERO              = 0x1 << 27,
        SCALE_ONE                   = SCALE_VOLUME_ONE | SCALE_UNIFORM,
        ROTTRANS_ZERO               = ROTATE_ZERO | TRANSLATE_ZERO,
        IDENTITY                    = SCALE_ONE | ROTATE_ZERO | TRANSLATE_ZERO,
        TRANSFORM_MASK              = SEGMENT_SCALE_COMPENSATE | IDENTITY,

        HIERARCHY_SHIFT             = 4,
        HI_SCALE_UNIFORM            = SCALE_UNIFORM     << HIERARCHY_SHIFT,
        HI_SCALE_VOLUME_ONE         = SCALE_VOLUME_ONE  << HIERARCHY_SHIFT,
        HI_ROTATE_ZERO              = ROTATE_ZERO       << HIERARCHY_SHIFT,
        HI_TRANSLATE_ZERO           = TRANSLATE_ZERO    << HIERARCHY_SHIFT,
        HI_SCALE_ONE                = SCALE_ONE         << HIERARCHY_SHIFT,
        HI_ROTTRANS_ZERO            = ROTTRANS_ZERO     << HIERARCHY_SHIFT,
        HI_IDENTITY                 = IDENTITY          << HIERARCHY_SHIFT
    };

    enum
    {
        BB_INDEX_NONE               = 0xFFFF
    };

    int GetIndex() const { return ref().index; }

    NW_G3D_RES_FIELD_STRING_DECL(Name);

    int GetSmoothMtxIndex() const { return ref().smoothMtxIndex; }

    int GetRigidMtxIndex() const { return ref().rigidMtxIndex; }

    int GetParentIndex() const { return ref().parentIndex; }

    bit32 GetRotateMode() const { return ref().flag & ROT_MASK; }

    bit32 GetBillboardMode() const { return ref().flag & BILLBOARD_MASK; }

    Vec3& GetScale() { return ref().scale; }

    const Vec3& GetScale() const { return ref().scale; }

    Vec3& GetTranslate() { return ref().translate; }

    const Vec3& GetTranslate() const { return ref().translate; }

    Vec3& GetRotateEuler()
    {
        NW_G3D_ASSERT(ROT_EULER_XYZ == GetRotateMode());
        return ref().rotate.euler;
    }

    const Vec3& GetRotateEuler() const
    {
        NW_G3D_ASSERT(ROT_EULER_XYZ == GetRotateMode());
        return ref().rotate.euler;
    }

    Quat& GetRotateQuat()
    {
        NW_G3D_ASSERT(ROT_QUAT == GetRotateMode());
        return ref().rotate.quat;
    }

    const Quat& GetRotateQuat() const
    {
        NW_G3D_ASSERT(ROT_QUAT == GetRotateMode());
        return ref().rotate.quat;
    }

    Mtx34& GetInvModelMtx() { return ref().invModelMtx; }

    const Mtx34& GetInvModelMtx() const { return ref().invModelMtx; }

    NW_G3D_RES_FIELD_DIC_DECL(ResUserData, UserData)
};

struct ResSkeletonData
{
    BinaryBlockHeader blockHeader;

    bit32 flag;
    u16 numBone;
    u16 numSmoothMtx;
    u16 numRigidMtx;
    u16 reserved;

    Offset ofsBoneDic;
    Offset ofsBoneArray;
    Offset ofsMtxToBoneTable;

    BinPtr pUserPtr;
};

class ResSkeleton : private ResSkeletonData
{
    NW_G3D_RES_COMMON(ResSkeleton);

public:
    enum Signature { SIGNATURE = NW_G3D_MAKE_U8X4_AS_U32('F', 'S', 'K', 'L') };

    enum Flag
    {
        SCALE_SHIFT     = 8,
        SCALE_NONE      = 0x0 << SCALE_SHIFT,
        SCALE_STD       = 0x1 << SCALE_SHIFT,
        SCALE_MAYA      = 0x2 << SCALE_SHIFT,
        SCALE_SOFTIMAGE = 0x3 << SCALE_SHIFT,
        SCALE_MASK      = 0x3 << SCALE_SHIFT,

        ROT_SHIFT       = ResBone::ROT_SHIFT,
        ROT_QUAT        = ResBone::ROT_QUAT,
        ROT_EULER_XYZ   = ResBone::ROT_EULER_XYZ,
        ROT_MASK        = ResBone::ROT_MASK,

        TRANSFORM_MASK  = SCALE_MASK | ROT_MASK
    };

    int GetSmoothMtxCount() const { return ref().numSmoothMtx; }

    int GetRigidMtxCount() const { return ref().numRigidMtx; }

    int GetMtxCount() const { return GetSmoothMtxCount() + GetRigidMtxCount(); }

    bit32 GetScaleMode() const { return ref().flag & SCALE_MASK; }

    bit32 GetRotateMode() const { return ref().flag & ROT_MASK; }

    int GetBranchEndIndex(int boneIndex) const;

    void SetUserPtr(void* pUserPtr) { ref().pUserPtr.set_ptr(pUserPtr); }

    void* GetUserPtr() { return ref().pUserPtr.to_ptr(); }

    const void* GetUserPtr() const { return ref().pUserPtr.to_ptr(); }

    template <typename T>
    T* GetUserPtr() { return ref().pUserPtr.to_ptr<T>(); }

    template <typename T>
    const T* GetUserPtr() const { return ref().pUserPtr.to_ptr<T>(); }

    void UpdateBillboardMode();

    NW_G3D_RES_FIELD_CLASS_NAMED_ARRAY_DECL(ResBone, Bone)
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESSKELETON_H_
