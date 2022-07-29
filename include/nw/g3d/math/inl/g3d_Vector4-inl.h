#include <nw/g3d/math/g3d_MathCommon.h>

namespace nw { namespace g3d { namespace math {

NW_G3D_MATH_INLINE
Vec4 Vec4::Make(float x, float y, float z, float w)
{
    return Vec4().Set(x, y, z, w);
}

NW_G3D_MATH_INLINE
Vec4* Vec4::Cast(float* a)
{
    return reinterpret_cast<Vec4*>(a);
}

NW_G3D_MATH_INLINE
const Vec4* Vec4::Cast(const float* a)
{
    return reinterpret_cast<const Vec4*>(a);
}

NW_G3D_MATH_INLINE
Vec4& Vec4::Set(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    return *this;
}

NW_G3D_MATH_INLINE
Vec4& Vec4::Set(const float* a)
{
    NW_G3D_ASSERT_NOT_NULL(a);
    for (int i = 0; i < DIM; ++i)
    {
        this->a[i] = a[i];
    }
    return *this;
}

NW_G3D_MATH_INLINE
Vec4& Vec4::Set(const Vec4& v)
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = v.a[i];
    }
    return *this;
}

NW_G3D_MATH_INLINE
Vec4& Vec4::Zero()
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = 0.0f;
    }
    return *this;
}

NW_G3D_MATH_INLINE
Vec4& Vec4::Neg(const Vec4& v)
{
#if defined( __ghs__ )
    ps[0] = __PS_NEG(v.ps[0]);
    ps[1] = __PS_NEG(v.ps[1]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = -v.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec4& Vec4::Rcp(const Vec4& v)
{
#if defined( __ghs__ )
    ps[0] = Math::Rcp(v.ps[0]);
    ps[1] = Math::Rcp(v.ps[1]);
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
Vec4& Vec4::Add(const Vec4& lhs, const Vec4& rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_ADD(lhs.ps[0], rhs.ps[0]);
    ps[1] = __PS_ADD(lhs.ps[1], rhs.ps[1]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] + rhs.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec4& Vec4::Sub(const Vec4& lhs, const Vec4& rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_SUB(lhs.ps[0], rhs.ps[0]);
    ps[1] = __PS_SUB(lhs.ps[1], rhs.ps[1]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] - rhs.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec4& Vec4::Mul(const Vec4& lhs, const Vec4& rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_MUL(lhs.ps[0], rhs.ps[0]);
    ps[1] = __PS_MUL(lhs.ps[1], rhs.ps[1]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] * rhs.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec4& Vec4::Div(const Vec4& lhs, const Vec4& rhs)
{
    Vec4 rcp;
    rcp.Rcp(rhs);
    Mul(lhs, rhs);
    return *this;
}

NW_G3D_MATH_INLINE
Vec4& Vec4::Mul(const Vec4& lhs, float rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_MULS0F(lhs.ps[0], rhs);
    ps[1] = __PS_MULS0F(lhs.ps[1], rhs);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] * rhs;
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec4& Vec4::Div(const Vec4& lhs, float rhs)
{
    float rcp = Math::Rcp(rhs);
    this->Mul(lhs, rcp);
    return *this;
}

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
float Vec4::Length(const Vec4& v)
{
    return Math::Sqrt(LengthSq(v));
}

NW_G3D_MATH_INLINE
float Vec4::LengthSq(const Vec4& v)
{
    return Dot(v, v);
}

NW_G3D_MATH_INLINE
float Vec4::Distance(const Vec4& lhs, const Vec4& rhs)
{
    Vec4 vec;
    vec.Sub(lhs, rhs);
    return Length(vec);
}

NW_G3D_MATH_INLINE
float Vec4::DistanceSq(const Vec4& lhs, const Vec4& rhs)
{
    Vec4 vec;
    vec.Sub(lhs, rhs);
    return LengthSq(vec);
}

NW_G3D_MATH_INLINE
float Vec4::Dot(const Vec4& lhs, const Vec4& rhs)
{
#if defined( __ghs__ )
    f32x2 ps = __PS_MUL(lhs.ps[0], rhs.ps[0]);
    ps = __PS_MADD(lhs.ps[1], rhs.ps[1], ps);
    ps = __PS_SUM0(ps, ps, ps);
    return ps[0];
#else
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
#endif
}

NW_G3D_MATH_INLINE
float Vec4::Normalize(const Vec4& v)
{
    float lengthSq = v.LengthSq(v);
    if (lengthSq > 0.0f)
    {
        float rcp = Math::RSqrt(lengthSq);
        Mul(v, rcp);
        return lengthSq * rcp;
    }
    return 0.0f;
}

} } } // namespace nw::g3d::math
