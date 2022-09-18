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
#if NW_G3D_IS_GX2
    GX2SetPolygonControlReg(&gx2PolygonControl);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    u32 cullFace = NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, CULL_FACE, u32);
    if (cullFace)
    {
        glEnable(GL_CULL_FACE);
        NW_G3D_TABLE_FIELD u32 tblCullFace[] = { GL_FRONT, GL_BACK, GL_FRONT_AND_BACK };
        glCullFace(tblCullFace[cullFace - 1]);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
    NW_G3D_TABLE_FIELD u32 tblFrontFace[] = { GL_CCW, GL_CW };
    glFrontFace(tblFrontFace[GetFrontFace()]);
    if (GetPolygonModeEnable())
    {
        NW_G3D_TABLE_FIELD u32 tblPolygonMode[] = { GL_POINT, GL_LINE, GL_FILL };
        if (GetPolygonModeFront() == GetPolygonModeBack())
        {
            glPolygonMode(GL_FRONT_AND_BACK, tblPolygonMode[GetPolygonModeFront()]);
        }
        else
        {
            NW_G3D_WARNING(false, "Separate front-back polygon mode is not supported.\n");
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, POLY_OFFSET_ENABLE, u32))
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
    }
    else
    {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
    if (GetPointLineOffsetEnable())
    {
        glEnable(GL_POLYGON_OFFSET_POINT);
        glEnable(GL_POLYGON_OFFSET_LINE);
    }
    else
    {
        glDisable(GL_POLYGON_OFFSET_POINT);
        glDisable(GL_POLYGON_OFFSET_LINE);
    }
    NW_G3D_GL_ASSERT();
#endif
}

GX2FrontFaceMode GfxPolygonCtrl::GetFrontFace() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.pa_su_sc_mode_cntl, FACE, GX2FrontFaceMode);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, FACE, GX2FrontFaceMode);
#endif // __WUT__
}

GX2Boolean GfxPolygonCtrl::GetPolygonModeEnable() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.pa_su_sc_mode_cntl, POLY_MODE, GX2Boolean);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, POLY_MODE, GX2Boolean);
#endif // __WUT__
}

GX2PolygonMode GfxPolygonCtrl::GetPolygonModeFront() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.pa_su_sc_mode_cntl, POLYMODE_FRONT_PTYPE, GX2PolygonMode);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, POLYMODE_FRONT_PTYPE, GX2PolygonMode);
#endif // __WUT__
}

GX2PolygonMode GfxPolygonCtrl::GetPolygonModeBack() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.pa_su_sc_mode_cntl, POLYMODE_BACK_PTYPE, GX2PolygonMode);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, POLYMODE_BACK_PTYPE, GX2PolygonMode);
#endif // __WUT__
}

void GfxPolygonCtrl::SetPolygonOffsetFrontEnable(GX2Boolean enable)
{
#ifdef __WUT__
    NW_G3D_SET_FLAG_VALUE(gx2PolygonControl.pa_su_sc_mode_cntl, POLY_OFFSET_FRONT_ENABLE, enable);
#else
    NW_G3D_SET_FLAG_VALUE(gx2PolygonControl.reg, POLY_OFFSET_FRONT_ENABLE, enable);
#endif // __WUT__
}

GX2Boolean GfxPolygonCtrl::GetPointLineOffsetEnable() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.pa_su_sc_mode_cntl, POLY_OFFSET_PARA_ENABLE, GX2Boolean);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2PolygonControl.reg, POLY_OFFSET_PARA_ENABLE, GX2Boolean);
#endif // __WUT__
}

