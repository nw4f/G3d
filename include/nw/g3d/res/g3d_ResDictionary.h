#ifndef NW_G3D_RES_RESDICTIONARY_H_
#define NW_G3D_RES_RESDICTIONARY_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>

namespace nw { namespace g3d { namespace res {

struct ResDicPatriciaData
{
    u32 size;
    s32 numData;

    struct Node
    {
        u32 refBit;
        u16 idxLeft;
        u16 idxRight;
        BinString ofsName;
        Offset ofsData;
    } node[1];
};

class ResDicPatricia : private ResDicPatriciaData
{
    NW_G3D_RES_COMMON(ResDicPatricia);

public:
    using ResDicPatriciaData::Node;

    int GetDataCount() const
    {
        return ref().numData;
    }

    void* Get(int idx) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(idx, ref().numData);
        return const_cast<void*>(ref().node[idx + 1].ofsData.to_ptr());
    }

private:
    void* Find_(const char* str) const;
  //void* Find_(const char* str, size_t len) const;
    void* Find_(const ResName* name) const;

    int FindIndex_(const char* str) const;
  //int FindIndex_(const char* str, size_t len) const;
    int FindIndex_(const ResName* name) const;

public:
    static void* Find(const ResDicPatricia* pDic, const char* str);
    static void* Find(const ResDicPatricia* pDic, const ResName* name);

    static int FindIndex(const ResDicPatricia* pDic, const char* str);
    static int FindIndex(const ResDicPatricia* pDic, const ResName* name);

public:
    Node* FindNode(const char* str, size_t len) const;
    Node* FindNode(const ResName* name) const;

    const ResName* GetResName(u32 idx) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(idx, ref().numData);
        return ref().node[idx + 1].ofsName.GetResName();
    }

    const char* GetName(int idx) const
    {
        NW_G3D_ASSERT_INDEX_BOUNDS(idx, ref().numData);
        return ref().node[idx + 1].ofsName.to_ptr();
    }

    enum BuildResult
    {
        SUCCESS = 0,

        ERR_NAME_DUPLICATION
    };

    BuildResult Build();
};

typedef ResDicPatricia ResDicType;

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESDICTIONARY_H_
