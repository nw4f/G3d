#ifndef NW_G3D_RES_RESCOMMON_H_
#define NW_G3D_RES_RESCOMMON_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/ut/g3d_Inlines.h>
#include <nw/g3d/res/g3d_ResDefs.h>
#include <cstring>

namespace nw { namespace g3d { namespace res {

#define NW_G3D_VALIDITY_ASSERT \
    NW_G3D_ASSERTMSG(ptr(), "%s::%s: Object not valid.", GetClassName(), __FUNCTION__)

#define NW_G3D_RES_COMMON_ALIGN(class_name, alignment)                                             \
public:                                                                                            \
    typedef class_name##Data DataType;                                                             \
    DataType* ptr() { return this; }                                                               \
    const DataType* ptr() const { return this; }                                                   \
    DataType& ref() { NW_G3D_VALIDITY_ASSERT; return *ptr(); }                                     \
    const DataType& ref() const { NW_G3D_VALIDITY_ASSERT; return *ptr(); }                         \
    static class_name* ResCast(DataType* ptr)                                                      \
    { NW_G3D_ASSERT_ADDR_ALIGNMENT(ptr, alignment); return static_cast<class_name*>(ptr); }        \
    static const class_name* ResCast(const DataType* ptr)                                          \
    { NW_G3D_ASSERT_ADDR_ALIGNMENT(ptr, alignment); return static_cast<const class_name*>(ptr); }  \
    static const char* GetClassName() { return #class_name; }                                      \
private:                                                                                           \
    NW_G3D_DISALLOW_COPY_AND_ASSIGN(class_name)                                                    \


#define NW_G3D_RES_COMMON(class_name) NW_G3D_RES_COMMON_ALIGN(class_name, 4)

struct ResNameData
{
    typedef u32 LengthType;
    LengthType len;
    char str[sizeof(LengthType)];
};

class ResName : private ResNameData
{
    NW_G3D_RES_COMMON(ResName);

public:
    using ResNameData::LengthType;

    size_t GetLength() const { return static_cast<size_t>(ref().len); }

    const char* GetName() const { return &ref().str[0]; }

    bool Equals(const char* str, size_t len) const
    {
        return (GetLength() == len) && (std::strcmp(GetName(), str) == 0);
    }

    bool Equals(const ResName* rhs) const
    {
        return (ptr() == rhs->ptr()) || Equals(rhs->GetName(), rhs->GetLength());
    }
};

class Offset
{
private:
    s32 offset;

public:
    operator s32() const { return offset; }

    Offset& operator=(s32 ofs) { offset = ofs; return *this; }

    void  set_ptr(const void* ptr)
    {
        offset = (ptr == NULL) ? 0 :
            static_cast<s32>(reinterpret_cast<intptr_t>(ptr) - reinterpret_cast<intptr_t>(this));
    }

    void* to_ptr() { return to_ptr<void>(); }

    const void* to_ptr() const { return to_ptr<void>(); }

    template<typename T>
    T* to_ptr() { return (offset == 0) ? NULL : AddOffset<T>(this, offset); }

    template<typename T>
    const T* to_ptr() const { return (offset == 0) ? NULL : AddOffset<T>(this, offset); }

    Offset* to_table_ptr() { return static_cast<Offset*>(to_ptr()); }

    const Offset* to_table_ptr() const { return static_cast<const Offset*>(to_ptr()); }
};

class BinString
{
public:
    Offset offset;

    const char* to_ptr() const { return offset.to_ptr<char>(); }

    void set_ptr(const char* ptr) { offset.set_ptr(ptr); }

    const ResName* GetResName() const
    {
        return offset == 0 ? NULL : ResName::ResCast(
            AddOffset<ResNameData>(this, offset - sizeof(ResName::LengthType)));
    }
};

class BinPtr
{
public:
    union
    {
        u32 addr;
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
        void* ptr;
#endif
    };

    void* to_ptr() { return to_ptr<void>(); }

    const void* to_ptr() const { return to_ptr<void>(); }

    template <typename T>
    T* to_ptr()
    {
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
        return static_cast<T*>(ptr);
#else
        return NULL;
#endif
    }

    template <typename T>
    const T* to_ptr() const
    {
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
        return static_cast<const T*>(ptr);
#else
        return NULL;
#endif
    }

    BinPtr& set_ptr(void* ptr)
    {
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
        this->ptr = ptr;
#else
        NW_G3D_ASSERT(ptr == NULL);
        NW_G3D_UNUSED(ptr);
        this->addr = 0;
#endif
        return *this;
    }
};

struct BinaryFileHeader
{
    enum
    {
        BYTE_ORDER_MARK = 0xFEFF
    };

    union
    {
        u8 signature[4];
        u32 sigWord;
    };
    union
    {
        u8 version[4];
        u32 verWord;
    };
    u16 byteOrder;
    u16 headerSize;
    u32 fileSize;
};

struct BinaryBlockHeader
{
    union
    {
        u8 signature[4];
        u32 sigWord;
    };
};

struct ResExternalFileData
{
    Offset offset;
    u32 size;
};

class ResExternalFile : private ResExternalFileData
{
    NW_G3D_RES_COMMON(ResExternalFile);

public:
    size_t GetSize() const { return ref().size; }

    void* GetData() { return ref().offset.to_ptr(); }

    const void* GetData() const { return ref().offset.to_ptr(); }
};

} } } // namespace nw::g3d::res

#endif // NW_G3D_RES_RESCOMMON_H_
