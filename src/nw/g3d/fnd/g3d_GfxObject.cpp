#include <nw/g3d/fnd/g3d_GfxObject.h>
#include <functional>
#include <algorithm>
#include <nw/g3d/fnd/g3d_GfxManage.h>
#include <nw/g3d/fnd/g3d_GX2Utility.h>
#include <nw/g3d/ut/g3d_Inlines.h>
#include <nw/g3d/ut/g3d_Flag.h>
#include <nw/g3d/math/g3d_MathCommon.h>

#include <bit>

namespace nw { namespace g3d { namespace fnd {

namespace {

enum RegSampler0
{
    NW_G3D_FLAG_VALUE_DECLARE( 0,  3, CLAMP_X),
    NW_G3D_FLAG_VALUE_DECLARE( 3,  3, CLAMP_Y),
    NW_G3D_FLAG_VALUE_DECLARE( 6,  3, CLAMP_Z),
    NW_G3D_FLAG_VALUE_DECLARE( 9,  2, XY_MAG_FILTER),
    NW_G3D_FLAG_VALUE_DECLARE(12,  2, XY_MIN_FILTER),
    NW_G3D_FLAG_VALUE_DECLARE(15,  2, Z_FILTER),
    NW_G3D_FLAG_VALUE_DECLARE(17,  2, MIP_FILTER),
    NW_G3D_FLAG_VALUE_DECLARE(19,  3, MAX_ANISO_RATIO),
    NW_G3D_FLAG_VALUE_DECLARE(22,  2, BORDER_COLOR_TYPE),
    NW_G3D_FLAG_VALUE_DECLARE(26,  3, DEPTH_COMPARE_FUNCTION)
};

enum RegSampler1
{
    NW_G3D_FLAG_VALUE_DECLARE( 0, 10, MIN_LOD),
    NW_G3D_FLAG_VALUE_DECLARE(10, 10, MAX_LOD),
    NW_G3D_FLAG_VALUE_DECLARE(20, 12, LOD_BIAS)
};

enum RegSampler2
{
    NW_G3D_FLAG_VALUE_DECLARE(30,  1, DEPTH_COMPARE_ENABLE)
};

enum InstCFlow1
{
    NW_G3D_FLAG_VALUE_DECLARE(10,  3, COUNT),
    NW_G3D_FLAG_VALUE_DECLARE(19,  1, COUNT_3)
};

enum InstVFetch0
{
    NW_G3D_FLAG_VALUE_DECLARE( 5,  2, FETCH_TYPE),
    NW_G3D_FLAG_VALUE_DECLARE( 8,  4, BUFFER_ID),
    NW_G3D_FLAG_VALUE_DECLARE(24,  2, SRC_SEL_X),
    NW_G3D_FLAG_VALUE_DECLARE(26,  6, MEGA_FETCH_COUNT)
};

enum InstVFetch1
{
    NW_G3D_FLAG_VALUE_DECLARE( 0,  8, SEMANTIC_ID),
    NW_G3D_FLAG_VALUE_DECLARE( 9,  3, DST_SEL_X),
    NW_G3D_FLAG_VALUE_DECLARE(12,  3, DST_SEL_Y),
    NW_G3D_FLAG_VALUE_DECLARE(15,  3, DST_SEL_Z),
    NW_G3D_FLAG_VALUE_DECLARE(18,  3, DST_SEL_W),
    NW_G3D_FLAG_VALUE_DECLARE(22,  6, DATA_FORMAT),
    NW_G3D_FLAG_VALUE_DECLARE(28,  2, NUM_FORMAT_ALL),
    NW_G3D_FLAG_VALUE_DECLARE(30,  1, FORMAT_COMP_ALL)
};

enum InstVFetch2
{
    NW_G3D_FLAG_VALUE_DECLARE( 0, 16, OFFSET),
    NW_G3D_FLAG_VALUE_DECLARE(16,  2, ENDIAN_SWAP)
};

NW_G3D_FORCE_INLINE
u32 ReadInst(const u32* pInst, int instIndex)
{
    return LoadRevU32(pInst + instIndex);
}

NW_G3D_FORCE_INLINE
void WriteInst(u32* pInst, int instIndex, u32 value)
{
    StoreRevU32(pInst + instIndex, value);
}

NW_G3D_INLINE
float U4_6ToFloat(u32 x)
{
    return FastCast<float>(static_cast<u16>(x)) / 64.0f;
}

NW_G3D_INLINE
float S5_6ToFloat(u32 x)
{
    return FastCast<float>(static_cast<s16>(std::bit_cast<s32, u32>(x << 20) >> 20)) / 64.0f;
}

#define GX2_SURFACE_FORMAT_T_R32_G32_B32_UINT       0x0000012f
#define GX2_SURFACE_FORMAT_T_R32_G32_B32_SINT       0x0000032f
#define GX2_SURFACE_FORMAT_T_R32_G32_B32_FLOAT      0x00000830

#define NW_G3D_DEF_FMT(attrib, surface, ...) \
    { static_cast<u8>(GX2_ATTRIB_FORMAT_##attrib), static_cast<u8>(GX2_SURFACE_FORMAT_##surface), __VA_ARGS__ }

const struct InternalFormat
{
    u8 attribFormat;
    u8 surfaceFormat;
    u8 fetchSize;
    u8 endianSwap;
    GX2CompSel compSel;
} s_InternalFormat[] = {
    NW_G3D_DEF_FMT(8_UNORM,             TC_R8_UNORM,                0,  0, GX2_COMP_SEL_X001),
    NW_G3D_DEF_FMT(4_4_UNORM,           T_R4_G4_UNORM,              0,  0, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(16_UNORM,            TCD_R16_UNORM,              1,  1, GX2_COMP_SEL_X001),
    NW_G3D_DEF_FMT(16_FLOAT,            TC_R16_FLOAT,               1,  1, GX2_COMP_SEL_X001),
    NW_G3D_DEF_FMT(8_8_UNORM,           TC_R8_G8_UNORM,             1,  0, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(32_UINT,             TC_R32_UINT,                3,  2, GX2_COMP_SEL_X001),
    NW_G3D_DEF_FMT(32_FLOAT,            TCD_R32_FLOAT,              3,  2, GX2_COMP_SEL_X001),
    NW_G3D_DEF_FMT(16_16_UNORM,         TC_R16_G16_UNORM,           3,  1, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(16_16_FLOAT,         TC_R16_G16_FLOAT,           3,  1, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(10_11_11_FLOAT,      TC_R11_G11_B10_FLOAT,       3,  2, GX2_COMP_SEL_XYZ1),
    NW_G3D_DEF_FMT(8_8_8_8_UNORM,       TCS_R8_G8_B8_A8_UNORM,      3,  0, GX2_COMP_SEL_XYZW),
    NW_G3D_DEF_FMT(10_10_10_2_UNORM,    TCS_R10_G10_B10_A2_UNORM,   3,  2, GX2_COMP_SEL_XYZW),
    NW_G3D_DEF_FMT(32_32_UINT,          TC_R32_G32_UINT,            7,  2, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(32_32_FLOAT,         TC_R32_G32_FLOAT,           7,  2, GX2_COMP_SEL_XY01),
    NW_G3D_DEF_FMT(16_16_16_16_UNORM,   TC_R16_G16_B16_A16_UNORM,   7,  1, GX2_COMP_SEL_XYZW),
    NW_G3D_DEF_FMT(16_16_16_16_FLOAT,   TC_R16_G16_B16_A16_FLOAT,   7,  1, GX2_COMP_SEL_XYZW),
    NW_G3D_DEF_FMT(32_32_32_UINT,       T_R32_G32_B32_UINT,         11, 2, GX2_COMP_SEL_XYZ1),
    NW_G3D_DEF_FMT(32_32_32_FLOAT,      T_R32_G32_B32_FLOAT,        11, 2, GX2_COMP_SEL_XYZ1),
    NW_G3D_DEF_FMT(32_32_32_32_UINT,    TC_R32_G32_B32_A32_UINT,    15, 2, GX2_COMP_SEL_XYZW),
    NW_G3D_DEF_FMT(32_32_32_32_FLOAT,   TC_R32_G32_B32_A32_FLOAT,   15, 2, GX2_COMP_SEL_XYZW)
};

#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )

struct GLAttribInternal
{
    u8 attribFormat;    // low byte only
    u8 surfaceFormat;   // low byte only
    u8 size;            // number of component
    u8 reserved;
    u32 type;           // float or unsigned integer
} s_GLAttribInternal[] = {
    NW_G3D_DEF_FMT(8_UNORM,             TC_R8_UNORM,                1,  0,  GL_UNSIGNED_BYTE),
    NW_G3D_DEF_FMT(4_4_UNORM,           T_R4_G4_UNORM,              2,  0,  GL_UNSIGNED_BYTE),
    NW_G3D_DEF_FMT(16_UNORM,            TCD_R16_UNORM,              1,  0,  GL_UNSIGNED_SHORT),
    NW_G3D_DEF_FMT(16_FLOAT,            TC_R16_FLOAT,               1,  0,  GL_HALF_FLOAT),
    NW_G3D_DEF_FMT(8_8_UNORM,           TC_R8_G8_UNORM,             2,  0,  GL_UNSIGNED_BYTE),
    NW_G3D_DEF_FMT(32_UINT,             TC_R32_UINT,                1,  0,  GL_UNSIGNED_INT),
    NW_G3D_DEF_FMT(32_FLOAT,            TCD_R32_FLOAT,              1,  0,  GL_FLOAT),
    NW_G3D_DEF_FMT(16_16_UNORM,         TC_R16_G16_UNORM,           2,  0,  GL_UNSIGNED_SHORT),
    NW_G3D_DEF_FMT(16_16_FLOAT,         TC_R16_G16_FLOAT,           2,  0,  GL_HALF_FLOAT),
    NW_G3D_DEF_FMT(10_11_11_FLOAT,      TC_R11_G11_B10_FLOAT,       3,  0,  GL_UNSIGNED_INT_10F_11F_11F_REV),
    NW_G3D_DEF_FMT(8_8_8_8_UNORM,       TCS_R8_G8_B8_A8_UNORM,      4,  0,  GL_UNSIGNED_BYTE),
    NW_G3D_DEF_FMT(10_10_10_2_UNORM,    TCS_R10_G10_B10_A2_UNORM,   4,  0,  GL_UNSIGNED_INT_2_10_10_10_REV),
    NW_G3D_DEF_FMT(32_32_UINT,          TC_R32_G32_UINT,            2,  0,  GL_UNSIGNED_INT),
    NW_G3D_DEF_FMT(32_32_FLOAT,         TC_R32_G32_FLOAT,           2,  0,  GL_FLOAT),
    NW_G3D_DEF_FMT(16_16_16_16_UNORM,   TC_R16_G16_B16_A16_UNORM,   4,  0,  GL_UNSIGNED_SHORT),
    NW_G3D_DEF_FMT(16_16_16_16_FLOAT,   TC_R16_G16_B16_A16_FLOAT,   4,  0,  GL_HALF_FLOAT),
    NW_G3D_DEF_FMT(32_32_32_UINT,       T_R32_G32_B32_UINT,         3,  0,  GL_UNSIGNED_INT),
    NW_G3D_DEF_FMT(32_32_32_FLOAT,      T_R32_G32_B32_FLOAT,        3,  0,  GL_FLOAT),
    NW_G3D_DEF_FMT(32_32_32_32_UINT,    TC_R32_G32_B32_A32_UINT,    4,  0,  GL_UNSIGNED_INT),
    NW_G3D_DEF_FMT(32_32_32_32_FLOAT,   TC_R32_G32_B32_A32_FLOAT,   4,  0,  GL_FLOAT)
};

struct AttribInternalCmp /* : public std::binary_function<GLAttribInternal, u8, bool> */
{
    bool operator ()(const GLAttribInternal& lhs, u8 rhs) const
    {
        return lhs.surfaceFormat == rhs;
    }
};

const GLAttribInternal& FindGLAttribInternal(u8 surfaceFormat)
{

    return *std::find_if(
        s_GLAttribInternal,
        s_GLAttribInternal + (sizeof(s_GLAttribInternal) / sizeof(GLAttribInternal) - 1),
        std::bind(AttribInternalCmp(), std::placeholders::_1, surfaceFormat));
}

struct TexImageArg
{
    GLenum target;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
    GLboolean compressed;
    u32 minWidth;
    u32 minHeight;
    u32 minDepth;
};

void InitTexImageArg(TexImageArg* pArg, const GX2Surface& surface, int arrayLength)
{
    NW_G3D_TABLE_FIELD GLenum s_tblTarget[] = {
        GL_TEXTURE_1D,
        GL_TEXTURE_2D,
        GL_TEXTURE_3D,
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_1D_ARRAY,
        GL_TEXTURE_2D_ARRAY,
        GL_TEXTURE_2D_MULTISAMPLE,
        GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    };
    pArg->target = s_tblTarget[surface.dim];
    if (surface.dim == GX2_SURFACE_DIM_CUBE && arrayLength > 0)
    {
        pArg->target = GL_TEXTURE_CUBE_MAP_ARRAY;
    }

    GLSurfaceFormat glFormat =
        (surface.use == GX2_SURFACE_USE_DEPTH_BUFFER ||
        surface.use == GX2_SURFACE_USE_DEPTH_BUFFER_TEXTURE) ?
        FindGLDepthFormat(surface.format) : FindGLFormat(surface.format);
    pArg->internalFormat = glFormat.internalFormat;
    pArg->format = glFormat.format;
    pArg->type = glFormat.type;

    pArg->compressed = IsCompressed(surface.format);

    pArg->minWidth = pArg->minHeight = pArg->minDepth = 1;
    if (surface.dim == GX2_SURFACE_DIM_1D_ARRAY)
    {
        pArg->minHeight = surface.height;
    }
    else if (surface.dim >= GX2_SURFACE_DIM_CUBE)
    {
        pArg->minDepth = surface.depth;
    }
}

void GL_TexImage(
    GLuint hTexture, GLenum target, GLint mipLevel, GLint internalFormat,
    GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type,
    GLboolean compressed, GLsizei imageSize, const GLvoid* data
)
{
    constexpr int CUBEMAP_SIZE = 6;
    constexpr GLint BORDER_SIZE = 0;

    glBindTexture(target, hTexture);

    switch (target)
    {
    case GL_TEXTURE_1D:
        if (compressed)
        {
            glCompressedTexImage1D(target, mipLevel, internalFormat,
                width, BORDER_SIZE, imageSize, data);
        }
        else
        {
            glTexImage1D(target, mipLevel, internalFormat,
                width, BORDER_SIZE, format, type, data);
        }
        break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_1D_ARRAY:
        if (compressed)
        {
            glCompressedTexImage2D(target, mipLevel, internalFormat,
                width, height, BORDER_SIZE, imageSize, data);
        }
        else
        {
            glTexImage2D(target, mipLevel, internalFormat,
                width, height, BORDER_SIZE, format, type, data);
        }
        break;
    case GL_TEXTURE_3D:
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        if (compressed)
        {
            glCompressedTexImage3D(target, mipLevel, internalFormat,
                width, height, depth, BORDER_SIZE, imageSize, data);
        }
        else
        {
            glTexImage3D(target, mipLevel, internalFormat,
                width, height, depth, BORDER_SIZE, format, type, data);
        }
        break;
    case GL_TEXTURE_CUBE_MAP:
        if (compressed)
        {
            const GLvoid *pData = data;
            const GLsizei faceSize = imageSize / CUBEMAP_SIZE;
            for (int face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
                face < GL_TEXTURE_CUBE_MAP_POSITIVE_X + CUBEMAP_SIZE; ++face)
            {
                glCompressedTexImage2D(face, mipLevel, internalFormat,
                    width, height, BORDER_SIZE, faceSize, pData);
                if (pData)
                {
                    pData = AddOffset(pData, faceSize);
                }
            }
        }
        else
        {
            const GLvoid *pData = data;
            const GLsizei faceSize = imageSize / CUBEMAP_SIZE;
            for (int face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
                face < GL_TEXTURE_CUBE_MAP_POSITIVE_X + CUBEMAP_SIZE; ++face)
            {
                glTexImage2D(face, mipLevel, internalFormat,
                    width, height, BORDER_SIZE, format, type, pData);
                if (pData)
                {
                    pData = AddOffset(pData, faceSize);
                }
            }
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE:
        NW_G3D_NOT_IMPLEMENTED();
        break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        NW_G3D_NOT_IMPLEMENTED();
        break;
    default:
        NW_G3D_NOT_SUPPORTED();
    }
}

void GL_TexSubImage(
    GLuint hTexture, GLenum target, GLint mipLevel, GLint internalFormat,
    GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type,
    GLboolean compressed, GLsizei imageSize, const GLvoid* data
)
{
    constexpr int CUBEMAP_SIZE = 6;
    constexpr GLint X_OFFSET = 0;
    constexpr GLint Y_OFFSET = 0;
    constexpr GLint Z_OFFSET = 0;

    glBindTexture(target, hTexture);

    switch (target)
    {
    case GL_TEXTURE_1D:
        if (compressed)
        {
            glCompressedTexSubImage1D(target, mipLevel, X_OFFSET,
                width, internalFormat, imageSize, data);
        }
        else
        {
            glTexSubImage1D(target, mipLevel, X_OFFSET,
                width, format, type, data);
        }
        break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_1D_ARRAY:
        if (compressed)
        {
            glCompressedTexSubImage2D(target, mipLevel, X_OFFSET, Y_OFFSET,
                width, height, internalFormat, imageSize, data);
        }
        else
        {
            glTexSubImage2D(target, mipLevel, X_OFFSET, Y_OFFSET,
                width, height, format, type, data);
        }
        break;
    case GL_TEXTURE_3D:
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        if (compressed)
        {
            glCompressedTexSubImage3D(target,
                mipLevel, X_OFFSET, Y_OFFSET, Z_OFFSET,
                width, height, depth, internalFormat, imageSize, data);
        }
        else
        {
            glTexSubImage3D(target,
                mipLevel, X_OFFSET, Y_OFFSET, Z_OFFSET,
                width, height, depth, format, type, data);
        }
        break;
    case GL_TEXTURE_CUBE_MAP:
        if (compressed)
        {
            const GLvoid *pData = data;
            const GLsizei faceSize = imageSize / CUBEMAP_SIZE;
            for (int face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
                face < GL_TEXTURE_CUBE_MAP_POSITIVE_X + CUBEMAP_SIZE; ++face)
            {
                glCompressedTexSubImage2D(face, mipLevel, X_OFFSET, Y_OFFSET,
                    width, height, internalFormat, faceSize, pData);
                if (pData)
                {
                    pData = AddOffset(pData, faceSize);
                }
            }
        }
        else
        {
            const GLvoid *pData = data;
            const GLsizei faceSize = imageSize / CUBEMAP_SIZE;
            for (int face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
                face < GL_TEXTURE_CUBE_MAP_POSITIVE_X + CUBEMAP_SIZE; ++face)
            {
                glTexSubImage2D(face, mipLevel, X_OFFSET, Y_OFFSET,
                    width, height, format, type, pData);
                if (pData)
                {
                    pData = AddOffset(pData, faceSize);
                }
            }
        }
        break;
    default:
        NW_G3D_NOT_SUPPORTED();
    }
}

#endif

enum
{
    ATTRIB_BIT_INT          = 0x100,
    ATTRIB_BIT_SIGNED       = 0x200,
    ATTRIB_BIT_FLOAT        = 0x800,
    ATTRIB_FMT_NORMALIZED   = 0,
    ATTRIB_FMT_INT          = 1,
    ATTRIB_FMT_SCALED       = 2
};

}

GfxBuffer::GfxBuffer()
{
    memset(static_cast<GfxBuffer_t*>(this), 0, sizeof(GfxBuffer_t));
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    handle = 0;
    bufferType = BUFFER_TYPE_INVALID;
#endif
}

void GfxBuffer::Setup()
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle == 0);
    glGenBuffers(1, &handle);
    NW_G3D_GL_ASSERT();
    UpdateRegs();
#endif
}

void GfxBuffer::Cleanup()
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    if (handle != 0)
    {
        glDeleteBuffers(1, &handle);
        handle = 0;
        bufferType = BUFFER_TYPE_INVALID;
        NW_G3D_GL_ASSERT();
    }
#endif
}

void GfxBuffer::UpdateRegs()
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);
    switch (bufferType)
    {
    case BUFFER_TYPE_ELEMENT_ARRAY:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * numBuffering, NULL, GL_DYNAMIC_DRAW);
        break;
    case BUFFER_TYPE_ARRAY:
        glBindBuffer(GL_ARRAY_BUFFER, handle);
        glBufferData(GL_ARRAY_BUFFER, size * numBuffering, NULL, GL_DYNAMIC_DRAW);
        break;
    case BUFFER_TYPE_UNIFORM:
        glBindBuffer(GL_UNIFORM_BUFFER, handle);
        glBufferData(GL_UNIFORM_BUFFER, size * numBuffering, NULL, GL_DYNAMIC_DRAW);
        break;
    default:
        NW_G3D_ASSERT(bufferType == BUFFER_TYPE_INVALID);
    }
#endif
}

void GfxBuffer::DCFlush(int bufferIndex /*= 0*/) const
{
    NW_G3D_ASSERT_INDEX_BOUNDS(bufferIndex, numBuffering);
#if NW_G3D_IS_GX2
#if NW_G3D_FORCE_PPC_SYNC
    DCFlushRange(GetData(bufferIndex), size);
#else
    DCFlushRangeNoSync(GetData(bufferIndex), size);
#endif // NW_G3D_FORCE_PPC_SYNC
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);
    switch (bufferType)
    {
    case BUFFER_TYPE_ELEMENT_ARRAY:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, size * bufferIndex, size, GetData(bufferIndex));
        break;
    case BUFFER_TYPE_ARRAY:
        glBindBuffer(GL_ARRAY_BUFFER, handle);
        if (stride > 0)
        {
            NW_G3D_ASSERT(bufferIndex == 0);
            glBufferSubData(GL_ARRAY_BUFFER, 0, size, GetData(0));
        }
        else
        {
            glBufferSubData(GL_ARRAY_BUFFER, size * bufferIndex, size, GetData(bufferIndex));
        }
        break;
    case BUFFER_TYPE_UNIFORM:
        glBindBuffer(GL_UNIFORM_BUFFER, handle);
        glBufferSubData(GL_UNIFORM_BUFFER, size * bufferIndex, size, GetData(bufferIndex));
        break;
    }
    NW_G3D_GL_ASSERT();
#else
    NW_G3D_UNUSED(bufferIndex);
#endif
}

void GfxBuffer::LoadIndices() const
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);
    NW_G3D_ASSERT(bufferType == BUFFER_TYPE_INVALID || bufferType == BUFFER_TYPE_ELEMENT_ARRAY);
    if (bufferType == BUFFER_TYPE_INVALID)
    {
        const_cast<GfxBuffer*>(this)->bufferType = BUFFER_TYPE_ELEMENT_ARRAY;
        const_cast<GfxBuffer*>(this)->UpdateRegs();
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size * numBuffering, GetData());
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
    }
    NW_G3D_GL_ASSERT();
#endif
}

