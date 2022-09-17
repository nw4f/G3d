#ifndef NW_G3D_FND_GFXSTATE_H_
#define NW_G3D_FND_GFXSTATE_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/fnd/g3d_GX2Struct.h>
#include <cafe/gx2/gx2RenderStateReg.h>

#define NW_G3D_GFX_STATE(class_name)                                                               \
public:                                                                                            \
    typedef class_name self_type;                                                                  \
    typedef class_name##_t base_type;                                                              \
    static self_type* DownCast(base_type* ptr) { return static_cast<self_type*>(ptr); }            \
    static const self_type* DownCast(const base_type* ptr)                                         \
        { return static_cast<const self_type*>(ptr); }                                             \
    static self_type& DownCast(base_type& ref) { return static_cast<self_type&>(ref); }            \
    static const self_type& DownCast(const base_type& ref)                                         \
        { return static_cast<const self_type&>(ref); }                                             \
    class_name() /* { SetDefault(); } */;                                                          \


namespace nw { namespace g3d { namespace fnd {

struct GfxPolygonCtrl_t
{
    GX2PolygonControlReg gx2PolygonControl;
};

class GfxPolygonCtrl : public GfxPolygonCtrl_t
{
    NW_G3D_GFX_STATE(GfxPolygonCtrl);

public:
    void Load() const;

    /*
    void SetDefault();

    void SetCullFront(GX2Boolean cull);
    void SetCullBack(GX2Boolean cull);
    void SetFrontFace(GX2FrontFaceMode mode);

    GX2Boolean GetCullFront() const;
    GX2Boolean GetCullBack() const;
    */
    GX2FrontFaceMode GetFrontFace() const;

    /*
    void SetPolygonModeEnable(GX2Boolean enable);
    void SetPolygonModeFront(GX2PolygonMode mode);
    void SetPolygonModeBack(GX2PolygonMode mode);
    */

    GX2Boolean GetPolygonModeEnable() const;
    GX2PolygonMode GetPolygonModeFront() const;
    GX2PolygonMode GetPolygonModeBack() const;

    void SetPolygonOffsetFrontEnable(GX2Boolean enable);
    /*
    void SetPolygonOffsetBackEnable(GX2Boolean enable);
    void SetPointLineOffsetEnable(GX2Boolean enable);

    GX2Boolean GetPolygonOffsetFrontEnable() const;
    GX2Boolean GetPolygonOffsetBackEnable() const;
    */
    GX2Boolean GetPointLineOffsetEnable() const;
};

struct GfxDepthCtrl_t
{
    GX2DepthStencilControlReg gx2DepthStencilControl;
};

class GfxDepthCtrl : public GfxDepthCtrl_t
{
    NW_G3D_GFX_STATE(GfxDepthCtrl);

public:
    void Load() const;

    /*
    void SetDefault();

    void SetDepthTestEnable(GX2Boolean enable);
    void SetDepthWriteEnable(GX2Boolean enable);
    void SetDepthFunc(GX2CompareFunction func);
    void SetStencilTestEnable(GX2Boolean enable);
    void SetBackStencilEnable(GX2Boolean enable);
    */

    GX2Boolean GetDepthTestEnable() const;
    GX2Boolean GetDepthWriteEnable() const;
    GX2CompareFunction GetDepthFunc() const;
    GX2Boolean GetStencilTestEnable() const;
    GX2Boolean GetBackStencilEnable() const;

    /*
    void SetFrontStencilFunc(GX2CompareFunction func);
    void SetFrontStencilZPass(GX2StencilFunction func);
    void SetFrontStencilZFail(GX2StencilFunction func);
    void SetFrontStencilFail(GX2StencilFunction func);
    */

    GX2CompareFunction GetFrontStencilFunc() const;
    GX2StencilFunction GetFrontStencilZPass() const;
    GX2StencilFunction GetFrontStencilZFail() const;
    GX2StencilFunction GetFrontStencilFail() const;

