#ifndef NW_G3D_DEFS_H_
#define NW_G3D_DEFS_H_

#define NW_G3D_VERSION_MAJOR            ( 3 )
#define NW_G3D_VERSION_MINOR            ( 0 )
#define NW_G3D_VERSION_MICRO            ( 0 )

#define NW_G3D_VERSION_BINARYBUGFIX     ( 1 )

#define NW_G3D_WIN_WIN                  ( 0x1 )
#define NW_G3D_CAFE_CAFE                ( 0x4 )
#define NW_G3D_CAFE_WIN                 ( 0x5 )

#if !defined( NW_G3D_PLATFORM )
    #if defined( _WIN32 )
        #define NW_G3D_PLATFORM         ( NW_G3D_CAFE_WIN )
    #else
        #define NW_G3D_PLATFORM         ( NW_G3D_CAFE_CAFE )
    #endif
#endif

#define NW_G3D_IS_TARGET_WIN            ( NW_G3D_PLATFORM == NW_G3D_WIN_WIN )
#define NW_G3D_IS_TARGET_CAFE           ( ( NW_G3D_PLATFORM | 1 ) == NW_G3D_CAFE_WIN )

#define NW_G3D_IS_HOST_WIN              ( NW_G3D_PLATFORM & 1 )
#define NW_G3D_IS_HOST_CAFE             ( NW_G3D_PLATFORM == NW_G3D_CAFE_CAFE )

#define NW_G3D_LITTLE_ENDIAN            ( 0 )
#define NW_G3D_BIG_ENDIAN               ( 1 )

#if NW_G3D_IS_TARGET_CAFE
    #define NW_G3D_TARGET_ENDIAN        ( NW_G3D_BIG_ENDIAN )
#else
    #define NW_G3D_TARGET_ENDIAN        ( NW_G3D_LITTLE_ENDIAN )
#endif

#if NW_G3D_IS_HOST_WIN
    #define NW_G3D_HOST_ENDIAN          ( NW_G3D_LITTLE_ENDIAN )
#else
    #define NW_G3D_HOST_ENDIAN          ( NW_G3D_TARGET_ENDIAN )
#endif

#define NW_G3D_32BIT                    ( 2 )
#define NW_G3D_64BIT                    ( 3 )

#define NW_G3D_TARGET_ARCH              ( NW_G3D_32BIT )

#define NW_G3D_TARGET_PTRSIZE           ( 1 << NW_G3D_TARGET_ARCH )
#define NW_G3D_TARGET_DEFAULT_ALIGNMENT ( NW_G3D_TARGET_PTRSIZE )

#if defined( _WIN64 )
  //#define NW_G3D_HOST_ARCH            ( NW_G3D_64BIT )
    #error "Win64 is not supported"
#else
    #define NW_G3D_HOST_ARCH            ( NW_G3D_32BIT )
#endif

#define NW_G3D_HOST_PTRSIZE             ( 1 << NW_G3D_HOST_ARCH )
#define NW_G3D_HOST_DEFAULT_ALIGNMENT   ( NW_G3D_HOST_PTRSIZE )

#define NW_G3D_DEBUG                    ( 1 )
#define NW_G3D_DEVELOP                  ( 2 )
#define NW_G3D_RELEASE                  ( 3 )

#if !defined( NW_G3D_BUILD )
  #if defined( NW_DEBUG )
    #define NW_G3D_BUILD                ( NW_G3D_DEBUG )
  #elif defined( NW_DEVELOP )
    #define NW_G3D_BUILD                ( NW_G3D_DEVELOP )
  #elif defined( NW_RELEASE )
    #define NW_G3D_BUILD                ( NW_G3D_RELEASE )
  #else
    #error "Unknown build"
  #endif
#endif

#define NW_G3D_IS_DEBUG                 ( NW_G3D_BUILD == NW_G3D_DEBUG )
#define NW_G3D_IS_DEVELOP               ( NW_G3D_BUILD == NW_G3D_DEVELOP )
#define NW_G3D_IS_RELEASE               ( NW_G3D_BUILD == NW_G3D_RELEASE )

#define NW_G3D_IS_GX2                   ( NW_G3D_IS_HOST_CAFE )
#define NW_G3D_IS_GL                    ( !NW_G3D_IS_HOST_CAFE )

