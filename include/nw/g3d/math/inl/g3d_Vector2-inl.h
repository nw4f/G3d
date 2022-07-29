#include <nw/g3d/math/g3d_MathCommon.h>

namespace nw { namespace g3d { namespace math {

NW_G3D_MATH_INLINE
Vec2 Vec2::Make(float x, float y)
{
    return Vec2().Set(x, y);
}

NW_G3D_MATH_INLINE
Vec2* Vec2::Cast(float* a)
{
    return reinterpret_cast<Vec2*>(a);
}

NW_G3D_MATH_INLINE
const Vec2* Vec2::Cast(const float* a)
{
    return reinterpret_cast<const Vec2*>(a);
}

NW_G3D_MATH_INLINE
Vec2& Vec2::Set(float x, float y)
{
    this->x = x;
    this->y = y;
    return *this;
}

NW_G3D_MATH_INLINE
Vec2& Vec2::Set(const float* a)
{
    NW_G3D_ASSERT_NOT_NULL(a);
    for (int i = 0; i < DIM; ++i)
    {
        this->a[i] = a[i];
    }
    return *this;
}

NW_G3D_MATH_INLINE
Vec2& Vec2::Set(const Vec2& v)
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = v.a[i];
    }
    return *this;
}

NW_G3D_MATH_INLINE
Vec2& Vec2::Zero()
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = 0.0f;
    }
    return *this;
}

NW_G3D_MATH_INLINE
Vec2& Vec2::Neg(const Vec2& v)
{
#if defined( __ghs__ )
    ps[0] = __PS_NEG(v.ps[0]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = -v.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec2& Vec2::Rcp(const Vec2& v)
{
#if defined( __ghs__ )
    ps[0] = Math::Rcp(v.ps[0]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = Math::Rcp(v.a[i]);
    }
#endif
    return *this;
}


//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
Vec2& Vec2::Add(const Vec2& lhs, const Vec2& rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_ADD(lhs.ps[0], rhs.ps[0]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] + rhs.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec2& Vec2::Sub(const Vec2& lhs, const Vec2& rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_SUB(lhs.ps[0], rhs.ps[0]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] - rhs.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec2& Vec2::Mul(const Vec2& lhs, const Vec2& rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_MUL(lhs.ps[0], rhs.ps[0]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] * rhs.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec2& Vec2::Div(const Vec2& lhs, const Vec2& rhs)
{
    Vec2 rcp;
    rcp.Rcp(rhs);
    Mul(lhs, rhs);
    return *this;
}

NW_G3D_MATH_INLINE
Vec2& Vec2::Mul(const Vec2& lhs, float rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_MULS0F(lhs.ps[0], rhs);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] * rhs;
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec2& Vec2::Div(const Vec2& lhs, float rhs)
{
    float rcp = Math::Rcp(rhs);
    this->Mul(lhs, rcp);
    return *this;
}

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
float Vec2::Length(const Vec2& v)
{
    return Math::Sqrt(LengthSq(v));
}

NW_G3D_MATH_INLINE
float Vec2::LengthSq(const Vec2& v)
{
    return Dot(v, v);
}

NW_G3D_MATH_INLINE
float Vec2::Distance(const Vec2& lhs, const Vec2& rhs)
{
    Vec2 vec;
    vec.Sub(lhs, rhs);
    return Length(vec);
}

NW_G3D_MATH_INLINE
float Vec2::DistanceSq(const Vec2& lhs, const Vec2& rhs)
{
    Vec2 vec;
    vec.Sub(lhs, rhs);
    return LengthSq(vec);
}

NW_G3D_MATH_INLINE
float Vec2::Dot(const Vec2& lhs, const Vec2& rhs)
{
#if defined( __ghs__ )
    f32x2 ps;
    ps = __PS_MUL(lhs.ps[0], rhs.ps[0]);
    ps = __PS_SUM0(ps, ps, ps);
    return ps[0];
#else
    return lhs.x * rhs.x + lhs.y * rhs.y;
#endif
}

NW_G3D_MATH_INLINE
float Vec2::Normalize(const Vec2& v)
{
    float lengthSq = LengthSq(v);
    if (lengthSq > 0.0f)
    {
        float rcp = Math::RSqrt(lengthSq);
        Mul(v, rcp);
        return lengthSq * rcp;
    }
    return 0.0f;
}


} } } // namespace nw::g3d::math