void GfxBuffer::LoadVertices(u32 slot) const
{
#if NW_G3D_IS_GX2
    GX2SetAttribBuffer(slot, size, stride, pData);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);
    NW_G3D_ASSERT(bufferType == BUFFER_TYPE_INVALID || bufferType == BUFFER_TYPE_ARRAY);
    NW_G3D_UNUSED(slot);
    if (bufferType == BUFFER_TYPE_INVALID)
    {
        const_cast<GfxBuffer*>(this)->bufferType = BUFFER_TYPE_ARRAY;
        const_cast<GfxBuffer*>(this)->UpdateRegs();
        glBufferSubData(GL_ARRAY_BUFFER, 0, size * numBuffering, GetData());
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, handle);
    }
    NW_G3D_GL_ASSERT();
#else
    NW_G3D_UNUSED(slot);
#endif
}

void GfxBuffer::LoadVertexUniforms(u32 location, int bufferIndex /*= 0*/) const
{
    NW_G3D_ASSERT_INDEX_BOUNDS(bufferIndex, numBuffering);
    NW_G3D_ASSERT(size > 0);
    NW_G3D_ASSERT(location != 0xFFFFFFFF);
#if NW_G3D_IS_GX2
    GX2SetVertexUniformBlock(location, size, GetData(bufferIndex));
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);
    NW_G3D_ASSERT(bufferType == BUFFER_TYPE_INVALID || bufferType == BUFFER_TYPE_UNIFORM);
    if (bufferType == BUFFER_TYPE_INVALID)
    {
        const_cast<GfxBuffer*>(this)->bufferType = BUFFER_TYPE_UNIFORM;
        const_cast<GfxBuffer*>(this)->UpdateRegs();
        glBufferSubData(GL_UNIFORM_BUFFER, 0, size * numBuffering, GetData());
    }
    NW_G3D_GL_ASSERT();
    glBindBufferRange(GL_UNIFORM_BUFFER, location, handle, size * bufferIndex, size);
    NW_G3D_GL_ASSERT();
