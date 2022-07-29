#ifndef NW_G3D_MATH_VECTOR4_H_
#define NW_G3D_MATH_VECTOR4_H_

#include <nw/g3d/g3d_config.h>

namespace nw { namespace g3d { namespace math {

struct Vec4_t
{
    enum { DIM = 4, DIM_PS = DIM / 2 };

    union
    {
        struct
        {
            float x, y, z, w;
        };
        float a[DIM];
#if defined( __ghs__ )
        f32x2 ps[DIM_PS];
#endif // defined( __ghs__ )
    };
};

class Vec4 : public Vec4_t
{
public:
    static Vec4 Make(float x, float y, float z, float w);
    static Vec4* Cast(float* a);
    static const Vec4* Cast(const float* a);

    Vec4& Set(float x, float y, float z, float w);
    Vec4& Set(const float* a);
    Vec4& Set(const Vec4& v);
    Vec4& Zero();
    Vec4& Neg(const Vec4& v);
    Vec4& Rcp(const Vec4& v);

    Vec4& Add(const Vec4& lhs, const Vec4& rhs);
    Vec4& Sub(const Vec4& lhs, const Vec4& rhs);
    Vec4& Mul(const Vec4& lhs, const Vec4& rhs);
    Vec4& Div(const Vec4& lhs, const Vec4& rhs);
    Vec4& Mul(const Vec4& lhs, float rhs);
    Vec4& Div(const Vec4& lhs, float rhs);

    static float Length(const Vec4& v);
    static float LengthSq(const Vec4& v);
    static float Distance(const Vec4& lhs, const Vec4& rhs);
    static float DistanceSq(const Vec4& lhs, const Vec4& rhs);
    static float Dot(const Vec4& lhs, const Vec4& rhs);
    float Normalize(const Vec4& v);
};

} } } // namespace nw::g3d::math

#include "inl/g3d_Vector4-inl.h"

#endif // NW_G3D_MATH_VECTOR4_H_
