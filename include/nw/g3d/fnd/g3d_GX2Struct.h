#ifndef NW_G3D_FND_GX2STRUCT_H_
#define NW_G3D_FND_GX2STRUCT_H_

#include <nw/g3d/g3d_config.h>
#include <cafe/gx2/gx2Enum.h>
#include <cafe/gx2/gx2Constant.h>
#include <cafe/gx2/gx2Misc.h>
#include <cafe/gx2/gx2Shaders.h>
#include <cafe/gx2/gx2Surface.h>
#include <cafe/gx2/gx2Texture.h>

namespace nw { namespace g3d { namespace fnd { namespace internal {

typedef ::GX2Surface GX2SurfaceData;
typedef ::GX2Texture GX2TextureData;

struct GX2LoopVarData
{
    u32 _regs[GX2_NUM_LOOP_VAR_U32_WORDS];
};

typedef ::GX2VertexShader GX2VertexShaderData;
typedef ::GX2GeometryShader GX2GeometryShaderData;
typedef ::GX2PixelShader GX2PixelShaderData;
typedef ::GX2FetchShader GX2FetchShaderData;

} } } } // namespace nw::g3d::fnd::internal

#endif // NW_G3D_FND_GX2STRUCT_H_