#else
    NW_G3D_UNUSED(location);
    NW_G3D_UNUSED(bufferIndex);
#endif
}

void GfxBuffer::LoadGeometryUniforms(u32 location, int bufferIndex /*= 0*/) const
{
    NW_G3D_ASSERT_INDEX_BOUNDS(bufferIndex, numBuffering);
    NW_G3D_ASSERT(size > 0);
    NW_G3D_ASSERT(location != 0xFFFFFFFF);
#if NW_G3D_IS_GX2
    GX2SetGeometryUniformBlock(location, size, GetData(bufferIndex));
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    LoadVertexUniforms(location, bufferIndex);
#else
    NW_G3D_UNUSED(location);
    NW_G3D_UNUSED(bufferIndex);
#endif
}

void GfxBuffer::LoadFragmentUniforms(u32 location, int bufferIndex /*= 0*/) const
{
    NW_G3D_ASSERT_INDEX_BOUNDS(bufferIndex, numBuffering);
    NW_G3D_ASSERT(size > 0);
    NW_G3D_ASSERT(location != 0xFFFFFFFF);
#if NW_G3D_IS_GX2
    GX2SetPixelUniformBlock(location, size, GetData(bufferIndex));
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    LoadVertexUniforms(location, bufferIndex);
#else
    NW_G3D_UNUSED(location);
    NW_G3D_UNUSED(bufferIndex);
#endif
}

