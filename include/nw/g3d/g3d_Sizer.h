#ifndef NW_G3D_SIZER_H_
#define NW_G3D_SIZER_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/ut/g3d_Inlines.h>

namespace nw { namespace g3d {

class Sizer
{
public:
    struct Chunk
    {
        size_t size;
        size_t align;
        size_t offset;
    };

    Sizer() : pChunk(NULL), totalSize(0) {}

    size_t GetTotalSize() const { return totalSize; }

    bool IsValid() const { return pChunk != NULL; }

    void Invalidate() { pChunk = NULL; totalSize = 0; }

    template <typename T>
    NW_G3D_FORCE_INLINE T* GetPtr(void* ptr, int type) const
    {
        NW_G3D_ASSERT_NOT_NULL(pChunk);
        return pChunk[type].size ? AddOffset<T>(ptr, pChunk[type].offset) : NULL;
    }

    NW_G3D_FORCE_INLINE void* GetPtr(void* ptr, int type) const
    {
        NW_G3D_ASSERT_NOT_NULL(pChunk);
        return pChunk[type].size ? AddOffset(ptr, pChunk[type].offset) : NULL;
    }

protected:
    void CalcOffset(Chunk* pChunk, int count)
    {
        int idx = 0;
        pChunk[idx].offset = 0;
        for (; idx < count - 1; ++idx)
        {
            size_t offset = pChunk[idx].offset + pChunk[idx].size;
            if (pChunk[idx + 1].size > 0)
            {
                size_t align = pChunk[idx + 1].align;
                if (align > 1)
                {
                    NW_G3D_ASSERT(IsPowerOfTwo(align));
                    size_t mis = offset & (align - 1);
                    if (mis != 0)
                    {
                        size_t pad = align - mis;
                        pChunk[idx].size += pad;
                        offset += pad;
                    }
                }
            }
            pChunk[idx + 1].offset = offset;
        }
        totalSize = pChunk[idx].offset + pChunk[idx].size;
        size_t align = alignof(max_align_t);
        NW_G3D_ASSERT(IsPowerOfTwo(align));
        size_t mis = totalSize & (align - 1);
        if (mis != 0)
        {
            size_t pad = align - mis;
            totalSize += pad;
        }
        this->pChunk = pChunk;
    }

private:
    Chunk* pChunk;
    size_t totalSize;
};

} } // namespace nw::g3d

#endif // NW_G3D_SIZER_H_