void GfxDepthCtrl::Load() const
{
#if NW_G3D_IS_GX2
    GX2SetDepthStencilControlReg(&gx2DepthStencilControl);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_GL_ASSERT();
    if (GetDepthTestEnable())
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    NW_G3D_GL_ASSERT();
    glDepthMask(static_cast<GLboolean>(GetDepthWriteEnable()));
    NW_G3D_GL_ASSERT();
    glDepthFunc(GL_NEVER + GetDepthFunc());
    NW_G3D_GL_ASSERT();

    if (GetStencilTestEnable())
    {
        glEnable(GL_STENCIL_TEST);
    }
    else
    {
        glDisable(GL_STENCIL_TEST);
    }
    NW_G3D_GL_ASSERT();
    NW_G3D_TABLE_FIELD u32 tblStencilOp[] = {
        GL_KEEP,
        GL_ZERO,
        GL_REPLACE,
        GL_INCR,
        GL_DECR,
        GL_INVERT,
        GL_INCR_WRAP,
        GL_DECR_WRAP
    };
    if (GetBackStencilEnable())
    {
        s32 refValue = 0;
        u32 valueMask = 0;
        glGetIntegerv(GL_STENCIL_REF, &refValue);
        NW_G3D_GL_ASSERT();
        glGetIntegerv(GL_STENCIL_VALUE_MASK, reinterpret_cast<s32*>(&valueMask));
        NW_G3D_GL_ASSERT();
        glStencilFuncSeparate(GL_FRONT, GL_NEVER + GetFrontStencilFunc(), refValue, valueMask);
        NW_G3D_GL_ASSERT();
        glGetIntegerv(GL_STENCIL_BACK_REF, &refValue);
        NW_G3D_GL_ASSERT();
        glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, reinterpret_cast<s32*>(&valueMask));
        NW_G3D_GL_ASSERT();
        glStencilFuncSeparate(GL_BACK, GL_NEVER + GetBackStencilFunc(), refValue, valueMask);
        NW_G3D_GL_ASSERT();
        glStencilOpSeparate(GL_FRONT, tblStencilOp[GetFrontStencilFail()],
            tblStencilOp[GetFrontStencilZFail()], tblStencilOp[GetFrontStencilZPass()]);
        NW_G3D_GL_ASSERT();
        glStencilOpSeparate(GL_BACK, tblStencilOp[GetFrontStencilFail()],
            tblStencilOp[GetFrontStencilZFail()], tblStencilOp[GetFrontStencilZPass()]);
        NW_G3D_GL_ASSERT();
    }
    else
    {
        s32 refValue = 0;
        u32 mask = 0;
        glGetIntegerv(GL_STENCIL_REF, &refValue);
        NW_G3D_GL_ASSERT();
        glGetIntegerv(GL_STENCIL_VALUE_MASK, reinterpret_cast<s32*>(&mask));
        NW_G3D_GL_ASSERT();
        glStencilFunc(GL_NEVER + GetFrontStencilFunc(), refValue, mask);
        NW_G3D_GL_ASSERT();
        glStencilOp(tblStencilOp[GetFrontStencilFail()],
            tblStencilOp[GetFrontStencilZFail()], tblStencilOp[GetFrontStencilZPass()]);
        NW_G3D_GL_ASSERT();
    }
    NW_G3D_GL_ASSERT();
#endif
}

GX2Boolean GfxDepthCtrl::GetDepthTestEnable() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, Z_ENABLE, GX2Boolean);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, Z_ENABLE, GX2Boolean);
#endif // __WUT__
}

GX2Boolean GfxDepthCtrl::GetDepthWriteEnable() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, Z_WRITE_ENABLE, GX2Boolean);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, Z_WRITE_ENABLE, GX2Boolean);
#endif // __WUT__
}

GX2CompareFunction GfxDepthCtrl::GetDepthFunc() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, ZFUNC, GX2CompareFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, ZFUNC, GX2CompareFunction);
#endif // __WUT__
}

GX2Boolean GfxDepthCtrl::GetStencilTestEnable() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, STENCIL_ENABLE, GX2Boolean);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCIL_ENABLE, GX2Boolean);
#endif // __WUT__
}

GX2Boolean GfxDepthCtrl::GetBackStencilEnable() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, BACKFACE_ENABLE, GX2Boolean);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, BACKFACE_ENABLE, GX2Boolean);
#endif // __WUT__
}

GX2CompareFunction GfxDepthCtrl::GetFrontStencilFunc() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, STENCILFUNC, GX2CompareFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILFUNC, GX2CompareFunction);
#endif // __WUT__
}

GX2StencilFunction GfxDepthCtrl::GetFrontStencilZPass() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, STENCILZPASS, GX2StencilFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILZPASS, GX2StencilFunction);
#endif // __WUT__
}

GX2StencilFunction GfxDepthCtrl::GetFrontStencilZFail() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, STENCILZFAIL, GX2StencilFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILZFAIL, GX2StencilFunction);
#endif // __WUT__
}

GX2StencilFunction GfxDepthCtrl::GetFrontStencilFail() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, STENCILFAIL, GX2StencilFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILFAIL, GX2StencilFunction);
#endif // __WUT__
}

GX2CompareFunction GfxDepthCtrl::GetBackStencilFunc() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, STENCILFUNC_BF, GX2CompareFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILFUNC_BF, GX2CompareFunction);
#endif // __WUT__
}

GX2StencilFunction GfxDepthCtrl::GetBackStencilZPass() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, STENCILZPASS_BF, GX2StencilFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILZPASS_BF, GX2StencilFunction);
#endif // __WUT__
}

GX2StencilFunction GfxDepthCtrl::GetBackStencilZFail() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, STENCILZFAIL_BF, GX2StencilFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILZFAIL_BF, GX2StencilFunction);
#endif // __WUT__
}

