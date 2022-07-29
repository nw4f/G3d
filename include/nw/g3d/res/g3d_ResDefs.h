#ifndef NW_G3D_RES_DEFS_H_
#define NW_G3D_RES_DEFS_H_

#include <nw/g3d/g3d_config.h>

#define NW_G3D_RES_ASSERT_INDEX(name, idx) NW_G3D_ASSERT_INDEX_BOUNDS(idx, Get##name##Count())

#define NW_G3D_RES_DIC(offset) ResDicType::ResCast(offset.to_ptr<ResDicType::DataType>())

#define NW_G3D_RES_FIELD_DIC_LOOKUP_IDX_DECL(name)                                                 \
    int Get##name##Index(const char* str) const                                                    \
    {                                                                                              \
        return NW_G3D_RES_DIC(ref().ofs##name##Dic)->FindIndex(str);                               \
    }                                                                                              \
    int Get##name##Index(const ResName* str) const                                                 \
    {                                                                                              \
        return NW_G3D_RES_DIC(ref().ofs##name##Dic)->FindIndex(str);                               \
    }                                                                                              \
    const char* Get##name##Name(int elemIndex) const                                               \
    {                                                                                              \
        NW_G3D_RES_ASSERT_INDEX(name, elemIndex);                                                  \
        return NW_G3D_RES_DIC(ref().ofs##name##Dic)->GetName(elemIndex);                           \
    }                                                                                              \


#define NW_G3D_RES_FIELD_DIC_LOOKUP_DECL(type, name)                                               \
    int Get##name##Count() const { return ref().num##name; }                                       \
    type* Get##name(const char* str)                                                               \
    {                                                                                              \
        void* ptr = NW_G3D_RES_DIC(ref().ofs##name##Dic)->Find(str);                               \
        return type::ResCast(static_cast<type##Data*>(ptr));                                       \
    }                                                                                              \
    type* Get##name(const ResName* str)                                                            \
    {                                                                                              \
        void* ptr = NW_G3D_RES_DIC(ref().ofs##name##Dic)->Find(str);                               \
        return type::ResCast(static_cast<type##Data*>(ptr));                                       \
    }                                                                                              \
    const type* Get##name(const char* str) const                                                   \
    {                                                                                              \
        void* ptr = NW_G3D_RES_DIC(ref().ofs##name##Dic)->Find(str);                               \
        return type::ResCast(static_cast<type##Data*>(ptr));                                       \
    }                                                                                              \
    const type* Get##name(const ResName* str) const                                                \
    {                                                                                              \
        void* ptr = NW_G3D_RES_DIC(ref().ofs##name##Dic)->Find(str);                               \
        return type::ResCast(static_cast<type##Data*>(ptr));                                       \
    }                                                                                              \
    NW_G3D_RES_FIELD_DIC_LOOKUP_IDX_DECL(name)                                                     \


#define NW_G3D_RES_FIELD_DIC_DECL(type, name)                                                      \
    NW_G3D_RES_FIELD_DIC_LOOKUP_DECL(type, name)                                                   \
    type* Get##name(int elemIndex)                                                                 \
    {                                                                                              \
        NW_G3D_RES_ASSERT_INDEX(name, elemIndex);                                                  \
        void* ptr = NW_G3D_RES_DIC(ref().ofs##name##Dic)->Get(elemIndex);                          \
        return type::ResCast(static_cast<type##Data*>(ptr));                                       \
    }                                                                                              \
    const type* Get##name(int elemIndex) const                                                     \
    {                                                                                              \
        NW_G3D_RES_ASSERT_INDEX(name, elemIndex);                                                  \
        void* ptr = NW_G3D_RES_DIC(ref().ofs##name##Dic)->Get(elemIndex);                          \
        return type::ResCast(static_cast<type##Data*>(ptr));                                       \
    }                                                                                              \


#define NW_G3D_RES_FIELD_STR_DIC_DECL(name)                                                        \
    NW_G3D_RES_FIELD_DIC_LOOKUP_IDX_DECL(name)                                                     \
    int Get##name##Count() const { return ref().num##name; }                                       \
    const char* Get##name(const char* str) const                                                   \
    {                                                                                              \
        return static_cast<const char*>(NW_G3D_RES_DIC(ref().ofs##name##Dic)->Find(str));          \
    }                                                                                              \
    const char* Get##name(const ResName* str) const                                                \
    {                                                                                              \
        return static_cast<const char*>(NW_G3D_RES_DIC(ref().ofs##name##Dic)->Find(str));          \
    }                                                                                              \
    const char* Get##name(int elemIndex) const                                                     \
    {                                                                                              \
        NW_G3D_RES_ASSERT_INDEX(name, elemIndex);                                                  \
        return static_cast<const char*>(NW_G3D_RES_DIC(ref().ofs##name##Dic)->Get(elemIndex));     \
    }                                                                                              \


#define NW_G3D_RES_FIELD_EMPTY_DIC_DECL(name)                                                      \
    NW_G3D_RES_FIELD_DIC_LOOKUP_IDX_DECL(name)                                                     \
    int Get##name##Count() const { return ref().num##name; }                                       \


// 配列から指定されたインデックスの type を返す
// 範囲外のインデックスを指定された場合はアサートする
#define NW_G3D_RES_FIELD_CLASS_ARRAY_DECL(type, name)                                              \
    int Get##name##Count() const { return ref().num##name; }                                       \
    type* Get##name(int elemIndex)                                                                 \
    {                                                                                              \
        NW_G3D_RES_ASSERT_INDEX(name, elemIndex);                                                  \
        type##Data* pArray = ref().ofs##name##Array.to_ptr<type##Data>();                          \
        return type::ResCast(&pArray[elemIndex]);                                                  \
    }                                                                                              \
    const type* Get##name(int elemIndex) const                                                     \
    {                                                                                              \
        NW_G3D_RES_ASSERT_INDEX(name, elemIndex);                                                  \
        const type##Data* pArray = ref().ofs##name##Array.to_ptr<type##Data>();                    \
        return type::ResCast(const_cast<type##Data*>(&pArray[elemIndex]));                         \
    }                                                                                              \


#define NW_G3D_RES_FIELD_CLASS_NAMED_ARRAY_DECL(type, name)                                        \
    NW_G3D_RES_FIELD_DIC_LOOKUP_DECL(type, name)                                                   \
    type* Get##name(int elemIndex)                                                                 \
    {                                                                                              \
        NW_G3D_RES_ASSERT_INDEX(name, elemIndex);                                                  \
        type##Data* pArray = ref().ofs##name##Array.to_ptr<type##Data>();                          \
        return type::ResCast(&pArray[elemIndex]);                                                  \
    }                                                                                              \
    const type* Get##name(int elemIndex) const                                                     \
    {                                                                                              \
        NW_G3D_RES_ASSERT_INDEX(name, elemIndex);                                                  \
        const type##Data* pArray = ref().ofs##name##Array.to_ptr<type##Data>();                    \
        return type::ResCast(const_cast<type##Data*>(&pArray[elemIndex]));                         \
    }                                                                                              \


#define NW_G3D_RES_FIELD_CLASS_DECL(type, name)                                                    \
    type* Get##name() { return type::ResCast(ref().ofs##name.to_ptr<type##Data>()); }              \
    const type* Get##name() const { return type::ResCast(ref().ofs##name.to_ptr<type##Data>()); }  \


#define NW_G3D_RES_FIELD_DEREFERENCE_DECL(type)                                                    \
    Res##type* Get() { return Res##type::ResCast(ref().ofsRefData.to_ptr<Res##type##Data>()); }    \
    const Res##type* Get() const                                                                   \
    {                                                                                              \
        return Res##type::ResCast(ref().ofsRefData.to_ptr<Res##type##Data>());                     \
    }                                                                                              \


#define NW_G3D_RES_FIELD_STRING_DECL(name)                                                         \
    const char* Get##name() const { return ref().ofs##name.to_ptr(); }                             \


#endif // NW_G3D_RES_DEFS_H_
