#include <nw/g3d/fnd/g3d_GX2Utility.h>
#include <nw/g3d/ut/g3d_Inlines.h>

#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    #ifdef _WIN32
        #ifndef WIN32
            #define WIN32
        #endif
    #endif
#endif

#include <functional>
#include <algorithm>
#include <cstring>

namespace nw { namespace g3d { namespace fnd {

#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )

GLPrimitiveType s_GLPrimType[] =
{
    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { GL_POINTS,                    GL_POINTS       },
    { GL_LINES,                     GL_LINES        },
    { GL_LINE_STRIP,                GL_LINES        },
    { GL_TRIANGLES,                 GL_TRIANGLES    },
    { GL_TRIANGLE_FAN,              GL_TRIANGLES    },
    { GL_TRIANGLE_STRIP,            GL_TRIANGLES    },
    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { GL_LINES_ADJACENCY,           GL_LINES        },
    { GL_LINE_STRIP_ADJACENCY,      GL_LINES        },
    { GL_TRIANGLES_ADJACENCY,       GL_TRIANGLES    },
    { GL_TRIANGLE_STRIP_ADJACENCY,  GL_TRIANGLES    },
    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { GL_LINES,                     GL_LINES        },
    { GL_LINE_LOOP,                 GL_LINES        },
    { GL_QUADS,                     GL_TRIANGLES    },
    { GL_QUAD_STRIP,                GL_TRIANGLES    },

    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { 0xFFFFFFFF,                   0xFFFFFFFF      },
    { 0xFFFFFFFF,                   0xFFFFFFFF      }
};

GLSurfaceFormat s_GLSurfaceFormat[] =
{
    { GX2_SURFACE_FORMAT_INVALID,                  GL_INVALID_ENUM,      GL_INVALID_ENUM,          GL_INVALID_ENUM                 },
    { GX2_SURFACE_FORMAT_TC_R8_UNORM,              GL_R8,                GL_RED,                   GL_UNSIGNED_BYTE                },
    { GX2_SURFACE_FORMAT_TC_R8_UINT,               GL_R8UI,              GL_RED_INTEGER,           GL_UNSIGNED_BYTE                },
    { GX2_SURFACE_FORMAT_TC_R8_SNORM,              GL_R8_SNORM,          GL_RED,                   GL_BYTE                         },
    { GX2_SURFACE_FORMAT_TC_R8_SINT,               GL_R8I,               GL_RED_INTEGER,           GL_BYTE                         },
    { GX2_SURFACE_FORMAT_TCD_R16_UNORM,            GL_R16,               GL_RED,                   GL_UNSIGNED_SHORT               },
    { GX2_SURFACE_FORMAT_TC_R16_UINT,              GL_R16UI,             GL_RED_INTEGER,           GL_UNSIGNED_SHORT               },
    { GX2_SURFACE_FORMAT_TC_R16_SNORM,             GL_R16_SNORM,         GL_RED,                   GL_SHORT                        },
    { GX2_SURFACE_FORMAT_TC_R16_SINT,              GL_R16I,              GL_RED_INTEGER,           GL_SHORT                        },
    { GX2_SURFACE_FORMAT_TC_R16_FLOAT,             GL_R16F,              GL_RED,                   GL_HALF_FLOAT                   },
    { GX2_SURFACE_FORMAT_TC_R8_G8_UNORM,           GL_RG8,               GL_RG,                    GL_UNSIGNED_BYTE                },
    { GX2_SURFACE_FORMAT_TC_R8_G8_UINT,            GL_RG8UI,             GL_RG_INTEGER,            GL_UNSIGNED_BYTE                },
    { GX2_SURFACE_FORMAT_TC_R8_G8_SNORM,           GL_RG8_SNORM,         GL_RG,                    GL_BYTE                         },
    { GX2_SURFACE_FORMAT_TC_R8_G8_SINT,            GL_RG8I,              GL_RG_INTEGER,            GL_BYTE                         },
    { GX2_SURFACE_FORMAT_TCS_R5_G6_B5_UNORM,       GL_RGB,               GL_RGB,                   GL_UNSIGNED_SHORT_5_6_5_REV     },
    { GX2_SURFACE_FORMAT_TC_R5_G5_B5_A1_UNORM,     GL_RGB5_A1,           GL_RGBA,                  GL_UNSIGNED_SHORT_1_5_5_5_REV   },
    { GX2_SURFACE_FORMAT_TC_R4_G4_B4_A4_UNORM,     GL_RGBA4,             GL_RGBA,                  GL_UNSIGNED_SHORT_4_4_4_4_REV   },
    { GX2_SURFACE_FORMAT_TC_A1_B5_G5_R5_UNORM,     GL_RGBA8,             GL_ABGR_EXT,              GL_UNSIGNED_SHORT_5_5_5_1       },
    { GX2_SURFACE_FORMAT_TC_R32_UINT,              GL_R32UI,             GL_RED_INTEGER,           GL_UNSIGNED_INT                 },
    { GX2_SURFACE_FORMAT_TC_R32_SINT,              GL_R32I,              GL_RED_INTEGER,           GL_UNSIGNED_INT                 },
    { GX2_SURFACE_FORMAT_TCD_R32_FLOAT,            GL_R32F,              GL_RED,                   GL_FLOAT                        },
    { GX2_SURFACE_FORMAT_TC_R16_G16_UNORM,         GL_RG16,              GL_RG,                    GL_UNSIGNED_SHORT               },
    { GX2_SURFACE_FORMAT_TC_R16_G16_UINT,          GL_RG16UI,            GL_RG_INTEGER,            GL_UNSIGNED_SHORT               },
    { GX2_SURFACE_FORMAT_TC_R16_G16_SNORM,         GL_RG16_SNORM,        GL_RG,                    GL_SHORT                        },
    { GX2_SURFACE_FORMAT_TC_R16_G16_SINT,          GL_RG16I,             GL_RG_INTEGER,            GL_SHORT                        },
    { GX2_SURFACE_FORMAT_TC_R16_G16_FLOAT,         GL_RG16F,             GL_RG,                    GL_HALF_FLOAT                   },
    { GX2_SURFACE_FORMAT_TC_R11_G11_B10_FLOAT,     GL_RGB16F,            GL_RGB,                   GL_UNSIGNED_INT_10F_11F_11F_REV },
    { GX2_SURFACE_FORMAT_TCS_R10_G10_B10_A2_UNORM, GL_RGB10_A2,          GL_RGBA,                  GL_UNSIGNED_INT_2_10_10_10_REV  },
    { GX2_SURFACE_FORMAT_TC_R10_G10_B10_A2_UINT,   GL_RGB10_A2UI,        GL_RGBA_INTEGER,          GL_UNSIGNED_INT_2_10_10_10_REV  },
    { GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM,    GL_RGBA8,             GL_RGBA,                  GL_UNSIGNED_BYTE                },
    { GX2_SURFACE_FORMAT_TC_R8_G8_B8_A8_UINT,      GL_RGBA8UI,           GL_RGBA_INTEGER,          GL_UNSIGNED_BYTE                },
    { GX2_SURFACE_FORMAT_TC_R8_G8_B8_A8_SNORM,     GL_RGBA8_SNORM,       GL_RGBA,                  GL_BYTE                         },
    { GX2_SURFACE_FORMAT_TC_R8_G8_B8_A8_SINT,      GL_RGBA8I,            GL_RGBA_INTEGER,          GL_BYTE                         },
    { GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_SRGB,     GL_SRGB8_ALPHA8,      GL_RGBA,                  GL_UNSIGNED_BYTE                },
    { GX2_SURFACE_FORMAT_TCS_A2_B10_G10_R10_UNORM, GL_RGBA16,            GL_ABGR_EXT,              GL_UNSIGNED_INT_10_10_10_2      },
    { GX2_SURFACE_FORMAT_TC_A2_B10_G10_R10_UINT,   GL_RGBA16UI,          GL_ABGR_EXT,              GL_UNSIGNED_INT_10_10_10_2      },
    { GX2_SURFACE_FORMAT_TC_R32_G32_UINT,          GL_RG32UI,            GL_RG_INTEGER,            GL_UNSIGNED_INT                 },
    { GX2_SURFACE_FORMAT_TC_R32_G32_SINT,          GL_RG32I,             GL_RG_INTEGER,            GL_INT                          },
    { GX2_SURFACE_FORMAT_TC_R32_G32_FLOAT,         GL_RG32F,             GL_RG,                    GL_FLOAT                        },
    { GX2_SURFACE_FORMAT_TC_R16_G16_B16_A16_UNORM, GL_RGBA16,            GL_RGBA,                  GL_UNSIGNED_SHORT               },
    { GX2_SURFACE_FORMAT_TC_R16_G16_B16_A16_UINT,  GL_RGBA16UI,          GL_RGBA_INTEGER,          GL_UNSIGNED_SHORT               },
    { GX2_SURFACE_FORMAT_TC_R16_G16_B16_A16_SNORM, GL_RGBA16_SNORM,      GL_RGBA,                  GL_SHORT                        },
    { GX2_SURFACE_FORMAT_TC_R16_G16_B16_A16_SINT,  GL_RGBA16I,           GL_RGBA_INTEGER,          GL_SHORT                        },
    { GX2_SURFACE_FORMAT_TC_R16_G16_B16_A16_FLOAT, GL_RGBA16F,           GL_RGBA,                  GL_HALF_FLOAT                   },
    { GX2_SURFACE_FORMAT_TC_R32_G32_B32_A32_UINT,  GL_RGBA32UI,          GL_RGBA_INTEGER,          GL_UNSIGNED_INT                 },
    { GX2_SURFACE_FORMAT_TC_R32_G32_B32_A32_SINT,  GL_RGBA32I,           GL_RGBA_INTEGER,          GL_INT                          },
    { GX2_SURFACE_FORMAT_TC_R32_G32_B32_A32_FLOAT, GL_RGBA32F,           GL_RGBA,                  GL_FLOAT                        },
    { GX2_SURFACE_FORMAT_T_BC1_UNORM,              GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,         GL_INVALID_ENUM, GL_INVALID_ENUM      },
    { GX2_SURFACE_FORMAT_T_BC1_SRGB,               GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,   GL_INVALID_ENUM, GL_INVALID_ENUM      },
    { GX2_SURFACE_FORMAT_T_BC2_UNORM,              GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,         GL_INVALID_ENUM, GL_INVALID_ENUM      },
    { GX2_SURFACE_FORMAT_T_BC2_SRGB,               GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,   GL_INVALID_ENUM, GL_INVALID_ENUM      },
    { GX2_SURFACE_FORMAT_T_BC3_UNORM,              GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,         GL_INVALID_ENUM, GL_INVALID_ENUM      },
    { GX2_SURFACE_FORMAT_T_BC3_SRGB,               GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,   GL_INVALID_ENUM, GL_INVALID_ENUM      },
    { GX2_SURFACE_FORMAT_T_BC4_UNORM,              GL_COMPRESSED_RED_RGTC1,                  GL_INVALID_ENUM, GL_INVALID_ENUM      },
    { GX2_SURFACE_FORMAT_T_BC4_SNORM,              GL_COMPRESSED_SIGNED_RED_RGTC1,           GL_INVALID_ENUM, GL_INVALID_ENUM      },
    { GX2_SURFACE_FORMAT_T_BC5_UNORM,              GL_COMPRESSED_RG_RGTC2,                   GL_INVALID_ENUM, GL_INVALID_ENUM      },
    { GX2_SURFACE_FORMAT_T_BC5_SNORM,              GL_COMPRESSED_SIGNED_RG_RGTC2,            GL_INVALID_ENUM, GL_INVALID_ENUM      },
    { GX2_SURFACE_FORMAT_LAST,                     GL_INVALID_ENUM,                          GL_INVALID_ENUM, GL_INVALID_ENUM      },
};

GLSurfaceFormat s_GLDepthFormat[] =
{
    { GX2_SURFACE_FORMAT_INVALID,                  GL_INVALID_ENUM,      GL_INVALID_ENUM,    GL_INVALID_ENUM                       },
    { GX2_SURFACE_FORMAT_TCD_R16_UNORM,            GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT                     },
    { GX2_SURFACE_FORMAT_TCD_R32_FLOAT,            GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT                              },
    { GX2_SURFACE_FORMAT_D_D24_S8_UNORM,           GL_DEPTH24_STENCIL8,  GL_DEPTH_STENCIL,   GL_UNSIGNED_INT_24_8                  },
    { GX2_SURFACE_FORMAT_D_D32_FLOAT_S8_UINT_X24,  GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL,   GL_FLOAT_32_UNSIGNED_INT_24_8_REV     },
    { GX2_SURFACE_FORMAT_LAST,                     GL_INVALID_ENUM,      GL_INVALID_ENUM,    GL_INVALID_ENUM                       },
};

const GLPrimitiveType& FindGLPrimitiveType(GX2PrimitiveType type)
{
    return s_GLPrimType[type];
}

struct GLSurfaceCmp : public std::binary_function<GLSurfaceFormat, GX2SurfaceFormat, bool>
{
    bool operator ()(const GLSurfaceFormat& lhs, GX2SurfaceFormat rhs) const
    {
        return lhs.gx2Format == rhs;
    }
};

const GLSurfaceFormat& FindGLFormat(GX2SurfaceFormat format)
{
    return *std::find_if(
        s_GLSurfaceFormat,
        s_GLSurfaceFormat + (sizeof(s_GLSurfaceFormat) / sizeof(GLSurfaceFormat) - 1),
        std::bind2nd(GLSurfaceCmp(), format));
}

const GLSurfaceFormat& FindGLDepthFormat(GX2SurfaceFormat format)
{
    return *std::find_if(
        s_GLDepthFormat,
        s_GLDepthFormat + (sizeof(s_GLDepthFormat) / sizeof(GLSurfaceFormat) - 1),
        std::bind2nd(GLSurfaceCmp(), format));
}

#endif

void ConvertToGLSurface(GX2Surface& surface, int arrayLength)
{
    // Some limitations
    NW_G3D_ASSERT(arrayLength == 1);
    NW_G3D_ASSERT(surface.dim == GX2_SURFACE_DIM_2D);
    NW_G3D_ASSERT(surface.depth == 1);
    NW_G3D_ASSERT(surface.aa == GX2_AA_MODE_1X);
    NW_G3D_ASSERT(surface.use & GX2_SURFACE_USE_TEXTURE);
    NW_G3D_ASSERT(surface.imageSize && surface.imagePtr);
    NW_G3D_ASSERT((surface.numMips <= 1 && !surface.mipSize && !surface.mipPtr) ||
                  (surface.numMips > 1 && surface.mipSize && surface.mipPtr));
    NW_G3D_ASSERT(surface.tileMode > GX2_TILE_MODE_DEFAULT &&
                  surface.tileMode < GX2_TILE_MODE_LINEAR_SPECIAL);

    GX2Surface linear;
    linear.dim = GX2_SURFACE_DIM_2D;
    linear.width = surface.width;
    linear.height = surface.height;
    linear.depth = 1;
    linear.numMips = surface.numMips;
    linear.format = surface.format;
    linear.aa = GX2_AA_MODE_1X;
    linear.use = surface.use;
    //u32 imageSize;
    //void* imagePtr;
    //u32 mipSize;
    //void* mipPtr;
    linear.tileMode = GX2_TILE_MODE_LINEAR_SPECIAL;
    linear.swizzle = 0;
    //u32 alignment;
    //u32 pitch;
    GX2CalcSurfaceSizeAndAlignment(&linear);

    NW_G3D_ASSERT(surface.imageSize >= linear.imageSize &&
                  surface.mipSize   >= linear.mipSize);

    size_t imageSize = linear.imageSize;
    if (linear.numMips > 1)
    {
        imageSize = linear.mipOffset[0];
        NW_G3D_ASSERT(imageSize >= linear.imageSize);
        imageSize += linear.mipSize;
    }

    linear.imagePtr = std::malloc(imageSize);
    NW_G3D_ASSERT(linear.imagePtr);

    if (linear.numMips > 1)
        linear.mipPtr = (void*)((uintptr_t)linear.imagePtr + linear.mipOffset[0]);
    else
        linear.mipPtr = NULL;

    for (u32 mipLevel = 2; mipLevel < surface.numMips; ++mipLevel)
        NW_G3D_ASSERT(linear.mipOffset[mipLevel - 1] < linear.mipSize);

    NW_G3D_ASSERT(surface.numMips == linear.numMips);
    for (u32 mipLevel = 0; mipLevel < surface.numMips; ++mipLevel)
        GX2CopySurface(&surface, mipLevel, 0, &linear, mipLevel, 0);

    bool relative = (surface.numMips > 1) &&
                    (((uintptr_t)surface.mipPtr - (uintptr_t)surface.imagePtr) == surface.mipOffset[0]);

    surface.tileMode = GX2_TILE_MODE_LINEAR_SPECIAL;
    surface.swizzle = 0;
    GX2CalcSurfaceSizeAndAlignment(&surface);

    std::memcpy(surface.imagePtr, linear.imagePtr, linear.imageSize);

    if (surface.numMips > 1)
    {
        // NW_G3D_ASSERT(relative);
        if (relative)
            surface.mipPtr = (void*)((uintptr_t)surface.imagePtr + surface.mipOffset[0]);

        std::memcpy(surface.mipPtr, linear.mipPtr, linear.mipSize);
    }

    std::free(linear.imagePtr);

    linear.imagePtr = NULL;
    linear.mipPtr = NULL;
}

u32 CalcImageSize(GX2SurfaceFormat format, u32 width, u32 height, u32 depth)
{
    u32 size = 0;
    u32 type = format & 0xFF;
    if (IsCompressed(format))
    {
        width  = ( width + 3 ) >> 2;
        height = ( height + 3 ) >> 2;
        size = (type == 0x31 || type == 0x34) ? 8 : 16; // (BC1 BC4) or (BC2 BC3 BC5)
    }
    else
    {
        size = type == 0 ? 0 :      // GX2_SURFACE_FORMAT_INVALID
            type <= 0x02 ? 1 :      // GX2_SURFACE_FORMAT_T_4_4_UNORM
            type <= 0x0C ? 2 :      // GX2_SURFACE_FORMAT_TC_5_5_5_1_UNORM
            type <= 0x1C ? 4 :      // GX2_SURFACE_FORMAT_T_X24_8_X32_UINT
            type <= 0x20 ? 8 :      // GX2_SURFACE_FORMAT_TC_16_16_16_16_FLOAT
            type <= 0x23 ? 16 :     // GX2_SURFACE_FORMAT_TC_32_32_32_32_FLOAT
            type >= 0x2F ? 12 :     // GX2_SURFACE_FORMAT_T_32_32_32_UINT
            type == 0x2B ? 4 : 0;   // GX2_SURFACE_FORMAT_T_5_9_9_9_SHAREDEXP
    }
    return width * height * depth * size;
}

bool IsCompressed(GX2SurfaceFormat format)
{
    return (format & 0xFF) >= (GX2_SURFACE_FORMAT_T_BC1_UNORM & 0xFF);
}

const void* GetImagePtr(const GX2Surface& surface, u32 mipLevel)
{
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    return
        mipLevel == 0 ? surface.imagePtr :
        mipLevel == 1 ? surface.mipPtr :
        surface.mipPtr ? AddOffset(surface.mipPtr, surface.mipOffset[mipLevel - 1]) : NULL;
#else
    NW_G3D_UNUSED(surface);
    NW_G3D_UNUSED(mipLevel);
    return NULL;
#endif
}

} } } // namespace nw::g3d::fnd
