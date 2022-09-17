#ifndef NW_G3D_FND_GX2UTILITY_H_
#define NW_G3D_FND_GX2UTILITY_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/fnd/g3d_GX2Struct.h>

namespace nw { namespace g3d { namespace fnd {

struct GLPrimitiveType
{
    u32 drawType;
    u32 varyingType;
};

struct GLSurfaceFormat
{
    GX2SurfaceFormat gx2Format;
    u32 internalFormat;
    u32 format;
    u32 type;
};

const GLPrimitiveType& FindGLPrimitiveType(GX2PrimitiveType type);

const GLSurfaceFormat& FindGLFormat(GX2SurfaceFormat format);
const GLSurfaceFormat& FindGLDepthFormat(GX2SurfaceFormat format);

void ConvertToGLSurface(GX2Surface& surface, int arrayLength);

bool IsCompressed(GX2SurfaceFormat format);
u32 CalcImageSize(GX2SurfaceFormat format, u32 width, u32 height, u32 depth);

// void* GetImagePtr(GX2Surface& surface, u32 mipLevel);
const void* GetImagePtr(const GX2Surface& surface, u32 mipLevel);

} } } // namespace nw::g3d::fnd

#endif // NW_G3D_FND_GX2UTILITY_H_
