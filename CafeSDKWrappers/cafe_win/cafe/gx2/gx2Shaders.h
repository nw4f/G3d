#pragma once

#include <ninTexUtils/gx2/gx2Shaders.h>

#include <cafe/gx2/gx2Constant.h>
#include <cafe/gx2/gx2Enum.h>

#define GX2_NUM_VERTEX_SHADER_REGISTERS      52
#define GX2_NUM_GEOMETRY_SHADER_REGISTERS      19
#define GX2_NUM_PIXEL_SHADER_REGISTERS      41
#define GX2_NUM_FETCH_SHADER_REGISTERS      1
#define GX2_NUM_COMPUTE_SHADER_REGISTERS      12
#define GX2_NUM_LOOP_VAR_U32_WORDS      2

typedef struct _GX2FetchShader
{
    GX2FetchShaderType type;
    u32 _regs[GX2_NUM_FETCH_SHADER_REGISTERS];
    u32                 shaderSize;
    void              * shaderPtr;
    u32 numAttribs;
    u32 _num_divisors;
    u32 _divisors[2];

} GX2FetchShader;

typedef struct _GX2StreamOutContext
{
    u32 data[GX2_STREAMOUT_BUFFER_CONTEXT_SIZE/sizeof(u32)];
} GX2StreamOutContext;