void GfxBuffer::SetData(void* pData, u32 size, int bufferingCount /*= 1*/)
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    NW_G3D_ASSERT(bufferingCount > 0);
    this->pData = size > 0 ? pData : NULL;
    this->size = size;
    this->numBuffering = static_cast<u16>(bufferingCount);
#else
    NW_G3D_UNUSED(pData);
    NW_G3D_UNUSED(size);
    NW_G3D_UNUSED(bufferingCount);
#endif
}

void* GfxBuffer::GetData(int bufferIndex /*= 0*/)
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    NW_G3D_ASSERT_INDEX_BOUNDS( bufferIndex, numBuffering );
    return AddOffset(pData, size * bufferIndex);
#else
    NW_G3D_UNUSED(bufferIndex);
    return NULL;
#endif
}

const void* GfxBuffer::GetData(int bufferIndex /*= 0*/) const
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    NW_G3D_ASSERT_INDEX_BOUNDS( bufferIndex, numBuffering );
    return AddOffset(pData, size * bufferIndex);
#else
    NW_G3D_UNUSED(bufferIndex);
    return NULL;
#endif
}
void GfxSampler::Setup()
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle == 0);
    glGenSamplers(1, &handle);
    NW_G3D_GL_ASSERT();
    UpdateRegs();
#endif
}

void GfxSampler::Cleanup()
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    if (handle != 0)
    {
        glDeleteSamplers(1, &handle);
        handle = 0;
        NW_G3D_GL_ASSERT();
    }
