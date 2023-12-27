#pragma once

#include <gx2/shaders.h>

#include <cafe/gx2/gx2Constant.h>
#include <cafe/gx2/gx2Misc.h>

#define GX2_NUM_VERTEX_SHADER_REGISTERS      52
#define GX2_NUM_GEOMETRY_SHADER_REGISTERS      19
#define GX2_NUM_PIXEL_SHADER_REGISTERS      41
#define GX2_NUM_FETCH_SHADER_REGISTERS      1
#define GX2_NUM_COMPUTE_SHADER_REGISTERS      12
#define GX2_NUM_LOOP_VAR_U32_WORDS      2

#define numAttribs attribCount
#define _divisors divisors
#define shaderPtr program
#define shaderSize size
#define copyShaderPtr vertexProgram
#define copyShaderSize vertexProgramSize
#define shaderMode mode
#define destSel mask
#define indexType type

typedef struct _GX2StreamOutContext
{
    u32 data[GX2_STREAMOUT_BUFFER_CONTEXT_SIZE/sizeof(u32)];
} GX2StreamOutContext;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

static inline void GX2SetShaders(
    const GX2FetchShader* fs,
    const GX2VertexShader* vs,
    const GX2PixelShader* ps
)
{
    GX2SetFetchShader(fs);
    GX2SetVertexShader(vs);
    GX2SetPixelShader(ps);
}

static inline void GX2SetShadersEx(
    const GX2FetchShader* fs,
    const GX2VertexShader* vs,
    const GX2GeometryShader* gs,
    const GX2PixelShader* ps
)
{
    GX2SetFetchShader(fs);
    GX2SetVertexShader(vs);
    GX2SetGeometryShader(gs);
    GX2SetPixelShader(ps);
}

static inline void GX2InitAttribStream(
    GX2AttribStream* attribStream,
    u32 location,
    u32 buffer,
    u32 offset,
    GX2AttribFormat format
)
{
    const GX2CompSel dstSel[20] = { 0x00040405, 0x00010405,
                                    0x00040405, 0x00040405,
                                    0x00010405, 0x00040405,
                                    0x00040405, 0x00010405,
                                    0x00010405, 0x00010205,
                                    0x00010203, 0x00010203,
                                    0x00010405, 0x00010405,
                                    0x00010203, 0x00010203,
                                    0x00010205, 0x00010205,
                                    0x00010203, 0x00010203};

    attribStream->buffer      = buffer;
    attribStream->offset      = offset;
    attribStream->location    = location;
    attribStream->format      = format;
    attribStream->mask        = dstSel[format & 0xff];
    attribStream->indexType   = GX2_ATTRIB_INDEX_PER_VERTEX;
    attribStream->aluDivisor  = 0;
    attribStream->endianSwap  = GX2_ENDIAN_SWAP_DEFAULT;
}

static inline GX2SamplerVar* GX2GetPixelSamplerVar(const GX2PixelShader* shader, const char* name)
{
    for (uint32_t i = 0; i < shader->samplerVarCount; i++)
    {
       if (strcmp(shader->samplerVars[i].name, name) == 0)
           return &(shader->samplerVars[i]);
    }

    return nullptr;
}

static inline s32 GX2GetPixelSamplerVarLocation(const GX2PixelShader* shader, const char* name)
{
    GX2SamplerVar* sampler = GX2GetPixelSamplerVar(shader, name);
    if (!sampler)
        return -1;

    return sampler->location;
}

static inline GX2AttribVar* GX2GetVertexAttribVar(const GX2VertexShader* shader, const char* name)
{
    for (uint32_t i = 0; i < shader->attribVarCount; i++)
    {
       if (strcmp(shader->attribVars[i].name, name) == 0)
           return &(shader->attribVars[i]);
    }

    return nullptr;
}

static inline s32 GX2GetVertexAttribVarLocation(const GX2VertexShader* shader, const char* name)
{
    GX2AttribVar* attrib = GX2GetVertexAttribVar(shader, name);
    if (!attrib)
        return -1;

    return attrib->location;
}

static inline u32 GX2CalcFetchShaderSize(u32 num_attrib)
{
    return GX2CalcFetchShaderSizeEx(num_attrib,
                                    GX2_FETCH_SHADER_TESSELLATION_NONE,
                                    GX2_TESSELLATION_MODE_DISCRETE);
}

static inline void GX2InitFetchShader(
    GX2FetchShader* fs,
    void* fs_buffer,
    u32 count,
    const GX2AttribStream* attribs
)
{
    GX2InitFetchShaderEx(fs,
                         (uint8_t*)fs_buffer,
                         count,
                         attribs,
                         GX2_FETCH_SHADER_TESSELLATION_NONE,
                         GX2_TESSELLATION_MODE_DISCRETE);

}

#ifdef __cplusplus
}
#endif // __cplusplus
