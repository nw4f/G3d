#include <nw/g3d/fnd/g3d_GfxState.h>
#include <nw/g3d/ut/g3d_Inlines.h>
#include <nw/g3d/ut/g3d_Flag.h>
#include <nw/g3d/math/g3d_MathCommon.h>

#include <algorithm>

namespace nw { namespace g3d { namespace fnd {

namespace {

enum RegPolygonCtrl
{
    NW_G3D_FLAG_VALUE_DECLARE( 0,  1, CULL_FRONT),
    NW_G3D_FLAG_VALUE_DECLARE( 1,  1, CULL_BACK),
    NW_G3D_FLAG_VALUE_DECLARE( 2,  1, FACE),
    NW_G3D_FLAG_VALUE_DECLARE( 3,  2, POLY_MODE),
    NW_G3D_FLAG_VALUE_DECLARE( 5,  3, POLYMODE_FRONT_PTYPE),
    NW_G3D_FLAG_VALUE_DECLARE( 8,  3, POLYMODE_BACK_PTYPE),
    NW_G3D_FLAG_VALUE_DECLARE(11,  1, POLY_OFFSET_FRONT_ENABLE),
    NW_G3D_FLAG_VALUE_DECLARE(12,  1, POLY_OFFSET_BACK_ENABLE),
    NW_G3D_FLAG_VALUE_DECLARE(13,  1, POLY_OFFSET_PARA_ENABLE),
    NW_G3D_FLAG_VALUE_DECLARE( 0,  2, CULL_FACE),
    NW_G3D_FLAG_VALUE_DECLARE(11,  2, POLY_OFFSET_ENABLE)
};

enum RegDepthCtrl
{
    NW_G3D_FLAG_VALUE_DECLARE( 0,  1, STENCIL_ENABLE),
    NW_G3D_FLAG_VALUE_DECLARE( 1,  1, Z_ENABLE),
    NW_G3D_FLAG_VALUE_DECLARE( 2,  1, Z_WRITE_ENABLE),
    NW_G3D_FLAG_VALUE_DECLARE( 4,  3, ZFUNC),
    NW_G3D_FLAG_VALUE_DECLARE( 7,  1, BACKFACE_ENABLE),
    NW_G3D_FLAG_VALUE_DECLARE( 8,  3, STENCILFUNC),
    NW_G3D_FLAG_VALUE_DECLARE(11,  3, STENCILFAIL),
    NW_G3D_FLAG_VALUE_DECLARE(14,  3, STENCILZPASS),
    NW_G3D_FLAG_VALUE_DECLARE(17,  3, STENCILZFAIL),
    NW_G3D_FLAG_VALUE_DECLARE(20,  3, STENCILFUNC_BF),
    NW_G3D_FLAG_VALUE_DECLARE(23,  3, STENCILFAIL_BF),
    NW_G3D_FLAG_VALUE_DECLARE(26,  3, STENCILZPASS_BF),
    NW_G3D_FLAG_VALUE_DECLARE(29,  3, STENCILZFAIL_BF)
};

enum RegStencilMask
{
    NW_G3D_FLAG_VALUE_DECLARE( 0,  8, STENCILREF),
    NW_G3D_FLAG_VALUE_DECLARE( 8,  8, STENCILMASK),
    NW_G3D_FLAG_VALUE_DECLARE(16,  8, STENCILWRITEMASK)
};

enum RegAlphaTest
{
    NW_G3D_FLAG_VALUE_DECLARE( 0,  3, ALPHA_FUNC),
    NW_G3D_FLAG_VALUE_DECLARE( 3,  1, ALPHA_TEST_ENABLE)
};

enum RegColorCtrl
{
    NW_G3D_FLAG_VALUE_DECLARE( 1,  1, MULTIWRITE_ENABLE),
    NW_G3D_FLAG_VALUE_DECLARE( 4,  3, SPECIAL_OP),
    NW_G3D_FLAG_VALUE_DECLARE( 8,  8, TARGET_BLEND_ENABLE),
    NW_G3D_FLAG_VALUE_DECLARE(16,  8, ROP3)
};

enum RegBlendCtrl
{
    NW_G3D_FLAG_VALUE_DECLARE( 0,  5, COLOR_SRCBLEND),
    NW_G3D_FLAG_VALUE_DECLARE( 5,  3, COLOR_COMB_FCN),
    NW_G3D_FLAG_VALUE_DECLARE( 8,  5, COLOR_DESTBLEND),
    NW_G3D_FLAG_VALUE_DECLARE(16,  5, ALPHA_SRCBLEND),
    NW_G3D_FLAG_VALUE_DECLARE(21,  3, ALPHA_COMB_FCN),
    NW_G3D_FLAG_VALUE_DECLARE(24,  5, ALPHA_DESTBLEND),
    NW_G3D_FLAG_VALUE_DECLARE(29,  1, SEPARATE_ALPHA_BLEND)
};

}

void GfxPolygonCtrl::Load() const
{
    GX2SetPolygonControlReg(&gx2PolygonControl);
}

GX2FrontFaceMode GfxPolygonCtrl::GetFrontFace() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, FACE, GX2FrontFaceMode);
}

GX2Boolean GfxPolygonCtrl::GetPolygonModeEnable() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, POLY_MODE, GX2Boolean);
}

GX2PolygonMode GfxPolygonCtrl::GetPolygonModeFront() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, POLYMODE_FRONT_PTYPE, GX2PolygonMode);
}

