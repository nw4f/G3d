#include <nw/g3d/math/g3d_MathCommon.h>
#include <nw/g3d/math/g3d_Matrix34.h>
#include <nw/g3d/math/g3d_Matrix44.h>
#include <nw/g3d/math/g3d_Quaternion.h>

namespace nw { namespace g3d { namespace math {

NW_G3D_MATH_INLINE
Vec3 Vec3::Make(float x, float y, float z)
{
    return Vec3().Set(x, y, z);
}

NW_G3D_MATH_INLINE
Vec3* Vec3::Cast(float* a)
{
    return reinterpret_cast<Vec3*>(a);
}

NW_G3D_MATH_INLINE
const Vec3* Vec3::Cast(const float* a)
{
    return reinterpret_cast<const Vec3*>(a);
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Set(const float* a)
{
    NW_G3D_ASSERT_NOT_NULL(a);
    for (int i = 0; i < DIM; ++i)
    {
        this->a[i] = a[i];
    }
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Set(const Vec3& v)
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = v.a[i];
    }
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Zero()
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = 0.0f;
    }
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Neg(const Vec3& v)
{
#if defined( __ghs__ )
    ps[0] = __PS_NEG(v.ps[0]);
    a[2] = -v.a[2];
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = -v.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Rcp(const Vec3& v)
{
#if defined( __ghs__ )
    ps[0] = Math::Rcp(v.ps[0]);
    a[2] = Math::Rcp(v.a[2]);
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
Vec3& Vec3::Add(const Vec3& lhs, const Vec3& rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_ADD(lhs.ps[0], rhs.ps[0]);
    z = lhs.z + rhs.z;
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] + rhs.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Sub(const Vec3& lhs, const Vec3& rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_SUB(lhs.ps[0], rhs.ps[0]);
    z = lhs.z - rhs.z;
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] - rhs.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Mul(const Vec3& lhs, const Vec3& rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_MUL(lhs.ps[0], rhs.ps[0]);
    z = lhs.z * rhs.z;
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] * rhs.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Div(const Vec3& lhs, const Vec3& rhs)
{
    Vec3 rcp;
    rcp.Rcp(rhs);
    Mul(lhs, rcp);
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Mul(const Vec3& lhs, float rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_MULS0F(lhs.ps[0], rhs);
    z = lhs.z * rhs;
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] * rhs;
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Mad(const Vec3& lhsMul, float rhsMul, const Vec3& add)
{
#if defined( __ghs__ )
    ps[0] = __PS_MADDS0F(lhsMul.ps[0], rhsMul, add.ps[0]);
    z = lhsMul.z * rhsMul + add.z;
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhsMul.a[i] * rhsMul + add.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Div(const Vec3& lhs, float rhs)
{
    float rcp = Math::Rcp(rhs);
    this->Mul(lhs, rcp);
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Mul(const Mtx34& lhs, const Vec3& rhs)
{
#if defined( __ghs__ )
    const f32x2 m00m01 = __PSQ_LX(&lhs,  0, 0, 0);
    const f32x2 m02m03 = __PSQ_LX(&lhs,  8, 0, 0);
    const f32x2 m10m11 = __PSQ_LX(&lhs, 16, 0, 0);
    const f32x2 m12m13 = __PSQ_LX(&lhs, 24, 0, 0);
    const f32x2 m20m21 = __PSQ_LX(&lhs, 32, 0, 0);
    const f32x2 m22m23 = __PSQ_LX(&lhs, 40, 0, 0);

    const f32x2 xy = __PSQ_LX(&rhs, 0, 0, 0);
    const f32x2 zw = __PSQ_LX(&rhs, 8, 1, 0);

    f32x2 fp0, fp1, fp2;

    fp0 = __PS_MUL(m00m01, xy);
    fp0 = __PS_MADD(m02m03, zw, fp0);
    fp0 = __PS_SUM0(fp0, fp0, fp0);

    fp1 = __PS_MUL(m10m11, xy);
    fp1 = __PS_MADD(m12m13, zw, fp1);
    fp1 = __PS_SUM1(fp1, fp0, fp1);

    fp2 = __PS_MUL(m20m21, xy);
    fp2 = __PS_MADD(m22m23, zw, fp2);
    fp2 = __PS_SUM0(fp2, fp2, fp2);

    ps[0] = fp1;
    z = fp2[0];

    return *this;
#else
    Vec3 out;
    out.x = lhs.m00 * rhs.x + lhs.m01 * rhs.y + lhs.m02 * rhs.z + lhs.m03;
    out.y = lhs.m10 * rhs.x + lhs.m11 * rhs.y + lhs.m12 * rhs.z + lhs.m13;
    out.z = lhs.m20 * rhs.x + lhs.m21 * rhs.y + lhs.m22 * rhs.z + lhs.m23;
    return *this = out;
#endif
}

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
float Vec3::Length(const Vec3& v)
{
    return Math::Sqrt(LengthSq(v));
}

NW_G3D_MATH_INLINE
float Vec3::LengthSq(const Vec3& v)
{
    return Dot(v, v);
}

NW_G3D_MATH_INLINE
float Vec3::Distance(const Vec3& lhs, const Vec3& rhs)
{
    Vec3 vec;
    vec.Sub(lhs, rhs);
    return Length(vec);
}

NW_G3D_MATH_INLINE
float Vec3::DistanceSq(const Vec3& lhs, const Vec3& rhs)
{
    Vec3 vec;
    vec.Sub(lhs, rhs);
    return LengthSq(vec);
}

NW_G3D_MATH_INLINE
float Vec3::Dot(const Vec3& lhs, const Vec3& rhs)
{
#if defined( __ghs__ )
    f32x2 ps;
    ps = __PS_MUL(lhs.ps[0], rhs.ps[0]);
    ps = __PS_SUM0(ps, ps, ps);
    return __FMADDS(lhs.z, rhs.z, ps[0]);
#else
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
#endif
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Cross(const Vec3& lhs, const Vec3& rhs)
{
    Vec3 out;
    out.x = lhs.y * rhs.z - lhs.z * rhs.y;
    out.y = lhs.z * rhs.x - lhs.x * rhs.z;
    out.z = lhs.x * rhs.y - lhs.y * rhs.x;
    return *this = out;
}

NW_G3D_MATH_INLINE
float Vec3::Normalize(const Vec3& v)
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

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
Vec3& Vec3::Transform(const Mtx34& m, const Vec3& v)
{
    return Mul(m, v);
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Rotate(const Mtx34& m, const Vec3& v)
{
#if defined( __ghs__ )
    const f32x2 m00m01 = __PSQ_LX(&m,  0, 0, 0);
    const f32x2 m02m03 = __PSQ_LX(&m,  8, 1, 0);
    const f32x2 m10m11 = __PSQ_LX(&m, 16, 0, 0);
    const f32x2 m12m13 = __PSQ_LX(&m, 24, 1, 0);
    const f32x2 m20m21 = __PSQ_LX(&m, 32, 0, 0);
    const f32x2 m22m23 = __PSQ_LX(&m, 40, 1, 0);

    const f32x2 xy = __PSQ_LX(&v, 0, 0, 0);
    const f32x2 zw = __PSQ_LX(&v, 8, 1, 0);

    f32x2 fp0, fp1, fp2;

    fp0 = __PS_MUL(m00m01, xy);
    fp0 = __PS_SUM0(fp0, fp0, fp0);
    fp0 = __PS_MADD(m02m03, zw, fp0);

    x = fp0[0];

    fp1 = __PS_MUL(m10m11, xy);
    fp1 = __PS_SUM0(fp1, fp1, fp1);
    fp1 = __PS_MADD(m12m13, zw, fp1);

    y = fp1[0];

    fp2 = __PS_MUL(m20m21, xy);
    fp2 = __PS_SUM0(fp2, fp2, fp2);
    fp2 = __PS_MADD(m22m23, zw, fp2);

    z = fp2[0];

    return *this;
#else
    Vec3 out;
    out.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z;
    out.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z;
    out.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z;
    return *this = out;
#endif
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Rotate(const Quat& q, const Vec3& v)
{
    Quat r;
    r.x =  v.z * q.y + q.w * v.x -v.y * q.z;
    r.y =  v.x * q.z + q.w * v.y -v.z * q.x;
    r.z =  v.y * q.x + q.w * v.z -v.x * q.y;
    r.w = v.x * q.x + v.y * q.y + v.z * q.z;

    this->x = q.y * r.z - q.z * r.y + r.w * q.x + q.w * r.x;
    this->y = q.z * r.x - q.x * r.z + r.w * q.y + q.w * r.y;
    this->z = q.x * r.y - q.y * r.x + r.w * q.z + q.w * r.z;
    return *this;
}

NW_G3D_MATH_INLINE
Vec3& Vec3::Project(const Mtx44& lhs, const Vec3& rhs)
{
#if defined( __ghs__ )
    const f32x2 m00m01 = __PSQ_LX(&lhs,  0, 0, 0);
    const f32x2 m02m03 = __PSQ_LX(&lhs,  8, 0, 0);
    const f32x2 m10m11 = __PSQ_LX(&lhs, 16, 0, 0);
    const f32x2 m12m13 = __PSQ_LX(&lhs, 24, 0, 0);
    const f32x2 m20m21 = __PSQ_LX(&lhs, 32, 0, 0);
    const f32x2 m22m23 = __PSQ_LX(&lhs, 40, 0, 0);
    const f32x2 m30m31 = __PSQ_LX(&lhs, 48, 0, 0);
    const f32x2 m32m33 = __PSQ_LX(&lhs, 56, 0, 0);

    const f32x2 xy = __PSQ_LX(&rhs, 0, 0, 0);
    const f32x2 zw = __PSQ_LX(&rhs, 8, 1, 0);

    f32x2 fp0, fp1, fp2, fp3;

    fp0 = __PS_MUL(m00m01, xy);
    fp0 = __PS_MADD(m02m03, zw, fp0);
    fp0 = __PS_SUM0(fp0, fp0, fp0);

    fp1 = __PS_MUL(m10m11, xy);
    fp1 = __PS_MADD(m12m13, zw, fp1);
    fp1 = __PS_SUM1(fp1, fp0, fp1);

    fp2 = __PS_MUL(m20m21, xy);
    fp2 = __PS_MADD(m22m23, zw, fp2);
    fp2 = __PS_SUM0(fp2, fp2, fp2);

    fp3 = __PS_MUL(m30m31, xy);
    fp3 = __PS_MADD(m32m33, zw, fp3);
    fp3 = __PS_SUM0(fp3, fp3, fp3);
    fp3[1] = fp3[0] = Math::Rcp(fp3[0]);

    ps[0] = __PS_MUL(fp1, fp3);
    z = fp2[0] * fp3[0];

    return *this;
#else
    Vec3 out;
    float r = Math::Rcp(lhs.m30 * rhs.x + lhs.m31 * rhs.y + lhs.m32 * rhs.z + lhs.m33);
    out.x = r * ( lhs.m00 * rhs.x + lhs.m01 * rhs.y + lhs.m02 * rhs.z + lhs.m03 );
    out.y = r * ( lhs.m10 * rhs.x + lhs.m11 * rhs.y + lhs.m12 * rhs.z + lhs.m13 );
    out.z = r * ( lhs.m20 * rhs.x + lhs.m21 * rhs.y + lhs.m22 * rhs.z + lhs.m23 );

    return *this = out;
#endif
}

} } } // namespace nw::g3d::math
