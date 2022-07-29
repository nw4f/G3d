#include <nw/g3d/math/g3d_MathCommon.h>
#include <nw/g3d/math/g3d_Vector3.h>
#include <nw/g3d/math/g3d_Vector4.h>
#include <nw/g3d/math/g3d_Quaternion.h>

namespace nw { namespace g3d { namespace math {

NW_G3D_MATH_INLINE
Mtx34 Mtx34::Make(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23)
{
    return Mtx34().Set(
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23);
}

NW_G3D_MATH_INLINE
Mtx34* Mtx34::Cast(float* a)
{
    return reinterpret_cast<Mtx34*>(a);
}

NW_G3D_MATH_INLINE
const Mtx34* Mtx34::Cast(const float* a)
{
    return reinterpret_cast<const Mtx34*>(a);
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Set(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23)
{
    v[0].Set(m00, m01, m02, m03);
    v[1].Set(m10, m11, m12, m13);
    v[2].Set(m20, m21, m22, m23);
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Set(const float* a)
{
    NW_G3D_ASSERT_NOT_NULL(a);
    for (int i = 0; i < DIM; ++i)
    {
        this->a[i] = a[i];
    }
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Set(const Mtx34& m)
{
    for (int i = 0; i < DIM_MAJOR; ++i)
    {
        v[i].Set(m.v[i]);
    }
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Set(const Vec3& x, const Vec3& y, const Vec3& z, const Vec3& w)
{
    Set(x.x, y.x, z.x, w.x, x.y, y.y, z.y, w.y, x.z, y.z, z.z, w.z);
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Zero()
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = 0.0f;
    }
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Identity()
{
    v[0].Set(1.0f, 0.0f, 0.0f, 0.0f);
    v[1].Set(0.0f, 1.0f, 0.0f, 0.0f);
    v[2].Set(0.0f, 0.0f, 1.0f, 0.0f);
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Neg(const Mtx34& m)
{
#if defined( __ghs__ )
    ps[0] = __PS_NEG(m.ps[0]);
    ps[1] = __PS_NEG(m.ps[1]);
    ps[2] = __PS_NEG(m.ps[2]);
    ps[3] = __PS_NEG(m.ps[3]);
    ps[4] = __PS_NEG(m.ps[4]);
    ps[5] = __PS_NEG(m.ps[5]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = -m.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::SetS(const Vec3& s)
{
    this->m00 = s.x;
    this->m11 = s.y;
    this->m22 = s.z;
    this->m01 = this->m02 = this->m10 = this->m12 = this->m20 = this->m21 = 0.0f;

    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::SetR(const Vec3& r)
{
    float sx, sy, sz, cx, cy, cz;

    Math::SinCos(&sx, &cx, r.x);
    Math::SinCos(&sy, &cy, r.y);
    Math::SinCos(&sz, &cz, r.z);

    float cxcz = cx * cz;
    float sxsy = sx * sy;
    float cxsz = cx * sz;

    this->m00 = cy * cz;
    this->m10 = cy * sz;
    this->m20 = -sy;

    this->m01 = sxsy * cz - cxsz;
    this->m11 = sxsy * sz + cxcz;
    this->m21 = sx * cy;

    this->m02 = cxcz * sy + sx * sz;
    this->m12 = cxsz * sy - sx * cz;
    this->m22 = cx * cy;

    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::SetR(const Quat& r)
{
    float yy2 = 2 * r.y * r.y;
    float zz2 = 2 * r.z * r.z;
    float xx2 = 2 * r.x * r.x;
    float xy2 = 2 * r.x * r.y;
    float xz2 = 2 * r.x * r.z;
    float yz2 = 2 * r.y * r.z;
    float wz2 = 2 * r.w * r.z;
    float wx2 = 2 * r.w * r.x;
    float wy2 = 2 * r.w * r.y;

    this->m00 = 1.0f - yy2 - zz2;
    this->m01 = xy2 - wz2;
    this->m02 = xz2 + wy2;

    this->m10 = xy2 + wz2;
    this->m11 = 1.0f - xx2 - zz2;
    this->m12 = yz2 - wx2;

    this->m20 = xz2 - wy2;
    this->m21 = yz2 + wx2;
    this->m22 = 1.0f - xx2 - yy2;

    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::SetT(const Vec3& t)
{
    this->m03 = t.x;
    this->m13 = t.y;
    this->m23 = t.z;

    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::SetSR(const Vec3& s, const Vec3& r)
{
    float sx, sy, sz, cx, cy, cz;

    Math::SinCos(&sx, &cx, r.x);
    Math::SinCos(&sy, &cy, r.y);
    Math::SinCos(&sz, &cz, r.z);

    float cxcz = cx * cz;
    float sxsy = sx * sy;
    float cxsz = cx * sz;

    this->m00 = s.x * cy * cz;
    this->m10 = s.y * cy * sz;
    this->m20 = s.z * (-sy);

    this->m01 = s.x * (sxsy * cz - cxsz);
    this->m11 = s.y * (sxsy * sz + cxcz);
    this->m21 = s.z * sx * cy;

    this->m02 = s.x * (cxcz * sy + sx * sz);
    this->m12 = s.y * (cxsz * sy - sx * cz);
    this->m22 = s.z * cx * cy;

    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::SetSR(const Vec3& s, const Quat& r)
{
    float yy2 = 2 * r.y * r.y;
    float zz2 = 2 * r.z * r.z;
    float xx2 = 2 * r.x * r.x;
    float xy2 = 2 * r.x * r.y;
    float xz2 = 2 * r.x * r.z;
    float yz2 = 2 * r.y * r.z;
    float wz2 = 2 * r.w * r.z;
    float wx2 = 2 * r.w * r.x;
    float wy2 = 2 * r.w * r.y;

    this->m00 = s.x * (1.0f - yy2 - zz2);
    this->m01 = s.y * (xy2 - wz2);
    this->m02 = s.z * (xz2 + wy2);

    this->m10 = s.x * (xy2 + wz2);
    this->m11 = s.y * (1.0f - xx2 - zz2);
    this->m12 = s.z * (yz2 - wx2);

    this->m20 = s.x * (xz2 - wy2);
    this->m21 = s.y * (yz2 + wx2);
    this->m22 = s.z * (1.0f - xx2 - yy2);

    return *this;
}

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Add(const Mtx34& lhs, const Mtx34& rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_ADD(lhs.ps[0], rhs.ps[0]);
    ps[1] = __PS_ADD(lhs.ps[1], rhs.ps[1]);
    ps[2] = __PS_ADD(lhs.ps[2], rhs.ps[2]);
    ps[3] = __PS_ADD(lhs.ps[3], rhs.ps[3]);
    ps[4] = __PS_ADD(lhs.ps[4], rhs.ps[4]);
    ps[5] = __PS_ADD(lhs.ps[5], rhs.ps[5]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] + rhs.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Sub(const Mtx34& lhs, const Mtx34& rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_SUB(lhs.ps[0], rhs.ps[0]);
    ps[1] = __PS_SUB(lhs.ps[1], rhs.ps[1]);
    ps[2] = __PS_SUB(lhs.ps[2], rhs.ps[2]);
    ps[3] = __PS_SUB(lhs.ps[3], rhs.ps[3]);
    ps[4] = __PS_SUB(lhs.ps[4], rhs.ps[4]);
    ps[5] = __PS_SUB(lhs.ps[5], rhs.ps[5]);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] - rhs.a[i];
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Mul(const Mtx34& lhs, const Mtx34& rhs)
{
#if defined( __ghs__ )
    const int ROW = 16;

    f32x2 f0 = __PSQ_LX(&rhs,  0, 0, 0);
    f32x2 f1 = __PSQ_LX(&rhs, 16, 0, 0);
    f32x2 f2 = __PSQ_LX(&rhs, 32, 0, 0);
    f32x2 f4 = __PSQ_LX(&rhs,  8, 0, 0);
    f32x2 f5 = __PSQ_LX(&rhs, 24, 0, 0);
    f32x2 f6 = __PSQ_LX(&rhs, 40, 0, 0);
    f32x2 f7 = { 0, 1 };

    f32x2 f8, f9, f10;

    f8  = __PSQ_LX(&lhs, 0 * ROW + 0, 0, 0);
    f9  = __PS_MULS0(f0, f8);
    f10 = __PS_MULS0(f4, f8);
    f9  = __PS_MADDS1(f1, f8, f9);
    f10 = __PS_MADDS1(f5, f8, f10);
    f8  = __PSQ_LX(&lhs, 0 * ROW + 8, 0, 0);
    f9  = __PS_MADDS0(f2, f8, f9);
    f10 = __PS_MADDS0(f6, f8, f10);
    f10 = __PS_MADDS1(f7, f8, f10);
    __PSQ_STX(this, 0 * ROW + 0, f9,  0, 0);
    __PSQ_STX(this, 0 * ROW + 8, f10, 0, 0);

    f8  = __PSQ_LX(&lhs, 1 * ROW + 0, 0, 0);
    f9  = __PS_MULS0(f0, f8);
    f10 = __PS_MULS0(f4, f8);
    f9  = __PS_MADDS1(f1, f8, f9);
    f10 = __PS_MADDS1(f5, f8, f10);
    f8  = __PSQ_LX(&lhs, 1 * ROW + 8, 0, 0);
    f9  = __PS_MADDS0(f2, f8, f9);
    f10 = __PS_MADDS0(f6, f8, f10);
    f10 = __PS_MADDS1(f7, f8, f10);
    __PSQ_STX(this, 1 * ROW + 0, f9,  0, 0);
    __PSQ_STX(this, 1 * ROW + 8, f10, 0, 0);

    f8  = __PSQ_LX(&lhs, 2 * ROW + 0, 0, 0);
    f9  = __PS_MULS0(f0, f8);
    f10 = __PS_MULS0(f4, f8);
    f9  = __PS_MADDS1(f1, f8, f9);
    f10 = __PS_MADDS1(f5, f8, f10);
    f8  = __PSQ_LX(&lhs, 2 * ROW + 8, 0, 0);
    f9  = __PS_MADDS0(f2, f8, f9);
    f10 = __PS_MADDS0(f6, f8, f10);
    f10 = __PS_MADDS1(f7, f8, f10);
    __PSQ_STX(this, 2 * ROW + 0, f9,  0, 0);
    __PSQ_STX(this, 2 * ROW + 8, f10, 0, 0);

    return *this;
#else
    Mtx34 out;

    out.m00 = lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20;
    out.m01 = lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21;
    out.m02 = lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22;
    out.m03 = lhs.m00 * rhs.m03 + lhs.m01 * rhs.m13 + lhs.m02 * rhs.m23 + lhs.m03;
    out.m10 = lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20;
    out.m11 = lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21;
    out.m12 = lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22;
    out.m13 = lhs.m10 * rhs.m03 + lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13;
    out.m20 = lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20;
    out.m21 = lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21;
    out.m22 = lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22;
    out.m23 = lhs.m20 * rhs.m03 + lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23;

    return *this = out;
#endif
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Mul(const Mtx34& lhs, float rhs)
{
#if defined( __ghs__ )
    ps[0] = __PS_MULS0F(lhs.ps[0], rhs);
    ps[1] = __PS_MULS0F(lhs.ps[1], rhs);
    ps[2] = __PS_MULS0F(lhs.ps[2], rhs);
    ps[3] = __PS_MULS0F(lhs.ps[3], rhs);
    ps[4] = __PS_MULS0F(lhs.ps[4], rhs);
    ps[5] = __PS_MULS0F(lhs.ps[5], rhs);
#else
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] * rhs;
    }
#endif
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Div(const Mtx34& lhs, float rhs)
{
    float rcp = Math::Rcp(rhs);
    this->Mul(lhs, rcp);
    return *this;
}

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
float Mtx34::Det(const Mtx34& m)
{
    float det =
        m.m00 * (m.m11 * m.m22 - m.m21 * m.m12) +
        m.m01 * (m.m12 * m.m20 - m.m10 * m.m22) +
        m.m02 * (m.m10 * m.m21 - m.m20 * m.m11);
    return det;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Transpose(const Mtx34& m)
{
    float tmp;
    tmp = m.m01; m01 = m.m10; m10 = tmp;
    tmp = m.m02; m02 = m.m20; m20 = tmp;
    tmp = m.m12; m12 = m.m21; m21 = tmp;
    m00 = m.m00; m11 = m.m11; m22 = m.m22;
    m03 = m13 = m23 = 0.0f;
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Adjugate(const Mtx34& m)
{
    float r00, r01, r02, r10, r11, r12, r20, r21, r22;

    r00 =  (m.m11 * m.m22 - m.m21 * m.m12);
    r01 = -(m.m01 * m.m22 - m.m21 * m.m02);
    r02 =  (m.m01 * m.m12 - m.m11 * m.m02);

    r10 = -(m.m10 * m.m22 - m.m20 * m.m12);
    r11 =  (m.m00 * m.m22 - m.m20 * m.m02);
    r12 = -(m.m00 * m.m12 - m.m10 * m.m02);

    r20 =  (m.m10 * m.m21 - m.m20 * m.m11);
    r21 = -(m.m00 * m.m21 - m.m20 * m.m01);
    r22 =  (m.m00 * m.m11 - m.m10 * m.m01);

    m00 = r00; m01 = r01; m02 = r02;
    m10 = r10; m11 = r11; m12 = r12;
    m20 = r20; m21 = r21; m22 = r22;

    m03 = -(r00 * m.m03 + r01 * m.m13 + r02 * m.m23);
    m13 = -(r10 * m.m03 + r11 * m.m13 + r12 * m.m23);
    m23 = -(r20 * m.m03 + r21 * m.m13 + r22 * m.m23);

    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Cofactor(const Mtx34& m)
{
    float r00, r01, r02, r10, r11, r12;

    r00 =  (m.m11 * m.m22 - m.m21 * m.m12);
    r01 = -(m.m10 * m.m22 - m.m20 * m.m12);
    r02 =  (m.m10 * m.m21 - m.m20 * m.m11);

    r10 = -(m.m01 * m.m22 - m.m21 * m.m02);
    r11 =  (m.m00 * m.m22 - m.m20 * m.m02);
    r12 = -(m.m00 * m.m21 - m.m20 * m.m01);

    m20 =  (m.m01 * m.m12 - m.m11 * m.m02);
    m21 = -(m.m00 * m.m12 - m.m10 * m.m02);
    m22 =  (m.m00 * m.m11 - m.m10 * m.m01);

    m00 = r00; m01 = r01; m02 = r02;
    m10 = r10; m11 = r11; m12 = r12;
    m03 = m13 = m23 = 0.0f;

    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::Inverse(float* pDet, const Mtx34& m)
{
    NW_G3D_ASSERT_NOT_NULL(pDet);
    float det = Det(m);
    *pDet = det;
    Adjugate(m);
    Div(*this, det);
    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::InvTranspose(float* pDet, const Mtx34& m)
{
    NW_G3D_ASSERT_NOT_NULL(pDet);
    float det = Det(m);
    *pDet = det;
    Cofactor(m);
    Div(*this, det);
    return *this;
}

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
Mtx34& Mtx34::ScaleBases(const Mtx34& m, const Vec3& s)
{
#if defined( __ghs__ )
    f32x2 zw = { s.z, 1.0f };
    ps[0] = __PS_MUL(m.ps[0], s.ps[0]);
    ps[1] = __PS_MUL(m.ps[1], zw);
    ps[2] = __PS_MUL(m.ps[2], s.ps[0]);
    ps[3] = __PS_MUL(m.ps[3], zw);
    ps[4] = __PS_MUL(m.ps[4], s.ps[0]);
    ps[5] = __PS_MUL(m.ps[5], zw);
#else
    m00 = m.m00 * s.x;  m01 = m.m01 * s.y;  m02 = m.m02 * s.z; m03 = m.m03;
    m10 = m.m10 * s.x;  m11 = m.m11 * s.y;  m12 = m.m12 * s.z; m13 = m.m13;
    m20 = m.m20 * s.x;  m21 = m.m21 * s.y;  m22 = m.m22 * s.z; m23 = m.m23;
#endif
    return *this;
}

NW_G3D_MATH_INLINE
float Mtx34::ExtractBaseScale(const Mtx34& m, int axis)
{
    return Math::Sqrt(ExtractBaseScaleSq(m, axis));
}

NW_G3D_MATH_INLINE
float Mtx34::ExtractBaseScaleSq(const Mtx34& m, int axis)
{
    NW_G3D_ASSERT_INDEX_BOUNDS(axis, 4);
    float x = m.m[0][axis];
    float y = m.m[1][axis];
    float z = m.m[2][axis];
    return x * x + y * y + z * z;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::LookAt(const Vec3& camPos, const Vec3& camUp, const Vec3& target)
{
    Vec3 vLook;
    vLook.Normalize(vLook.Sub(camPos, target));

    Vec3 vRight;
    vRight.Normalize(vRight.Cross(camUp, vLook));

    Vec3 vUp;
    vUp.Cross(vLook, vRight);

    m[0][0] = vRight.x;
    m[0][1] = vRight.y;
    m[0][2] = vRight.z;
    m[0][3] = -Vec3::Dot(camPos, vRight);

    m[1][0] = vUp.x;
    m[1][1] = vUp.y;
    m[1][2] = vUp.z;
    m[1][3] = -Vec3::Dot(camPos, vUp);

    m[2][0] = vLook.x;
    m[2][1] = vLook.y;
    m[2][2] = vLook.z;
    m[2][3] = -Vec3::Dot(camPos, vLook);

    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::TexProjFrustum(float l, float r, float b, float t, float n)
{
    static const float SCALE_S = 0.5f;
    static const float SCALE_T = -0.5f;
    static const float TRANSLATE_S = 0.5f;
    static const float TRANSLATE_T = 0.5f;

    NW_G3D_ASSERT(t != b);
    NW_G3D_ASSERT(l != r);

    float reverseWidth = 1.0f / (r - l);
    m[0][0] = ((2.0f * n) * reverseWidth) * SCALE_S;
    m[0][1] = 0.0f;
    m[0][2] = (((r + l) * reverseWidth) * SCALE_S) - TRANSLATE_S;
    m[0][3] = 0.0f;

    float reverseHeight = 1.0f / (t - b);
    m[1][0] = 0.0f;
    m[1][1] = ((2.0f * n) * reverseHeight) * SCALE_T;
    m[1][2] = (((t + b) * reverseHeight) * SCALE_T) - TRANSLATE_T;
    m[1][3] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = -1.0f;
    m[2][3] = 0.0f;

    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::TexProjPerspective(
    float fovy,
    float aspect)
{
    static const float SCALE_S = 0.5f;
    static const float SCALE_T = -0.5f;
    static const float TRANSLATE_S = 0.5f;
    static const float TRANSLATE_T = 0.5f;

    NW_G3D_ASSERT((fovy > 0.0f) && (fovy < Math::Pi()));
    NW_G3D_ASSERT((aspect != 0.0f));

    float angle = fovy * 0.5f;
    float cot = 1.0f / Math::Tan(angle);

    m[0][0] = (cot / aspect) * SCALE_S;
    m[0][1] = 0.0f;
    m[0][2] = -TRANSLATE_S;
    m[0][3] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = cot * SCALE_T;
    m[1][2] = -TRANSLATE_T;
    m[1][3] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = -1.0f;
    m[2][3] = 0.0f;

    return *this;
}

NW_G3D_MATH_INLINE
Mtx34& Mtx34::TexProjOrtho(
    float l,
    float r,
    float b,
    float t)
{
    static const float SCALE_S = 0.5f;
    static const float SCALE_T = -0.5f;
    static const float TRANSLATE_S = 0.5f;
    static const float TRANSLATE_T = 0.5f;

    NW_G3D_ASSERT(t != b);
    NW_G3D_ASSERT(l != r);

    float reverseWidth = 1.0f / (r - l);
    m[0][0] = 2.0f * reverseWidth * SCALE_S;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = ((-(r + l) * reverseWidth) * SCALE_S) + TRANSLATE_S;

    float reverseHeight = 1.0f / (t - b);
    m[1][0] = 0.0f;
    m[1][1] = (2.0f * reverseHeight) * SCALE_T;
    m[1][2] = 0.0f;
    m[1][3] = ((-(t + b) * reverseHeight) * SCALE_T) + TRANSLATE_T;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 0.0f;
    m[2][3] = 1.0f;

    return *this;
}

} } } // namespace nw::g3d::math