#if NW_G3D_IS_HOST_WIN
    #define NW_G3D_FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define NW_G3D_FORCE_INLINE inline
#endif

#define NW_G3D_INLINE inline
#define NW_G3D_MATH_INLINE inline

#if defined( __ghs__ )
    #define NW_G3D_ALIGN(alignment) ALIGNVAR(alignment)
#else
    #define NW_G3D_ALIGN(alignment) __attribute__((aligned(alignment)))
#endif

#define NW_G3D_UNUSED( var ) (void)var

#define NW_G3D_STATIC_CONDITION(exp) (exp)

#define NW_G3D_TABLE_FIELD static const

#if defined( __ghs__ )
    #define NW_G3D_CODE_POSITION_NAME __BASE__
#else
    #define NW_G3D_CODE_POSITION_NAME __FUNCTION__
#endif

#define NW_G3D_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);                      \
  void operator=(const TypeName&)

#define NW_G3D_DEPRECATED_FUNCTION_MSG(MFunc, msg) MFunc __attribute__ ((deprecated))

#define NW_G3D_DEPRECATED_FUNCTION(MFunc) NW_G3D_DEPRECATED_FUNCTION_MSG(MFunc, \
    "## g3d deprecated function ##")

#if NW_G3D_HOST_ENDIAN == NW_G3D_LITTLE_ENDIAN

    #define NW_G3D_MAKE_U8X4_AS_U32(a,b,c,d)    \
        static_cast<u32>(                       \
              (static_cast<u8>(a) <<  0)        \
            | (static_cast<u8>(b) <<  8)        \
            | (static_cast<u8>(c) << 16)        \
            | (static_cast<u8>(d) << 24)        \
        )

    #define NW_G3D_GET_SIGNATURE0(sig) ((static_cast<u32>(sig) >>  0) & 0xFF)
    #define NW_G3D_GET_SIGNATURE1(sig) ((static_cast<u32>(sig) >>  8) & 0xFF)
    #define NW_G3D_GET_SIGNATURE2(sig) ((static_cast<u32>(sig) >> 16) & 0xFF)
    #define NW_G3D_GET_SIGNATURE3(sig) ((static_cast<u32>(sig) >> 24) & 0xFF)

#else

    #define NW_G3D_MAKE_U8X4_AS_U32(a,b,c,d)    \
        static_cast<u32>(                       \
              (static_cast<u8>(a) << 24)        \
            | (static_cast<u8>(b) << 16)        \
            | (static_cast<u8>(c) <<  8)        \
            | (static_cast<u8>(d) <<  0)        \
            )

    #define NW_G3D_GET_SIGNATURE0(sig) ((static_cast<u32>(sig) >> 24) & 0xFF)
    #define NW_G3D_GET_SIGNATURE1(sig) ((static_cast<u32>(sig) >> 16) & 0xFF)
    #define NW_G3D_GET_SIGNATURE2(sig) ((static_cast<u32>(sig) >>  8) & 0xFF)
    #define NW_G3D_GET_SIGNATURE3(sig) ((static_cast<u32>(sig) >>  0) & 0xFF)

#endif

#define NW_G3D_VERSION                  \
    NW_G3D_MAKE_U8X4_AS_U32(            \
        NW_G3D_VERSION_MAJOR,           \
        NW_G3D_VERSION_MINOR,           \
        NW_G3D_VERSION_MICRO,           \
        NW_G3D_VERSION_BINARYBUGFIX     \
    )

namespace nw { namespace g3d {

namespace ut {}
using namespace nw::g3d::ut;

namespace math {}
using namespace nw::g3d::math;

namespace fnd {}
using namespace nw::g3d::fnd;

namespace res {}
using namespace nw::g3d::res;

} } // namespace nw::g3d

#include <cafe/os/OSCore.h>

typedef u8 bit8;
typedef u16 bit16;
typedef u32 bit32;
typedef u64 bit64;

typedef unsigned int uint;

#if defined( __ghs__ )
#include <stdint.h>
#include <ppc_ghs.h>
#include <ppc_ps.h>
#else
typedef wchar_t char16;
#endif

#endif // NW_G3D_DEFS_H_
