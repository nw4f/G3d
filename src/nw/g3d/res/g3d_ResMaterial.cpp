#include <nw/g3d/res/g3d_ResMaterial.h>
#include <nw/g3d/res/g3d_ResFile.h>
#include <limits>
#include <algorithm>

namespace nw { namespace g3d { namespace res {

namespace {

struct TexMtx
{
    float m[3][2];
};

enum
{
    NUM_COMP_MTX14 = 4,
    NUM_COMP_MTX24 = 8
};

template <bool swap>
void ConvertSrt2d(void* pDst, const void* pSrc)
{
    const Srt2d& srt = *static_cast<const Srt2d*>(pSrc);

    float sinR, cosR;
    Math::SinCos(&sinR, &cosR, srt.r);

    TexMtx mtx;
    mtx.m[0][0] =  srt.sx * cosR;
    mtx.m[0][1] =  srt.sx * sinR;
    mtx.m[1][0] = -srt.sy * sinR;
    mtx.m[1][1] =  srt.sy * cosR;
    mtx.m[2][0] =  srt.tx;
    mtx.m[2][1] =  srt.ty;

    Copy32<swap>(pDst, &mtx, sizeof(TexMtx) / sizeof(float));
}

template <bool swap>
void ConvertSrt3d(void* pDst, const void* pSrc)
{
    const Srt3d& srt = *static_cast<const Srt3d*>(pSrc);

    Mtx34 mtx;
    mtx.SetSR(srt.scale, srt.rotate);
    mtx.SetT(srt.translate);

    Copy32<swap>(pDst, &mtx, sizeof(Mtx34) / sizeof(float));
}

template <bool swap>
void ConvertTexSrtMaya(void* pDst, const void* pSrc)
{
    const TexSrt& srt = *static_cast<const TexSrt*>(pSrc);
    float sinR, cosR;
    Math::SinCos(&sinR, &cosR, srt.r);
    const float sinPart = 0.5f * sinR - 0.5f;
    const float cosPart = -0.5f * cosR;

    TexMtx mtx;
    mtx.m[0][0] =  srt.sx * cosR;
    mtx.m[0][1] = -srt.sy * sinR;
    mtx.m[1][0] =  srt.sx * sinR;
    mtx.m[1][1] =  srt.sy * cosR;
    mtx.m[2][0] =  srt.sx * (cosPart - sinPart - srt.tx);
    mtx.m[2][1] =  srt.sy * (cosPart + sinPart + srt.ty) + 1.0f;

    Copy32<swap>(pDst, &mtx, sizeof(TexMtx) / sizeof(float));
}

template <bool swap>
void ConvertTexSrt3dsmax(void* pDst, const void* pSrc)
{
    const TexSrt& srt = *static_cast<const TexSrt*>(pSrc);
    float sinR, cosR;
    Math::SinCos(&sinR, &cosR, srt.r);
    const float sxcr = srt.sx * cosR;
    const float sxsr = srt.sx * sinR;
    const float sycr = srt.sy * cosR;
    const float sysr = srt.sy * sinR;

    TexMtx mtx;
    mtx.m[0][0] =  sxcr;
    mtx.m[0][1] = -sysr;
    mtx.m[1][0] =  sxsr;
    mtx.m[1][1] =  sycr;
    mtx.m[2][0] = -sxcr * (srt.tx + 0.5f) + sxsr * (srt.ty - 0.5f) + 0.5f;
    mtx.m[2][1] =  sysr * (srt.tx + 0.5f) + sycr * (srt.ty - 0.5f) + 0.5f;

    Copy32<swap>(pDst, &mtx, sizeof(TexMtx) / sizeof(float));
}

template <bool swap>
void ConvertTexSrtSoftimage(void* pDst, const void* pSrc)
{
    const TexSrt& srt = *static_cast<const TexSrt*>(pSrc);
    float sinR, cosR;
    Math::SinCos(&sinR, &cosR, srt.r);
    const float sxcr = srt.sx * cosR;
    const float sxsr = srt.sx * sinR;
    const float sycr = srt.sy * cosR;
    const float sysr = srt.sy * sinR;

    TexMtx mtx;
    mtx.m[0][0] =  sxcr;
    mtx.m[0][1] =  sysr;
    mtx.m[1][0] = -sxsr;
    mtx.m[1][1] =  sycr;
    mtx.m[2][0] =  sxsr - sxcr * srt.tx - sxsr * srt.ty;
    mtx.m[2][1] = -sycr - sysr * srt.tx + sycr * srt.ty + 1.0f;

    Copy32<swap>(pDst, &mtx, sizeof(TexMtx) / sizeof(float));
}

template <bool swap>
void ConvertTexSrtExMaya(void* pDst, const void* pSrc)
{
    const TexSrtEx& srtEx = *static_cast<const TexSrtEx*>(pSrc);
    const TexSrt& srt = srtEx.srt;
    float sinR, cosR;
    Math::SinCos(&sinR, &cosR, srt.r);
    const float sxcr = srt.sx * cosR;
    const float sxsr = srt.sx * sinR;
    const float sycr = srt.sy * cosR;
    const float sysr = srt.sy * sinR;
    const float tx = -0.5f * (sxcr + sxsr - srt.sx) - srt.tx * srt.sx;
    const float ty = -0.5f * (sycr - sysr + srt.sy) + srt.ty * srt.sy + 1.0f;

    Mtx34 mtx;
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    const Mtx34* pEffectMtx = srtEx.pEffectMtx;
    if (pEffectMtx)
    {
        mtx.m00 =  sxcr * pEffectMtx->m00 + sxsr * pEffectMtx->m10 + tx * pEffectMtx->m20;
        mtx.m10 = -sysr * pEffectMtx->m00 + sycr * pEffectMtx->m10 + ty * pEffectMtx->m20;
        mtx.m01 =  sxcr * pEffectMtx->m01 + sxsr * pEffectMtx->m11 + tx * pEffectMtx->m21;
        mtx.m11 = -sysr * pEffectMtx->m01 + sycr * pEffectMtx->m11 + ty * pEffectMtx->m21;
        mtx.m02 =  sxcr * pEffectMtx->m02 + sxsr * pEffectMtx->m12 + tx * pEffectMtx->m22;
        mtx.m12 = -sysr * pEffectMtx->m02 + sycr * pEffectMtx->m12 + ty * pEffectMtx->m22;
        mtx.m03 =  sxcr * pEffectMtx->m03 + sxsr * pEffectMtx->m13 + tx * pEffectMtx->m23;
        mtx.m13 = -sysr * pEffectMtx->m03 + sycr * pEffectMtx->m13 + ty * pEffectMtx->m23;

        memcpy(mtx.m[2], pEffectMtx->m[2], NUM_COMP_MTX14 << 2);
    }
    else
#endif
    {
        mtx.m00 =  sxcr;
        mtx.m10 = -sysr;
        mtx.m01 =  sxsr;
        mtx.m11 =  sycr;
        mtx.m02 =  tx;
        mtx.m12 =  ty;
        mtx.m03 = mtx.m13 = mtx.m20 = mtx.m21 = mtx.m23 = 0.0f;
        mtx.m22 = 1.0f;
    }

    Copy32<swap>(pDst, &mtx, sizeof(Mtx34) / sizeof(float));
}

template <bool swap>
void ConvertTexSrtEx3dsmax(void* pDst, const void* pSrc)
{
    const TexSrtEx& srtEx = *static_cast<const TexSrtEx*>(pSrc);
    const TexSrt& srt = srtEx.srt;
    float sinR, cosR;
    Math::SinCos(&sinR, &cosR, srt.r);
    const float sxcr = srt.sx * cosR;
    const float sxsr = srt.sx * sinR;
    const float sycr = srt.sy * cosR;
    const float sysr = srt.sy * sinR;
    const float tx = -sxcr * (srt.tx + 0.5f) + sxsr * (srt.ty - 0.5f) + 0.5f;
    const float ty =  sysr * (srt.tx + 0.5f) + sycr * (srt.ty - 0.5f) + 0.5f;

    Mtx34 mtx;
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    const Mtx34* pEffectMtx = srtEx.pEffectMtx;
    if (pEffectMtx)
    {
        mtx.m00 =  sxcr * pEffectMtx->m00 + sxsr * pEffectMtx->m10 + tx * pEffectMtx->m20;
        mtx.m10 = -sysr * pEffectMtx->m00 + sycr * pEffectMtx->m10 + ty * pEffectMtx->m20;
        mtx.m01 =  sxcr * pEffectMtx->m01 + sxsr * pEffectMtx->m11 + tx * pEffectMtx->m21;
        mtx.m11 = -sysr * pEffectMtx->m01 + sycr * pEffectMtx->m11 + ty * pEffectMtx->m21;
        mtx.m02 =  sxcr * pEffectMtx->m02 + sxsr * pEffectMtx->m12 + tx * pEffectMtx->m22;
        mtx.m12 = -sysr * pEffectMtx->m02 + sycr * pEffectMtx->m12 + ty * pEffectMtx->m22;
        mtx.m03 =  sxcr * pEffectMtx->m03 + sxsr * pEffectMtx->m13 + tx * pEffectMtx->m23;
        mtx.m13 = -sysr * pEffectMtx->m03 + sycr * pEffectMtx->m13 + ty * pEffectMtx->m23;

        memcpy(mtx.m[2], pEffectMtx->m[2], NUM_COMP_MTX14 << 2);
    }
    else
#endif
    {
        mtx.m00 =  sxcr;
        mtx.m10 = -sysr;
        mtx.m01 =  sxsr;
        mtx.m11 =  sycr;
        mtx.m02 =  tx;
        mtx.m12 =  ty;
        mtx.m03 = mtx.m13 = mtx.m20 = mtx.m21 = mtx.m23 = 0.0f;
        mtx.m22 = 1.0f;
    }

    Copy32<swap>(pDst, &mtx, sizeof(Mtx34) / sizeof(float));
}

template <bool swap>
void ConvertTexSrtExSoftimage(void* pDst, const void* pSrc)
{
    const TexSrtEx& srtEx = *static_cast<const TexSrtEx*>(pSrc);
    const TexSrt& srt = srtEx.srt;
    float sinR, cosR;
    Math::SinCos(&sinR, &cosR, srt.r);
    const float sxcr = srt.sx * cosR;
    const float sxsr = srt.sx * sinR;
    const float sycr = srt.sy * cosR;
    const float sysr = srt.sy * sinR;
    const float tx =  sxsr - sxcr * srt.tx - sxsr * srt.ty;
    const float ty = -sycr - sysr * srt.tx + sycr * srt.ty + 1.0f;

    Mtx34 mtx;
#if NW_G3D_HOST_PTRSIZE == NW_G3D_TARGET_PTRSIZE
    const Mtx34* pEffectMtx = srtEx.pEffectMtx;
    if (pEffectMtx)
    {
        mtx.m00 = sxcr * pEffectMtx->m00 - sxsr * pEffectMtx->m10 + tx * pEffectMtx->m20;
        mtx.m10 = sysr * pEffectMtx->m00 + sycr * pEffectMtx->m10 + ty * pEffectMtx->m20;
        mtx.m01 = sxcr * pEffectMtx->m01 - sxsr * pEffectMtx->m11 + tx * pEffectMtx->m21;
        mtx.m11 = sysr * pEffectMtx->m01 + sycr * pEffectMtx->m11 + ty * pEffectMtx->m21;
        mtx.m02 = sxcr * pEffectMtx->m02 - sxsr * pEffectMtx->m12 + tx * pEffectMtx->m22;
        mtx.m12 = sysr * pEffectMtx->m02 + sycr * pEffectMtx->m12 + ty * pEffectMtx->m22;
        mtx.m03 = sxcr * pEffectMtx->m03 - sxsr * pEffectMtx->m13 + tx * pEffectMtx->m23;
        mtx.m13 = sysr * pEffectMtx->m03 + sycr * pEffectMtx->m13 + ty * pEffectMtx->m23;

        memcpy(mtx.m[2], pEffectMtx->m[2], NUM_COMP_MTX14 << 2);
    }
    else
#endif
    {
        mtx.m00 =  sxcr;
        mtx.m10 =  sysr;
        mtx.m01 = -sxsr;
        mtx.m11 =  sycr;
        mtx.m02 =  tx;
        mtx.m12 =  ty;
        mtx.m03 = mtx.m13 = mtx.m20 = mtx.m21 = mtx.m23 = 0.0f;
        mtx.m22 = 1.0f;
    }

    Copy32<swap>(pDst, &mtx, sizeof(Mtx34) / sizeof(float));
}

void (* const s_pFuncConvertSrt[])(void*, const void*) = {
    &ConvertSrt2d<false>,
    &ConvertSrt2d<true>,
    &ConvertSrt3d<false>,
    &ConvertSrt3d<true>
};

void (* const s_pFuncConvertTexSrt[])(void*, const void*) = {
    &ConvertTexSrtMaya<false>,
    &ConvertTexSrtMaya<true>,
    &ConvertTexSrt3dsmax<false>,
    &ConvertTexSrt3dsmax<true>,
    &ConvertTexSrtSoftimage<false>,
    &ConvertTexSrtSoftimage<true>,
    &ConvertTexSrtExMaya<false>,
    &ConvertTexSrtExMaya<true>,
    &ConvertTexSrtEx3dsmax<false>,
    &ConvertTexSrtEx3dsmax<true>,
    &ConvertTexSrtExSoftimage<false>,
    &ConvertTexSrtExSoftimage<true>
};

}

void ResRenderState::Load() const
{
    GetPolygonCtrl().Load();
    GetDepthCtrl().Load();
    GetAlphaTest().Load();
    GetColorCtrl().Load();
    GetBlendCtrl().Load();
    GetBlendColor().Load();
}

void ResSampler::Setup()
{
    this->GetGfxSampler()->Setup();
}

void ResSampler::Cleanup()
{
    this->GetGfxSampler()->Cleanup();
}

template <bool swap>
void ResShaderParam::Convert(void* pDst, const void* pSrc) const
{
    ResShaderParam::Type type = GetType();
    NW_G3D_ASSERT_INDEX_BOUNDS(type, NUM_TYPE);

    if (type <= TYPE_FLOAT4)
    {
        int numComp = (type & 0x3) + 1;
        Copy32<swap>(pDst, pSrc, numComp);
    }
    else if(type <= TYPE_FLOAT4x4)
    {
        const int numCol = (type & 0x3) + 1;
        const int numRow = ((type - TYPE_RESERVED2) >> 2) + 2;
        const int numComp = static_cast<int>(numCol);
        const size_t size = sizeof(float) * numCol;
        const size_t stride = sizeof(float) * 4;
        for (int idxRow = 0; idxRow < numRow; ++idxRow)
        {
            Copy32<swap>(pDst, pSrc, numComp);
            pDst = AddOffset(pDst, stride);
            pSrc = AddOffset(pSrc, size);
        }
    }
    else if (type <= TYPE_SRT3D)
    {
        s_pFuncConvertSrt[2 * (type - TYPE_SRT2D)
                          + swap](pDst, pSrc);
    }
    else
    {
        s_pFuncConvertTexSrt[2 * ((type - TYPE_TEXSRT) * 3
                                  + static_cast<const TexSrt*>(pSrc)->mode)
                             + swap](pDst, pSrc);
    }
}

BindResult ResMaterial::Bind(const ResFile* pFile)
{
    NW_G3D_ASSERT_NOT_NULL(pFile);

    BindResult result;

    for (int idxTex = 0, numTex = GetSamplerCount(); idxTex < numTex; ++idxTex)
    {
        ResTextureRef* pTextureRef = GetTextureRef(idxTex);
        if (!pTextureRef->IsBound())
        {
            result |= pTextureRef->Bind(pFile);
        }
    }

    return result;
}

void ResMaterial::Setup()
{
    int numSampler = this->GetSamplerCount();
    for (int i = 0; i < numSampler; ++i)
    {
        this->GetSampler(i)->Setup();
    }
}

void ResMaterial::Cleanup()
{
    int numSampler = this->GetSamplerCount();
    for (int i = 0; i < numSampler; ++i)
    {
        this->GetSampler(i)->Cleanup();
    }
}

template void ResShaderParam::Convert<true>(void*, const void*) const;
template void ResShaderParam::Convert<false>(void*, const void*) const;

} } } // namespace nw::g3d::res