GX2StencilFunction GfxDepthCtrl::GetBackStencilFail() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.db_depth_control, STENCILFAIL_BF, GX2StencilFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2DepthStencilControl.reg, STENCILFAIL_BF, GX2StencilFunction);
#endif // __WUT__
}

#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )

u32 s_AlphaFunc     = GL_ALWAYS;
f32 s_AlphaRefValue = 0.5f;

#endif

void GfxAlphaTest::Load() const
{
#if NW_G3D_IS_GX2
    GX2SetAlphaTestReg(&gx2AlphaTest);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    if (GetAlphaTestEnable())
    {
      //NW_G3D_WARNING(false, "Alpha test is not supported.\n");
        s_AlphaFunc     = GL_NEVER + GetAlphaFunc();
        s_AlphaRefValue = GetRefValue();
    }
    else
    {
        s_AlphaFunc     = GL_ALWAYS;
        s_AlphaRefValue = 0.5f;
    }
#endif
}

void GfxAlphaTest::SetRefValue(f32 value)
{
    union
    {
        f32 _f32;
        u32 _u32;
    } reg = { value };
#ifdef __WUT__
    gx2AlphaTest.sx_alpha_ref = reg._u32;
#else
    gx2AlphaTest.reg[1] = reg._u32;
#endif // __WUT__
}

GX2Boolean GfxAlphaTest::GetAlphaTestEnable() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2AlphaTest.sx_alpha_test_control, ALPHA_TEST_ENABLE, GX2Boolean);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2AlphaTest.reg[0], ALPHA_TEST_ENABLE, GX2Boolean);
#endif // __WUT__
}

GX2CompareFunction GfxAlphaTest::GetAlphaFunc() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2AlphaTest.sx_alpha_test_control, ALPHA_FUNC, GX2CompareFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2AlphaTest.reg[0], ALPHA_FUNC, GX2CompareFunction);
#endif // __WUT__
}

f32 GfxAlphaTest::GetRefValue() const
{
    union
    {
        u32 _u32;
        f32 _f32;
#ifdef __WUT__
    } reg = { gx2AlphaTest.sx_alpha_ref };
#else
    } reg = { gx2AlphaTest.reg[1] };
#endif // __WUT__
    return reg._f32;
}

void GfxColorCtrl::Load() const
{
#if NW_G3D_IS_GX2
    GX2SetColorControlReg(&gx2ColorControl);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    if (GetLogicOp() == GX2_LOGIC_OP_COPY)
    {
        glDisable(GL_COLOR_LOGIC_OP);
        NW_G3D_GL_ASSERT();
    }
    else
    {
        NW_G3D_TABLE_FIELD u32 tblLogicOp[] = {
            GL_CLEAR,
            GL_NOR,
            GL_AND_INVERTED,
            GL_COPY_INVERTED,
            GL_AND_REVERSE,
            GL_INVERT,
            GL_XOR,
            GL_NAND,
            GL_AND,
            GL_EQUIV,
            GL_NOOP,
            GL_OR_INVERTED,
            GL_COPY,
            GL_OR_REVERSE,
            GL_OR,
            GL_SET
        };
        glEnable(GL_COLOR_LOGIC_OP);
        NW_G3D_GL_ASSERT();
        glLogicOp(tblLogicOp[GetLogicOp() & 0xF]);
        NW_G3D_GL_ASSERT();
    }
    u32 blendEnableMask = GetBlendEnableMask();
    for (int renderTarget = 0; renderTarget < 8; ++renderTarget)
    {
        if ((blendEnableMask >> renderTarget) & 0x1)
        {
            glEnablei(GL_BLEND, renderTarget);
        }
        else
        {
            glDisablei(GL_BLEND, renderTarget);
        }
        NW_G3D_GL_ASSERT();
    }
    NW_G3D_GL_ASSERT();
#endif
}

GX2LogicOp GfxColorCtrl::GetLogicOp() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2ColorControl.cb_color_control, ROP3, GX2LogicOp);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2ColorControl.reg, ROP3, GX2LogicOp);
#endif // __WUT__
}

u8 GfxColorCtrl::GetBlendEnableMask() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2ColorControl.cb_color_control, TARGET_BLEND_ENABLE, u8);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2ColorControl.reg, TARGET_BLEND_ENABLE, u8);
#endif // __WUT__
}

void GfxBlendCtrl::Load() const
{
#if NW_G3D_IS_GX2
    GX2SetBlendControlReg(&gx2BlendControl);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    Load(static_cast<GX2RenderTarget>(gx2BlendControl.reg[0]));
#endif
}

