#ifndef NW_G3D_CONFIG_H_
#define NW_G3D_CONFIG_H_

#if !defined(NW_DEBUG) && !defined(NW_DEVELOP) && !defined(NW_RELEASE)
#error "Build target is not defined."
#endif

#include <cstddef>
#include <cstdlib>
#include <new>

#define NW_G3D_CONFIG_USE_STATIC 1

#define NW_G3D_PERF_ENABLE 1
#define NW_G3D_PERF_LEVEL 0

#define NW_G3D_FORCE_PPC_SYNC 0

#define NW_G3D_MAX_SHADER_KEY 32

#include <nw/g3d/g3d_defs.h>

#include <sdk_ver.h>
#if NW_G3D_IS_GX2
#include <cafe/gx2.h>
#endif

#include <nw/g3d/g3d_assert.h>

#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )

#include <GL/glew.h>

#if defined(_DEBUG)
    #define NW_G3D_GL_ASSERT()                                                                         \
        do                                                                                             \
        {                                                                                              \
            GLenum err; (void)err;                                                                     \
            NW_G3D_ASSERTMSG(GL_NO_ERROR == (err = glGetError()), "GL Error: %s (0x%x)\n",             \
                GetGLErrorString(err), err);                                                  \
        } while (NW_G3D_STATIC_CONDITION(0))
#else
    #define NW_G3D_GL_ASSERT() ((void)0)
#endif

namespace {

#define TYPE_CASE(err) case err: return #err;

NW_G3D_FORCE_INLINE const char* GetGLErrorString(GLenum err)
{
    switch (err)
    {
        TYPE_CASE(GL_NO_ERROR);
        TYPE_CASE(GL_INVALID_ENUM);
        TYPE_CASE(GL_INVALID_VALUE);
        TYPE_CASE(GL_INVALID_OPERATION);
        TYPE_CASE(GL_STACK_OVERFLOW);
        TYPE_CASE(GL_STACK_UNDERFLOW);
        TYPE_CASE(GL_OUT_OF_MEMORY);
        TYPE_CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
    default:
        return "Unkown Error";
    }
}

#undef TYPE_CASE

}

#endif

#endif // NW_G3D_CONFIG_H_