#endif
}

void GfxSampler::UpdateRegs()
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);

    // clamp
    NW_G3D_TABLE_FIELD u32 tblClamp[] = {
        GL_REPEAT,
        GL_MIRRORED_REPEAT,
        GL_CLAMP_TO_EDGE,
        GL_MIRROR_CLAMP_TO_EDGE_EXT,
        GL_CLAMP,
        GL_MIRROR_CLAMP_EXT,
        GL_CLAMP_TO_BORDER,
        GL_MIRROR_CLAMP_TO_BORDER_EXT
    };
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, tblClamp[GetClampX()]);
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, tblClamp[GetClampY()]);
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_R, tblClamp[GetClampZ()]);

    GX2TexBorderType borderType = GetBorderType();
    if (borderType < GX2_TEX_BORDER_USE_REGISTER)
    {
        NW_G3D_TABLE_FIELD float tblBorderColor[][4] = {
            { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f, 1.0f }
        };
        glSamplerParameterfv(handle, GL_TEXTURE_BORDER_COLOR, tblBorderColor[borderType]);
    }

    // filter
    NW_G3D_TABLE_FIELD u32 tblFilter[] = {
        GL_NEAREST,
        GL_LINEAR,
        GL_NEAREST_MIPMAP_NEAREST,
        GL_LINEAR_MIPMAP_NEAREST,
        GL_NEAREST_MIPMAP_LINEAR,
        GL_LINEAR_MIPMAP_LINEAR
    };
    NW_G3D_TABLE_FIELD u32 tblAniso[] = {
        1, 2, 4, 8, 16
    };
    glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, tblFilter[GetMagFilter()]);
    glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER,
        tblFilter[GetMipFilter() << 1 | GetMinFilter()]);
    glSamplerParameteri(handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, tblAniso[GetMaxAniso()]);

    // LOD
    glSamplerParameterf(handle, GL_TEXTURE_MIN_LOD, GetMinLOD());
    glSamplerParameterf(handle, GL_TEXTURE_MAX_LOD, GetMaxLOD());
    glSamplerParameterf(handle, GL_TEXTURE_LOD_BIAS, GetLODBias());

    // compare
    glSamplerParameteri(handle, GL_TEXTURE_COMPARE_MODE, GL_NONE);

    NW_G3D_GL_ASSERT();
#endif
}

void GfxSampler::LoadVertexSampler(u32 unit) const
{
#if NW_G3D_IS_GX2
    GX2SetVertexSampler(GetGX2Sampler(), unit);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);
    glBindSampler(unit, handle);
    NW_G3D_GL_ASSERT();
    glUniform1i(unit, unit);
    NW_G3D_GL_ASSERT();
#else
    NW_G3D_UNUSED(unit);
#endif
}

void GfxSampler::LoadGeometrySampler(u32 unit) const
{
#if NW_G3D_IS_GX2
    GX2SetGeometrySampler(GetGX2Sampler(), unit);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    LoadVertexSampler(unit);
#else
    NW_G3D_UNUSED(unit);
#endif
}

void GfxSampler::LoadFragmentSampler(u32 unit) const
{
#if NW_G3D_IS_GX2
    GX2SetPixelSampler(GetGX2Sampler(), unit);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    LoadVertexSampler(unit);
#else
    NW_G3D_UNUSED(unit);
#endif
}

GX2TexClamp GfxSampler::GetClampX() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], CLAMP_X, GX2TexClamp);
}

GX2TexClamp GfxSampler::GetClampY() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], CLAMP_Y, GX2TexClamp);
}

GX2TexClamp GfxSampler::GetClampZ() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], CLAMP_Z, GX2TexClamp);
}

GX2TexBorderType GfxSampler::GetBorderType() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], BORDER_COLOR_TYPE, GX2TexBorderType);
}

GX2TexXYFilterType GfxSampler::GetMagFilter() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], XY_MAG_FILTER, GX2TexXYFilterType);
}

GX2TexXYFilterType GfxSampler::GetMinFilter() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], XY_MIN_FILTER, GX2TexXYFilterType);
}

GX2TexMipFilterType GfxSampler::GetMipFilter() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], MIP_FILTER, GX2TexMipFilterType);
}

GX2TexAnisoRatio GfxSampler::GetMaxAniso() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0], MAX_ANISO_RATIO, GX2TexAnisoRatio);
}

float GfxSampler::GetMinLOD() const
{
    u32 val = NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[1], MIN_LOD, u32);
    return U4_6ToFloat(val);
}

float GfxSampler::GetMaxLOD() const
{
    u32 val = NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[1], MAX_LOD, u32);
    return U4_6ToFloat(val);
}

float GfxSampler::GetLODBias() const
{
    u32 val = NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[1], LOD_BIAS, u32);
    return S5_6ToFloat(val);
}

GX2Boolean GfxSampler::GetCompareEnable() const
{
    return GX2_FALSE;
}

GX2CompareFunction GfxSampler::GetCompareFunc() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2Sampler.samplerReg[0],
        DEPTH_COMPARE_FUNCTION, GX2CompareFunction);
}

void GfxTexture::Setup()
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle == 0);
    glGenTextures(1, &handle);
    NW_G3D_GL_ASSERT();
#endif
    UpdateRegs();
}

void GfxTexture::Cleanup()
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    if (handle)
    {
        glDeleteTextures(1, &handle);
        handle = 0;
        NW_G3D_GL_ASSERT();
    }
#endif
}

void GfxTexture::UpdateRegs()
{
#if NW_G3D_IS_GX2
    GX2InitTextureRegs(GetGX2Texture());
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);

    const GX2Texture& texture = *GetGX2Texture();
    const GX2Surface& surface = texture.surface;

    TexImageArg arg;
    InitTexImageArg(&arg, surface, arrayLength);

    for (u32 mipLevel = 0, numMips = surface.numMips; mipLevel < numMips; ++mipLevel)
    {
        GLsizei width = std::max<u32>(arg.minWidth, surface.width >> mipLevel);
        GLsizei height = std::max<u32>(arg.minHeight, surface.height >> mipLevel);
        GLsizei depth = std::max<u32>(arg.minDepth, surface.depth >> mipLevel);
        GLsizei imageSize = CalcImageSize(surface.format, width, height, depth);
        GL_TexImage(handle, arg.target, mipLevel, arg.internalFormat, width, height, depth,
            arg.format, arg.type, arg.compressed, imageSize, NULL);
    }

    NW_G3D_TABLE_FIELD GLenum tblSwizzle[] = {
        GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_ZERO, GL_ONE
    };
    GX2CompSel compSel = texture.compSel;
    GLint swizzle[] = {
        static_cast<GLint>(tblSwizzle[GX2_GET_COMPONENT_X_R(compSel)]),
        static_cast<GLint>(tblSwizzle[GX2_GET_COMPONENT_Y_G(compSel)]),
        static_cast<GLint>(tblSwizzle[GX2_GET_COMPONENT_Z_B(compSel)]),
        static_cast<GLint>(tblSwizzle[GX2_GET_COMPONENT_W_A(compSel)])
    };

    glBindTexture(arg.target, handle);

    glTexParameteri(arg.target, GL_TEXTURE_BASE_LEVEL, texture.viewFirstMip);
    glTexParameteri(arg.target, GL_TEXTURE_MAX_LEVEL,
        texture.viewFirstMip + texture.viewNumMips - 1);
    glTexParameteriv(arg.target, GL_TEXTURE_SWIZZLE_RGBA, swizzle);

    NW_G3D_GL_ASSERT();
