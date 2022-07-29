#ifndef NW_G3D_MATH_VECTOR2_H_
#define NW_G3D_MATH_VECTOR2_H_

#include <nw/g3d/g3d_config.h>

namespace nw { namespace g3d { namespace math {

struct Vec2_t
{
    enum { DIM = 2, DIM_PS = DIM / 2 };

    union
    {
        struct
        {
            float x, y;
        };
        float a[DIM];
#if defined( __ghs__ )
        f32x2 ps[DIM_PS];
#endif // defined( __ghs__ )
    };
};

class Vec2 : public Vec2_t
{
public:
    static Vec2 Make(float x, float y);
    static Vec2* Cast(float* a);
    static const Vec2* Cast(const float* a);

    Vec2& Set(float x, float y);
    Vec2& Set(const float* a);
    Vec2& Set(const Vec2& v);
    Vec2& Zero();
    Vec2& Neg(const Vec2& v);
    Vec2& Rcp(const Vec2& v);

    Vec2& Add(const Vec2& lhs, const Vec2& rhs);
    Vec2& Sub(const Vec2& lhs, const Vec2& rhs);
    Vec2& Mul(const Vec2& lhs, const Vec2& rhs);
    Vec2& Div(const Vec2& lhs, const Vec2& rhs);
    Vec2& Mul(const Vec2& lhs, float rhs);
    Vec2& Div(const Vec2& lhs, float rhs);

    static float Length(const Vec2& v);
    static float LengthSq(const Vec2& v);
    static float Distance(const Vec2& lhs, const Vec2& rhs);
    static float DistanceSq(const Vec2& lhs, const Vec2& rhs);
    static float Dot(const Vec2& lhs, const Vec2& rhs);
    float Normalize(const Vec2& v);
};

} } } // namespace nw::g3d::math

#include "inl/g3d_Vector2-inl.h"

#endif // NW_G3D_MATH_VECTOR2_H_