void GfxBlendCtrl::Load(GX2RenderTarget target) const
{
#if NW_G3D_IS_GX2
    GX2BlendControlReg tempReg;

#ifdef __WUT__
    tempReg.target = target;
    tempReg.cb_blend_control = gx2BlendControl.cb_blend_control;
#else
    tempReg.reg[0] = static_cast<u32>(target);
    tempReg.reg[1] = gx2BlendControl.reg[1];
#endif // __WUT__

    GX2SetBlendControlReg(&tempReg);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    NW_G3D_TABLE_FIELD u32 tblBlendEq[] = {
        GL_FUNC_ADD,
        GL_FUNC_SUBTRACT,
        GL_MIN,
        GL_MAX,
        GL_FUNC_REVERSE_SUBTRACT
    };
    NW_G3D_TABLE_FIELD u32 tblBlendFunc[] = {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_ONE_MINUS_SRC_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA_SATURATE,
        GL_NONE,
        GL_NONE,
        GL_CONSTANT_COLOR,
        GL_ONE_MINUS_CONSTANT_COLOR,
        GL_SRC1_COLOR,
        GL_ONE_MINUS_SRC1_COLOR,
        GL_SRC1_ALPHA,
        GL_ONE_MINUS_SRC1_ALPHA,
        GL_CONSTANT_ALPHA,
        GL_ONE_MINUS_CONSTANT_ALPHA
    };
    if (target > 0)
    {
        NW_G3D_WARNING(false, "Separate blending per render target is not supported.\n");
    }
    if (GetSeparateAlphaBlend())
    {
        glBlendEquationSeparate(
            tblBlendEq[GetColorCombine()], tblBlendEq[GetAlphaCombine()]);
        glBlendFuncSeparate(
            tblBlendFunc[GetColorSrcBlend()], tblBlendFunc[GetColorDstBlend()],
            tblBlendFunc[GetAlphaSrcBlend()], tblBlendFunc[GetAlphaDstBlend()]);
    }
    else
    {
        glBlendEquation(tblBlendEq[GetColorCombine()]);
        glBlendFunc(
            tblBlendFunc[GetColorSrcBlend()], tblBlendFunc[GetColorDstBlend()]);
    }
    NW_G3D_GL_ASSERT();
#else
    NW_G3D_UNUSED(target);
#endif
}

GX2BlendFunction GfxBlendCtrl::GetColorSrcBlend() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.cb_blend_control, COLOR_SRCBLEND, GX2BlendFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], COLOR_SRCBLEND, GX2BlendFunction);
#endif // __WUT__
}

GX2BlendFunction GfxBlendCtrl::GetColorDstBlend() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.cb_blend_control, COLOR_DESTBLEND, GX2BlendFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], COLOR_DESTBLEND, GX2BlendFunction);
#endif // __WUT__
}

GX2BlendCombine GfxBlendCtrl::GetColorCombine() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.cb_blend_control, COLOR_COMB_FCN, GX2BlendCombine);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], COLOR_COMB_FCN, GX2BlendCombine);
#endif // __WUT__
}

GX2Boolean GfxBlendCtrl::GetSeparateAlphaBlend() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.cb_blend_control, SEPARATE_ALPHA_BLEND, GX2Boolean);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], SEPARATE_ALPHA_BLEND, GX2Boolean);
#endif // __WUT__
}

GX2BlendFunction GfxBlendCtrl::GetAlphaSrcBlend() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.cb_blend_control, ALPHA_SRCBLEND, GX2BlendFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], ALPHA_SRCBLEND, GX2BlendFunction);
#endif // __WUT__
}

GX2BlendFunction GfxBlendCtrl::GetAlphaDstBlend() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.cb_blend_control, ALPHA_DESTBLEND, GX2BlendFunction);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], ALPHA_DESTBLEND, GX2BlendFunction);
#endif // __WUT__
}

GX2BlendCombine GfxBlendCtrl::GetAlphaCombine() const
{
#ifdef __WUT__
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.cb_blend_control, ALPHA_COMB_FCN, GX2BlendCombine);
#else
    return NW_G3D_GET_FLAG_VALUE(gx2BlendControl.reg[1], ALPHA_COMB_FCN, GX2BlendCombine);
#endif // __WUT__
}

void GfxBlendColor::Load() const
{
#if NW_G3D_IS_GX2
    GX2SetBlendConstantColorReg(&gx2BlendConstantColor);
#elif NW_G3D_IS_GL && !defined( NW_STRIP_GL )
    glBlendColor(
        gx2BlendConstantColor.reg[0].f32All, gx2BlendConstantColor.reg[1].f32All,
        gx2BlendConstantColor.reg[2].f32All, gx2BlendConstantColor.reg[3].f32All);
    NW_G3D_GL_ASSERT();
#endif
}

} } } // namespace nw::g3d::fnd