    /*
    void SetBackStencilFunc(GX2CompareFunction func);
    void SetBackStencilZPass(GX2StencilFunction func);
    void SetBackStencilZFail(GX2StencilFunction func);
    void SetBackStencilFail(GX2StencilFunction func);
    */

    GX2CompareFunction GetBackStencilFunc() const;
    GX2StencilFunction GetBackStencilZPass() const;
    GX2StencilFunction GetBackStencilZFail() const;
    GX2StencilFunction GetBackStencilFail() const;
};

struct GfxAlphaTest_t
{
    GX2AlphaTestReg gx2AlphaTest;
};

#if NW_G3D_IS_GL && !defined( NW_STRIP_GL )
extern u32 s_AlphaFunc;
extern f32 s_AlphaRefValue;
#endif

class GfxAlphaTest : public GfxAlphaTest_t
{
    NW_G3D_GFX_STATE(GfxAlphaTest);

public:
    void Load() const;

    /*
    void SetDefault();

    void SetAlphaTestEnable(GX2Boolean enable);
    void SetAlphaFunc(GX2CompareFunction func);
    */
    void SetRefValue(f32 value);

    GX2Boolean GetAlphaTestEnable() const;
    GX2CompareFunction GetAlphaFunc() const;
    f32 GetRefValue() const;
};

struct GfxColorCtrl_t
{
    GX2ColorControlReg gx2ColorControl;
};

class GfxColorCtrl : public GfxColorCtrl_t
{
    NW_G3D_GFX_STATE(GfxColorCtrl);

public:
    void Load() const;

    /*
    void SetDefault();

    void SetLogicOp(GX2LogicOp lop);
    void SetBlendEnableMask(u8 mask);
    void SetMultiWriteEnable(GX2Boolean enable);
    void SetColorBufferEnable(GX2Boolean enable);
    */

    GX2LogicOp GetLogicOp() const;
    u8 GetBlendEnableMask() const;
    /*
    GX2Boolean GetMultiWriteEnable() const;
    GX2Boolean GetColorBufferEnable() const;
    */
};

struct GfxBlendCtrl_t
{
    GX2BlendControlReg gx2BlendControl;
};

class GfxBlendCtrl : public GfxBlendCtrl_t
{
    NW_G3D_GFX_STATE(GfxBlendCtrl);

public:
    void Load() const;
    void Load(GX2RenderTarget target) const;

    /*
    void SetDefault();

    void SetColorSrcBlend(GX2BlendFunction func);
    void SetColorDstBlend(GX2BlendFunction func);
    void SetColorCombine(GX2BlendCombine combine);
    void SetSeparateAlphaBlend(GX2Boolean enable);
    void SetAlphaSrcBlend(GX2BlendFunction func);
    void SetAlphaDstBlend(GX2BlendFunction func);
    void SetAlphaCombine(GX2BlendCombine combine);
    */

    GX2BlendFunction GetColorSrcBlend() const;
    GX2BlendFunction GetColorDstBlend() const;
    GX2BlendCombine GetColorCombine() const;
    GX2Boolean GetSeparateAlphaBlend() const;
    GX2BlendFunction GetAlphaSrcBlend() const;
    GX2BlendFunction GetAlphaDstBlend() const;
    GX2BlendCombine GetAlphaCombine() const;
};

struct GfxBlendColor_t
{
   GX2BlendConstantColorReg gx2BlendConstantColor;
};

class GfxBlendColor : public GfxBlendColor_t
{
    NW_G3D_GFX_STATE(GfxBlendColor);

public:
    void Load() const;

    /*
    void SetDefault();

    void SetConstantColor(const f32 color[4]);
    f32* GetConstantColor();
    const f32* GetConstantColor() const;
    */
};

} } } // namespace nw::g3d::fnd

#endif // NW_G3D_FND_GFXSTATE_H_
