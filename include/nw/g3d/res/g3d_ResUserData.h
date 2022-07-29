#ifndef NW_G3D_RES_USERDATA_H_
#define NW_G3D_RES_USERDATA_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/res/g3d_ResCommon.h>

namespace nw { namespace g3d { namespace res {

struct ResUserDataData
{
    BinString ofsName;
    u16 count;
    u8 type;
    u8 reserved;
    union Data
    {
        s32 iValue[1];
        f32 fValue[1];
        Offset ofsString[1];
        struct
        {
            u32 size;
            u8 value[4];
        };
    } data;
};

class ResUserData : private ResUserDataData
{
    NW_G3D_RES_COMMON(ResUserData);

public:
    enum Type
    {
        INT,
        FLOAT,
        STRING,
        WSTRING,
        STREAM
    };

    NW_G3D_RES_FIELD_STRING_DECL(Name)

    int GetCount() const { return ref().count; }

    Type GetType() const { return static_cast<Type>(ref().type); }

    int* GetInt()
    {
        NW_G3D_ASSERT(ref().type == INT);
        return reinterpret_cast<int*>(ref().data.iValue);
    }

    const int* GetInt() const
    {
        NW_G3D_ASSERT(ref().type == INT);
        return reinterpret_cast<const int*>(ref().data.iValue);
    }

    float* GetFloat()
    {
        NW_G3D_ASSERT(ref().type == FLOAT);
        return reinterpret_cast<float*>(ref().data.fValue);
    }

    const float* GetFloat() const
    {
        NW_G3D_ASSERT(ref().type == FLOAT);
        return reinterpret_cast<const float*>(ref().data.fValue);
    }

    const char* GetString(int strIndex) const
    {
        NW_G3D_ASSERT(ref().type == STRING);
        NW_G3D_ASSERT_INDEX_BOUNDS(strIndex, ref().count);
        return ref().data.ofsString[strIndex].to_ptr<char>();
    }

    const char16* GetWString(int strIndex) const
    {
        NW_G3D_ASSERT(ref().type == WSTRING);
        NW_G3D_ASSERT_INDEX_BOUNDS(strIndex, ref().count);
        return ref().data.ofsString[strIndex].to_ptr<char16>();
    }

    void* GetStream()
    {
        NW_G3D_ASSERT(ref().type == STREAM);
        return ref().data.value;
    }

    const void* GetStream() const
    {
        NW_G3D_ASSERT(ref().type == STREAM);
        return ref().data.value;
    }

    size_t GetStreamSize() const
    {
        NW_G3D_ASSERT(ref().type == STREAM);
        return ref().data.size;
    }
};

} } } // namespace nw::g3d

#endif // NW_G3D_RES_USERDATA_H_