#endif
}

void GfxTexture::DCFlush() const
{
#if NW_G3D_IS_GX2
#if NW_G3D_FORCE_PPC_SYNC
    DCFlushRange(GetBasePtr(), GetBaseSize());
    if (GetMipPtr())
    {
        DCFlushRange(GetMipPtr(), GetMipSize());
    }
#else
    DCFlushRangeNoSync(GetBasePtr(), GetBaseSize());
    if (GetMipPtr())
    {
        DCFlushRangeNoSync(GetMipPtr(), GetMipSize());
    }
#endif // NW_G3D_FORCE_PPC_SYNC
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);

    const GX2Texture& texture = *GetGX2Texture();
    const GX2Surface& surface = texture.surface;

    TexImageArg arg;
    InitTexImageArg(&arg, surface, arrayLength);

    for (u32 mipLevel = 0, numMips = surface.numMips; mipLevel < numMips; ++mipLevel)
    {
        GLsizei width = std::max<u32>(arg.minWidth, surface.width >> mipLevel);
        GLsizei height = std::max<u32>(arg.minHeight, surface.height >> mipLevel);
        GLsizei depth = std::max<u32>(arg.minDepth, surface.depth >> mipLevel);
        GLsizei imageSize = CalcImageSize(surface.format, width, height, depth);
        const GLvoid* imagePtr = GetImagePtr(mipLevel);
        GL_TexSubImage(handle, arg.target, mipLevel, arg.internalFormat, width, height, depth,
            arg.format, arg.type, arg.compressed, imageSize, imagePtr);
    }

    NW_G3D_GL_ASSERT();
#endif
}

void GfxTexture::SetImagePtrs(void* basePtr, void* mipPtr)
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    GX2Texture* pTexture = GetGX2Texture();
    pTexture->surface.imagePtr = basePtr;
    if (pTexture->surface.numMips > 1 && mipPtr == NULL && basePtr) {
        pTexture->surface.mipPtr = AddOffset(basePtr, pTexture->surface.mipOffset[0]);
    } else {
        pTexture->surface.mipPtr = mipPtr;
    }
#else
    NW_G3D_UNUSED(basePtr);
    NW_G3D_UNUSED(mipPtr);
    gx2Texture.surface.imagePtr = 0;
    gx2Texture.surface.mipPtr = 0;
#endif
}

const void* GfxTexture::GetBasePtr() const
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    return GetGX2Texture()->surface.imagePtr;
#else
    return NULL;
#endif
}

const void* GfxTexture::GetMipPtr() const
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    return GetGX2Texture()->surface.mipPtr;
#else
    return NULL;
#endif
}

const void* GfxTexture::GetImagePtr(int mipLevel) const
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    return fnd::GetImagePtr(GetGX2Texture()->surface, mipLevel);
#else
    NW_G3D_UNUSED(mipLevel);
    return NULL;
#endif
}

void GfxTexture::LoadVertexTexture(u32 unit) const
{
#if NW_G3D_IS_GX2
    GX2SetVertexTexture(GetGX2Texture(), unit);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);

    NW_G3D_TABLE_FIELD u32 tblDimension[] = {
        GL_TEXTURE_1D,
        GL_TEXTURE_2D,
        GL_TEXTURE_3D,
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_1D_ARRAY,
        GL_TEXTURE_2D_ARRAY,
        GL_TEXTURE_2D_MULTISAMPLE,
        GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    };
    u32 dimension = tblDimension[GetDimension()];
    if (GetDimension() == GX2_SURFACE_DIM_CUBE &&
        arrayLength > 0)
    {
        dimension = GL_TEXTURE_CUBE_MAP_ARRAY;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(dimension, handle);

    NW_G3D_GL_ASSERT();
#else
    NW_G3D_UNUSED(unit);
#endif
}

void GfxTexture::LoadGeometryTexture(u32 unit) const
{
#if NW_G3D_IS_GX2
    GX2SetGeometryTexture(GetGX2Texture(), unit);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    LoadVertexTexture(unit);
#else
    NW_G3D_UNUSED(unit);
#endif
}

void GfxTexture::LoadFragmentTexture(u32 unit) const
{
#if NW_G3D_IS_GX2
    GX2SetPixelTexture(GetGX2Texture(), unit);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    LoadVertexTexture(unit);
#else
    NW_G3D_UNUSED(unit);
#endif
}

GfxFetchShader::GfxFetchShader()
{
    memset(this, 0, sizeof(*this));
}

void GfxFetchShader::Setup()
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle == 0);
    glGenVertexArrays(1, &handle);
    NW_G3D_GL_ASSERT();
    UpdateRegs();
#endif
}

void GfxFetchShader::Cleanup()
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    if (handle)
    {
        glDeleteVertexArrays(1, &handle);
        handle = 0;
        NW_G3D_GL_ASSERT();
    }
#endif
}

void GfxFetchShader::UpdateRegs()
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    bit32 fixedAttribs = 0;
    for (u32 idxAttrib = 0; idxAttrib < gx2FetchShader.numAttribs; ++idxAttrib)
    {
        const GfxBuffer* pBuffer = GetVertexBuffer(idxAttrib);
        NW_G3D_ASSERT_NOT_NULL(pBuffer);
        if (pBuffer->stride == 0)
        {
            fixedAttribs |= 0x1 << idxAttrib;
        }
    }
    gx2FetchShader._regs[0] = fixedAttribs;
#endif
}

void GfxFetchShader::DCFlush() const
{
#if NW_G3D_IS_GX2
#if NW_G3D_FORCE_PPC_SYNC
    DCFlushRange(GetShaderPtr(), GetShaderSize());
#else
    DCFlushRangeNoSync(GetShaderPtr(), GetShaderSize());
#endif // NW_G3D_FORCE_PPC_SYNC
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);
    LoadVertexAttribArray();
    NW_G3D_GL_ASSERT();
#endif
}

void GfxFetchShader::CalcSize()
{
    u32 sizeCFInst = static_cast<u32>((Align(gx2FetchShader.numAttribs, MAX_INST_PER_FETCH_CLAUSE) /
        MAX_INST_PER_FETCH_CLAUSE + 1) * CF_INST_SIZE);
    ofsVFInst = static_cast<u32>(Align(sizeCFInst, FETCH_INST_ALIGNMENT));
    gx2FetchShader.shaderSize = ofsVFInst + gx2FetchShader.numAttribs * VF_INST_SIZE;
}

void GfxFetchShader::Load() const
{
#if NW_G3D_IS_GX2
    GX2SetFetchShader(GetGX2FetchShader());
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_ASSERT(handle != 0);
    glBindVertexArray(handle);
    LoadVertexAttribValue();
    NW_G3D_GL_ASSERT();
#endif
}

