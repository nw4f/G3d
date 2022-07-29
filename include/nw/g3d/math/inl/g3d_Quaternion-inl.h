#include <nw/g3d/math/g3d_MathCommon.h>
#include <nw/g3d/math/g3d_Matrix34.h>

namespace nw { namespace g3d { namespace math {

NW_G3D_MATH_INLINE
Quat Quat::Make(float x, float y, float z, float w)
{
    return Quat().Set(x, y, z, w);
}

NW_G3D_MATH_INLINE
Quat* Quat::Cast(float* a)
{
    return reinterpret_cast<Quat*>(a);
}

NW_G3D_MATH_INLINE
const Quat* Quat::Cast(const float* a)
{
    return reinterpret_cast<const Quat*>(a);
}

NW_G3D_MATH_INLINE
Quat& Quat::Set(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    return *this;
}

NW_G3D_MATH_INLINE
Quat& Quat::Set(const float* a)
{
    NW_G3D_ASSERT_NOT_NULL(a);
    for (int i = 0; i < DIM; ++i)
    {
        this->a[i] = a[i];
    }
    return *this;
}

NW_G3D_MATH_INLINE
Quat& Quat::Set(const Mtx34& m)
{
    float trace = m.m00 + m.m11 + m.m22;
    float mag;
    if (trace >= 0.0f)
    {
        w = Math::Sqrt(trace* 0.25f + 0.25f);
        float s = Math::Rcp(w * 4.0f);
        x = (m.m21 - m.m12) * s;
        y = (m.m02 - m.m20) * s;
        z = (m.m10 - m.m01) * s;
    }
    else if ((mag = 2.0f * m.m00 - trace) >= 0.0f)
    {
        x = Math::Sqrt(mag * 0.25f + 0.25f);
        float s = Math::Rcp(x * 4.0f);
        w = (m.m21 - m.m12) * s;
        y = (m.m01 + m.m10) * s;
        z = (m.m02 + m.m20) * s;
    }
    else if ((mag = 2.0f * m.m11 - trace) >= 0.0f)
    {
        y = Math::Sqrt(mag * 0.25f + 0.25f);
        float s = Math::Rcp(y * 4.0f);
        w = (m.m02 - m.m20) * s;
        z = (m.m12 + m.m21) * s;
        x = (m.m10 + m.m01) * s;
    }
    else
    {
        mag = 2.0f * m.m22 - trace;
        z = Math::Sqrt(mag * 0.25f + 0.25f);
        float s = Math::Rcp(z * 4.0f);
        w = (m.m10 - m.m01) * s;
        x = (m.m20 + m.m02) * s;
        y = (m.m21 + m.m12) * s;
    }
    return *this;
}

NW_G3D_MATH_INLINE
Quat& Quat::Zero()
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = 0.0f;
    }
    return *this;
}

NW_G3D_MATH_INLINE
Quat& Quat::Identity()
{
    Set(0.0f, 0.0f, 0.0f, 1.0f);
    return *this;
}

NW_G3D_MATH_INLINE
Quat& Quat::Neg(const Quat& q)
{
#if defined( __ghs__ )
    ps[0] = __PS_NEG(q.ps[0]);
    ps[1] = __PS_NEG(q.ps[1]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = -q.a[i];
    }
#endif
    return *this;
}

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
Quat& Quat::Add(const Quat& lhs, const Quat& rhs)
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
Quat& Quat::Sub(const Quat& lhs, const Quat& rhs)
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
Quat& Quat::Mul(const Quat& lhs, const Quat& rhs)
{
#if defined( __ghs__ )
    const f32x2 axay = lhs.ps[0];
    const f32x2 azaw = lhs.ps[1];
    const f32x2 bxby = rhs.ps[0];
    const f32x2 bzbw = rhs.ps[1];

    const f32x2 AxAy = __PS_NEG(axay);
    const f32x2 Axay = __PS_MERGE01(AxAy, axay);
    const f32x2 AzAw = __PS_NEG(azaw);
    const f32x2 Azaw = __PS_MERGE01(AzAw, azaw);

    f32x2 oxy, ozw;

    oxy = __PS_MULS0(azaw, bxby);
    oxy = __PS_MADDS0(Axay, bzbw, oxy);
    oxy = __PS_MERGE10(oxy, oxy);
    oxy = __PS_MADDS1(Azaw, bxby, oxy);
    oxy = __PS_MADDS1(axay, bzbw, oxy);

    ozw = __PS_MULS0(AxAy, bxby);
    ozw = __PS_MADDS0(Azaw, bzbw, ozw);
    ozw = __PS_MERGE10(ozw, ozw);
    ozw = __PS_MADDS1(azaw, bzbw, ozw);
    ozw = __PS_SUB(ozw, __PS_MULS1(Axay, bxby));

    ps[0] = oxy;
    ps[1] = ozw;

    return *this;
#else
    Quat out;
    out.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
    out.x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
    out.y = lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x;
    out.z = lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w;
    return *this = out;
#endif
}

NW_G3D_MATH_INLINE
Quat& Quat::Mul(const Quat& lhs, float rhs)
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
Quat& Quat::Div(const Quat& lhs, float rhs)
{
    float rcp = Math::Rcp(rhs);
    this->Mul(lhs, rcp);
    return *this;
}

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
float Quat::Length(const Quat& q)
{
    return Math::Sqrt(LengthSq(q));
}

NW_G3D_MATH_INLINE
float Quat::LengthSq(const Quat& q)
{
    return Dot(q, q);
}

NW_G3D_MATH_INLINE
float Quat::Dot(const Quat& lhs, const Quat& rhs)
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
float Quat::Normalize(const Quat& q)
{
    float lengthSq = LengthSq(q);
    if (lengthSq > 0.0f)
    {
        float rcp = Math::RSqrt(lengthSq);
        Mul(q, rcp);
        return lengthSq * rcp;
    }
    return 0.0f;
}

NW_G3D_MATH_INLINE
Quat& Quat::Conjugate(const Quat& q)
{
    this->x = -q.x;
    this->y = -q.y;
    this->z = -q.z;
    this->w = q.w;
    return *this;
}

NW_G3D_MATH_INLINE
Quat& Quat::Inverse(const Quat& q)
{
    float lengthSq = LengthSq(q);
    if (lengthSq > 0.0f)
    {
        float rcp = Math::RSqrt(lengthSq);
        Conjugate(q);
        Mul(*this, rcp);
        return *this;
    }
    Zero();
    return *this;
}

NW_G3D_MATH_INLINE
Quat& Quat::Lerp(const Quat& q0, const Quat& q1, float t)
{
    Quat sub;
    sub.Sub(q1, q0);
#if defined( __ghs__ )
    ps[0] = __PS_MADDS0F(sub.ps[0], t, q0.ps[0]);
    ps[1] = __PS_MADDS0F(sub.ps[1], t, q0.ps[1]);
#else
    x = t * sub.x + q0.x;
    y = t * sub.y + q0.y;
    z = t * sub.z + q0.z;
    w = t * sub.w + q0.w;
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Quat& Quat::Slerp(const Quat& q0, const Quat& q1, float t)
{
    float cosTheta = Dot(q0, q1);
    float t0;
    float t1 = Math::Select(cosTheta, 1.0f, -1.0f);
    cosTheta = Math::Abs(cosTheta);

    if (cosTheta <= 0.99999f)
    {
        u32 theta = Math::AcosIdx(cosTheta);
        float sinTheta = Math::SinIdx(theta);
        float rcpSin = Math::Rcp(sinTheta);
        u32 tTheta = static_cast<u32>(t * theta);
        t0 = Math::SinIdx(theta - tTheta) * rcpSin;
        t1 *= Math::SinIdx(tTheta) * rcpSin;
    }
    else
    {
        t0 = 1.0f - t;
        t1 *= t;
    }

#if defined( __ghs__ )
    ps[0] = __PS_MADDS0F(q0.ps[0], t0, __PS_MULS0F(q1.ps[0], t1));
    ps[1] = __PS_MADDS0F(q0.ps[1], t0, __PS_MULS0F(q1.ps[1], t1));
#else
    x = t0 * q0.x + t1 * q1.x;
    y = t0 * q0.y + t1 * q1.y;
    z = t0 * q0.z + t1 * q1.z;
    w = t0 * q0.w + t1 * q1.w;
#endif
    return *this;
}

} } } // namespace nw::g3d::math