GX2PolygonMode GfxPolygonCtrl::GetPolygonModeBack() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, POLYMODE_BACK_PTYPE, GX2PolygonMode);
}

void GfxPolygonCtrl::SetPolygonOffsetFrontEnable(GX2Boolean enable)
{
    NW_G3D_SET_FLAG_VALUE(gx2PolygonControl.reg, POLY_OFFSET_FRONT_ENABLE, enable);
}

GX2Boolean GfxPolygonCtrl::GetPointLineOffsetEnable() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, POLY_OFFSET_PARA_ENABLE, GX2Boolean);
}

void GfxDepthCtrl::Load() const
{
    GX2SetDepthStencilControlReg(&gx2DepthStencilControl);
}

GX2Boolean GfxDepthCtrl::GetDepthTestEnable() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, Z_ENABLE, GX2Boolean);
}

GX2Boolean GfxDepthCtrl::GetDepthWriteEnable() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, Z_WRITE_ENABLE, GX2Boolean);
}

GX2CompareFunction GfxDepthCtrl::GetDepthFunc() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, ZFUNC, GX2CompareFunction);
}

GX2Boolean GfxDepthCtrl::GetStencilTestEnable() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCIL_ENABLE, GX2Boolean);
}

GX2Boolean GfxDepthCtrl::GetBackStencilEnable() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, BACKFACE_ENABLE, GX2Boolean);
}

GX2CompareFunction GfxDepthCtrl::GetFrontStencilFunc() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILFUNC, GX2CompareFunction);
}

GX2StencilFunction GfxDepthCtrl::GetFrontStencilZPass() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILZPASS, GX2StencilFunction);
}

GX2StencilFunction GfxDepthCtrl::GetFrontStencilZFail() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILZFAIL, GX2StencilFunction);
}

GX2StencilFunction GfxDepthCtrl::GetFrontStencilFail() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILFAIL, GX2StencilFunction);
}

GX2CompareFunction GfxDepthCtrl::GetBackStencilFunc() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILFUNC_BF, GX2CompareFunction);
}

GX2StencilFunction GfxDepthCtrl::GetBackStencilZPass() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILZPASS_BF, GX2StencilFunction);
}

GX2StencilFunction GfxDepthCtrl::GetBackStencilZFail() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILZFAIL_BF, GX2StencilFunction);
}

GX2StencilFunction GfxDepthCtrl::GetBackStencilFail() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILFAIL_BF, GX2StencilFunction);
}

void GfxAlphaTest::Load() const
{
    GX2SetAlphaTestReg(&gx2AlphaTest);
}

void GfxAlphaTest::SetRefValue(f32 value)
{
    union
    {
        f32 _f32;
        u32 _u32;
    } reg = { value };
    gx2AlphaTest.reg[1] = reg._u32;
}

GX2Boolean GfxAlphaTest::GetAlphaTestEnable() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2AlphaTest.reg[0], ALPHA_TEST_ENABLE, GX2Boolean);
}

GX2CompareFunction GfxAlphaTest::GetAlphaFunc() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2AlphaTest.reg[0], ALPHA_FUNC, GX2CompareFunction);
}

f32 GfxAlphaTest::GetRefValue() const
{
    union
    {
        u32 _u32;
        f32 _f32;
    } reg = { gx2AlphaTest.reg[1] };
    return reg._f32;
}

void GfxColorCtrl::Load() const
{
    GX2SetColorControlReg(&gx2ColorControl);
}

GX2LogicOp GfxColorCtrl::GetLogicOp() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2ColorControl.reg, ROP3, GX2LogicOp);
}

u8 GfxColorCtrl::GetBlendEnableMask() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2ColorControl.reg, TARGET_BLEND_ENABLE, u8);
}

void GfxBlendCtrl::Load() const
{
    GX2SetBlendControlReg(&gx2BlendControl);
}

void GfxBlendCtrl::Load(GX2RenderTarget target) const
{
    GX2BlendControlReg tempReg;

    tempReg.reg[0] = static_cast<u32>(target);
    tempReg.reg[1] = gx2BlendControl.reg[1];

    GX2SetBlendControlReg(&tempReg);
}

GX2BlendFunction GfxBlendCtrl::GetColorSrcBlend() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], COLOR_SRCBLEND, GX2BlendFunction);
}

GX2BlendFunction GfxBlendCtrl::GetColorDstBlend() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], COLOR_DESTBLEND, GX2BlendFunction);
}

GX2BlendCombine GfxBlendCtrl::GetColorCombine() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], COLOR_COMB_FCN, GX2BlendCombine);
}

GX2Boolean GfxBlendCtrl::GetSeparateAlphaBlend() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], SEPARATE_ALPHA_BLEND, GX2Boolean);
}

GX2BlendFunction GfxBlendCtrl::GetAlphaSrcBlend() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], ALPHA_SRCBLEND, GX2BlendFunction);
}

GX2BlendFunction GfxBlendCtrl::GetAlphaDstBlend() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], ALPHA_DESTBLEND, GX2BlendFunction);
}

GX2BlendCombine GfxBlendCtrl::GetAlphaCombine() const
{
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], ALPHA_COMB_FCN, GX2BlendCombine);
}

void GfxBlendColor::Load() const
{
    GX2SetBlendConstantColorReg(&gx2BlendConstantColor);
}

} } } // namespace nw::g3d::fnd