void GfxFetchShader::SetVertexBuffer(int attribIndex, const GfxBuffer* pBuffer)
{
    u32* pVFetchInst = GetVFInst(attribIndex);
    pVFetchInst[3] = reinterpret_cast<u32>(pBuffer);
}

const GfxBuffer* GfxFetchShader::GetVertexBuffer(int attribIndex) const
{
    const u32* pVFetchInst = GetVFInst(attribIndex);
    return reinterpret_cast<const GfxBuffer*>(pVFetchInst[3]);
}

void GfxFetchShader::SetShaderPtr(void* ptr)
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
#if NW_G3D_IS_HOST_CAFE
    NW_G3D_ASSERT_ADDR_ALIGNMENT(ptr, SHADER_ALIGNMENT);
#endif // NW_G3D_IS_HOST_CAFE
    GetGX2FetchShader()->shaderPtr = ptr;
#else
    NW_G3D_UNUSED(ptr);
    gx2FetchShader.shaderPtr = 0;
#endif
}

void* GfxFetchShader::GetShaderPtr()
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    return GetGX2FetchShader()->shaderPtr;
#else
    return NULL;
#endif
}

const void* GfxFetchShader::GetShaderPtr() const
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    return GetGX2FetchShader()->shaderPtr;
#else
    return NULL;
#endif
}

void GfxFetchShader::SetDefault(void* pShader)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
#if NW_G3D_IS_HOST_CAFE
    NW_G3D_ASSERT_ADDR_ALIGNMENT(pShader, SHADER_ALIGNMENT);
#endif // NW_G3D_IS_HOST_CAFE

    u32 numAttrib = gx2FetchShader.numAttribs;
    u32 numCFlowInst = (numAttrib + MAX_INST_PER_FETCH_CLAUSE - 1) >> 4;
    u32* pCFlowInst = static_cast<u32*>(pShader);
    for (u32 idxCFlowInst = 0; idxCFlowInst < numCFlowInst; ++idxCFlowInst)
    {
        u32 inst0 = (ofsVFInst + idxCFlowInst * MAX_INST_PER_FETCH_CLAUSE * VF_INST_SIZE) >> 3;
        WriteInst(pCFlowInst++, 0, inst0);
        if (numAttrib > (idxCFlowInst + 1) * MAX_INST_PER_FETCH_CLAUSE)
        {
            WriteInst(pCFlowInst++, 0, 0x01881C00);
        }
        else
        {
            u32 inst1 = 0x01800000;
            NW_G3D_SET_FLAG_VALUE(inst1, COUNT, (numAttrib - 1));
            NW_G3D_SET_FLAG_VALUE(inst1, COUNT_3, (numAttrib - 1) >> 3);
            WriteInst(pCFlowInst++, 0, inst1);
        }
    }
    WriteInst(pCFlowInst++, 0, 0);
    WriteInst(pCFlowInst++, 0, 0x8A000000);
    memset(pCFlowInst, 0, CF_INST_SIZE);

    for (u32 idxAttrib = 0; idxAttrib < numAttrib; ++idxAttrib)
    {
        u32* pVFetchInst = GetVFInst(pShader, idxAttrib);
        WriteInst(pVFetchInst, 0, 0x3C00A001);
        WriteInst(pVFetchInst, 1, 0x28D64800);
        WriteInst(pVFetchInst, 2, 0x000A0000);
        WriteInst(pVFetchInst, 3, 0);
    }
}

void GfxFetchShader::SetLocation(void* pShader, int attribIndex, u32 location)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst1 = ReadInst(pVFetchInst, 1);
    NW_G3D_SET_FLAG_VALUE(inst1, SEMANTIC_ID, location);
    WriteInst(pVFetchInst, 1, inst1);
}

u32 GfxFetchShader::GetLocation(const void* pShader, int attribIndex) const
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    const u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst1 = ReadInst(pVFetchInst, 1);
    return NW_G3D_GET_FLAG_VALUE(inst1, SEMANTIC_ID, u32);
}

void GfxFetchShader::SetBufferSlot(void* pShader, int attribIndex, u32 slot)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    NW_G3D_ASSERT_INDEX_BOUNDS(slot, GX2_MAX_ATTRIB_BUFFERS);
    u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst0 = ReadInst(pVFetchInst, 0);
    NW_G3D_SET_FLAG_VALUE(inst0, BUFFER_ID, slot);
    WriteInst(pVFetchInst, 0, inst0);
}

void GfxFetchShader::SetFormat(void* pShader, int attribIndex, GX2AttribFormat format)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    const InternalFormat& fmt = s_InternalFormat[format & 0x3F];

    u32 inst0 = ReadInst(pVFetchInst, 0);
    NW_G3D_SET_FLAG_VALUE(inst0, MEGA_FETCH_COUNT, fmt.fetchSize);
    WriteInst(pVFetchInst, 0, inst0);

    u32 inst1 = ReadInst(pVFetchInst, 1);
    u32 fmtType = (format & static_cast<u32>(ATTRIB_BIT_FLOAT)) ? ATTRIB_FMT_SCALED :
        (format & static_cast<u32>(ATTRIB_BIT_INT)) ? ATTRIB_FMT_INT : ATTRIB_FMT_NORMALIZED;
    NW_G3D_SET_FLAG_VALUE(inst1, NUM_FORMAT_ALL, fmtType);
    NW_G3D_SET_FLAG_VALUE(inst1, DST_SEL_X, GX2_GET_COMPONENT_X_R(fmt.compSel));
    NW_G3D_SET_FLAG_VALUE(inst1, DST_SEL_Y, GX2_GET_COMPONENT_Y_G(fmt.compSel));
    NW_G3D_SET_FLAG_VALUE(inst1, DST_SEL_Z, GX2_GET_COMPONENT_Z_B(fmt.compSel));
    NW_G3D_SET_FLAG_VALUE(inst1, DST_SEL_W, GX2_GET_COMPONENT_W_A(fmt.compSel));
    NW_G3D_SET_FLAG_VALUE(inst1, FORMAT_COMP_ALL,  format & static_cast<u32>(ATTRIB_BIT_SIGNED) ? 1 : 0);
    NW_G3D_SET_FLAG_VALUE(inst1, DATA_FORMAT, fmt.surfaceFormat);
    WriteInst(pVFetchInst, 1, inst1);

    u32 inst2 = ReadInst(pVFetchInst, 2);
    NW_G3D_SET_FLAG_VALUE(inst2, ENDIAN_SWAP, fmt.endianSwap);
    WriteInst(pVFetchInst, 2, inst2);
}

u32 GfxFetchShader::GetDivisorSlot(const void* pShader, int attribIndex) const
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    static const u8 tblDivisor[] = { 0, 2, 3, 1 };
    const u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst0 = ReadInst(pVFetchInst, 0);
    return tblDivisor[NW_G3D_GET_FLAG_VALUE(inst0, SRC_SEL_X, u32)];
}

