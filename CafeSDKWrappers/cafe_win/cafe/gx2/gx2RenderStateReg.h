#pragma once

typedef struct{
    u32 reg;
}GX2DepthStencilControlReg;

typedef struct{
    u32 reg;
}GX2PolygonControlReg;

typedef struct{
    u32 reg[2];
}GX2StencilMaskReg;

typedef struct{
    u32 reg[2];
}GX2AlphaTestReg;

typedef struct{
    u32 reg;
}GX2AlphaToMaskReg;

typedef struct{
    union {
        u32 u32All;
        f32 f32All;
    } reg[5];
}GX2PolygonOffsetReg;

typedef struct{
    u32 reg;
}GX2TargetChannelMaskReg;

typedef struct{
    u32 reg;
}GX2AAMaskReg;

typedef struct{
    u32 reg;
}GX2ColorControlReg;

typedef struct{
    u32 reg[2];
}GX2BlendControlReg;

typedef struct{
    union {
        u32 u32All;
        f32 f32All;
    } reg[4];
}GX2BlendConstantColorReg;

typedef struct{
    u32 reg;
}GX2PointSizeReg;

typedef struct{
    u32 reg;
}GX2PointLimitsReg;

typedef struct{
    u32 reg;
}GX2LineWidthReg;

typedef struct{
    union {
        u32 u32All;
        f32 f32All;
    } reg[12];
}GX2ViewportReg;

typedef struct{
    u32 reg[2];
}GX2ScissorReg;