void GfxFetchShader::SetOffset(void* pShader, int attribIndex, u32 offset)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst2 = ReadInst(pVFetchInst, 2);
    NW_G3D_SET_FLAG_VALUE(inst2, OFFSET, offset);
    WriteInst(pVFetchInst, 2, inst2);
}

u32 GfxFetchShader::GetOffset(const void* pShader, int attribIndex) const
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    const u32* pVFetchInst = GetVFInst(pShader, attribIndex);
    u32 inst2 = ReadInst(pVFetchInst, 2);
    return NW_G3D_GET_FLAG_VALUE(inst2, OFFSET, u32);
}

u32* GfxFetchShader::GetVFInst(void* pShader, int attribIndex)
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    return AddOffset<u32>(pShader, ofsVFInst + attribIndex * VF_INST_SIZE);
}

const u32* GfxFetchShader::GetVFInst(const void* pShader, int attribIndex) const
{
    NW_G3D_ASSERT_NOT_NULL(pShader);
    return AddOffset<u32>(pShader, ofsVFInst + attribIndex * VF_INST_SIZE);
}

void GfxFetchShader::LoadVertexAttribArray(u32 idxAttrib, const GfxBuffer* pBuffer) const
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    u32 location = GetLocation(idxAttrib);

    NW_G3D_ASSERT(handle != 0);
    glBindVertexArray(handle);
    NW_G3D_GL_ASSERT();

    glEnableVertexAttribArray(location);
    NW_G3D_GL_ASSERT();

    const u32* pVFetchInst = GetVFInst(idxAttrib);
    u32 inst1 = ReadInst(pVFetchInst, 1);
    u8 surfaceFormat = NW_G3D_GET_FLAG_VALUE(inst1, DATA_FORMAT, u8);
    const GLAttribInternal& fmt = FindGLAttribInternal(surfaceFormat);
    u32 type;
    if (fmt.type == GL_UNSIGNED_INT_2_10_10_10_REV)
    {
        type = NW_G3D_GET_FLAG_VALUE(inst1, FORMAT_COMP_ALL, u8) ?
            GL_INT_2_10_10_10_REV : GL_UNSIGNED_INT_2_10_10_10_REV;
    }
    else
    {
        type = fmt.type - NW_G3D_GET_FLAG_VALUE(inst1, FORMAT_COMP_ALL, u8);
    }
    u32 formatAll = NW_G3D_GET_FLAG_VALUE(inst1, NUM_FORMAT_ALL, u32);
    u32 offset = GetOffset(idxAttrib);
    if (formatAll == ATTRIB_FMT_INT)
    {
        NW_G3D_ASSERT(pBuffer->handle != 0);
        glBindBuffer(GL_ARRAY_BUFFER, pBuffer->handle);
        NW_G3D_GL_ASSERT();
        glVertexAttribIPointer(location, fmt.size, type, pBuffer->stride, reinterpret_cast<const GLvoid*>(offset));
        NW_G3D_GL_ASSERT();
    }
    else
    {
        NW_G3D_ASSERT(pBuffer->handle != 0);
        glBindBuffer(GL_ARRAY_BUFFER, pBuffer->handle);
        NW_G3D_GL_ASSERT();
        glVertexAttribPointer(location, fmt.size, type, formatAll == ATTRIB_FMT_NORMALIZED, pBuffer->stride, reinterpret_cast<const GLvoid*>(offset));
        NW_G3D_GL_ASSERT();
    }
    glVertexAttribDivisor(location, GetDivisor(GetDivisorSlot(idxAttrib)));
    NW_G3D_GL_ASSERT();
#else
    NW_G3D_UNUSED(idxAttrib);
    NW_G3D_UNUSED(pBuffer);
#endif
}

void GfxFetchShader::LoadVertexAttribArray() const
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    for (u32 idxAttrib = 0; idxAttrib < gx2FetchShader.numAttribs; ++idxAttrib)
    {
        const GfxBuffer* pBuffer = GetVertexBuffer(idxAttrib);
        NW_G3D_ASSERT_NOT_NULL(pBuffer);
        if (pBuffer->stride > 0)
        {
            LoadVertexAttribArray(idxAttrib, pBuffer);
            NW_G3D_GL_ASSERT();
        }
    }
#endif
}

void GfxFetchShader::LoadVertexAttribValue() const
{
#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    if (bit32 fixedAttribs = gx2FetchShader._regs[0])
    {
        for (u32 idxAttrib = 0; idxAttrib < gx2FetchShader.numAttribs; ++idxAttrib)
        {
            if (fixedAttribs & (0x1 << idxAttrib))
            {
                const GfxBuffer* pBuffer = GetVertexBuffer(idxAttrib);
                NW_G3D_ASSERT_NOT_NULL(pBuffer);

                u32 location = GetLocation(idxAttrib);

                NW_G3D_ASSERT(handle != 0);
                glBindVertexArray(handle);
                NW_G3D_GL_ASSERT();

                glDisableVertexAttribArray(location);
                NW_G3D_GL_ASSERT();

                const u32* pVFetchInst = GetVFInst(GetGX2FetchShader()->shaderPtr, idxAttrib);
                u32 inst2 = ReadInst(pVFetchInst, 2);

                u8 endianSwap = NW_G3D_GET_FLAG_VALUE(inst2, ENDIAN_SWAP, u8);
                u8 cmpSize = 0x1 << endianSwap;
                if (cmpSize == 4)
                {
                    u32 inst0 = ReadInst(pVFetchInst, 0);
                    u32 inst1 = ReadInst(pVFetchInst, 1);
                    u8 vertexSize = NW_G3D_GET_FLAG_VALUE(inst0, MEGA_FETCH_COUNT, u8) + 1;
                    u32 fmtType = NW_G3D_GET_FLAG_VALUE(inst1, NUM_FORMAT_ALL, u32);
                    u8 signedType = NW_G3D_GET_FLAG_VALUE(inst1, FORMAT_COMP_ALL,  u8);

                    u8 cmpCount = vertexSize / cmpSize;
                    if (fmtType == ATTRIB_FMT_SCALED)
                    {
                        float vertices[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                        memcpy(vertices, pBuffer->pData, sizeof(u32) * cmpCount);
                        glVertexAttrib4fv(location, vertices);
                    }
                    else
                    {
                        u32 vertices[4] = { 0, 0, 0, 1 };
                        memcpy(vertices, pBuffer->pData, sizeof(u32) * cmpCount);
                        if (signedType)
                        {
                            glVertexAttrib4iv(location, reinterpret_cast<s32*>(vertices));
                        }
                        else
                        {
                            glVertexAttrib4uiv(location, vertices);
                        }
                    }
                }
                else
                {
                    NW_G3D_NOT_IMPLEMENTED();
                }
            }
        }
    }
#endif
}

void SetPrimitiveRestartIndex(u32 restartIndex)
{
#if NW_G3D_IS_GX2
    GX2SetPrimitiveRestartIndex(restartIndex);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    glPrimitiveRestartIndex(restartIndex);
    NW_G3D_GL_ASSERT();
#else
    NW_G3D_UNUSED(restartIndex);
#endif
}

} } } // namespace nw::g3d::fnd
