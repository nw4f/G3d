#include <nw/g3d/math/g3d_MathCommon.h>
#include <nw/g3d/math/g3d_Vector3.h>
#include <nw/g3d/math/g3d_Vector4.h>
#include <nw/g3d/math/g3d_Matrix34.h>
#include <nw/g3d/math/g3d_Quaternion.h>

namespace nw { namespace g3d { namespace math {

NW_G3D_MATH_INLINE
Mtx44 Mtx44::Make(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33)
{
    return Mtx44().Set(
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33);
}

NW_G3D_MATH_INLINE
Mtx44* Mtx44::Cast(float* a)
{
    return reinterpret_cast<Mtx44*>(a);
}

NW_G3D_MATH_INLINE
const Mtx44* Mtx44::Cast(const float* a)
{
    return reinterpret_cast<const Mtx44*>(a);
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Set(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33)
{
    v[0].Set(m00, m01, m02, m03);
    v[1].Set(m10, m11, m12, m13);
    v[2].Set(m20, m21, m22, m23);
    v[3].Set(m30, m31, m32, m33);
    return *this;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Set(const float* a)
{
    NW_G3D_ASSERT_NOT_NULL(a);
    for (int i = 0; i < DIM; ++i)
    {
        this->a[i] = a[i];
    }
    return *this;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Set(const Mtx44& m)
{
    for (int i = 0; i < DIM_MAJOR; ++i)
    {
        v[i].Set(m.v[i]);
    }
    return *this;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Set(const Vec4& x, const Vec4& y, const Vec4& z, const Vec4& w)
{
    Set(x.x, y.x, z.x, w.x, x.y, y.y, z.y, w.y, x.z, y.z, z.z, w.z, x.w, y.w, z.w, w.w);
    return *this;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Zero()
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = 0.0f;
    }
    return *this;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Identity()
{
    v[0].Set(1.0f, 0.0f, 0.0f, 0.0f);
    v[1].Set(0.0f, 1.0f, 0.0f, 0.0f);
    v[2].Set(0.0f, 0.0f, 1.0f, 0.0f);
    v[3].Set(0.0f, 0.0f, 0.0f, 1.0f);
    return *this;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Neg(const Mtx44& m)
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = -m.a[i];
    }
    return *this;
}

//--------------------------------------------------------------------------------------------------

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Add(const Mtx44& lhs, const Mtx44& rhs)
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] + rhs.a[i];
    }
    return *this;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Sub(const Mtx44& lhs, const Mtx44& rhs)
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] - rhs.a[i];
    }
    return *this;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Mul(const Mtx44& lhs, const Mtx44& rhs)
{
    Mtx44 out;

    out.m00 = lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20 + lhs.m03 * rhs.m30;
    out.m01 = lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21 + lhs.m03 * rhs.m31;
    out.m02 = lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22 + lhs.m03 * rhs.m32;
    out.m03 = lhs.m00 * rhs.m03 + lhs.m01 * rhs.m13 + lhs.m02 * rhs.m23 + lhs.m03 * rhs.m33;
    out.m10 = lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20 + lhs.m13 * rhs.m30;
    out.m11 = lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21 + lhs.m13 * rhs.m31;
    out.m12 = lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22 + lhs.m13 * rhs.m32;
    out.m13 = lhs.m10 * rhs.m03 + lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13 * rhs.m33;
    out.m20 = lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20 + lhs.m23 * rhs.m30;
    out.m21 = lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21 + lhs.m23 * rhs.m31;
    out.m22 = lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22 + lhs.m23 * rhs.m32;
    out.m23 = lhs.m20 * rhs.m03 + lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23 * rhs.m33;
    out.m30 = lhs.m30 * rhs.m00 + lhs.m31 * rhs.m10 + lhs.m32 * rhs.m20 + lhs.m33 * rhs.m30;
    out.m31 = lhs.m30 * rhs.m01 + lhs.m31 * rhs.m11 + lhs.m32 * rhs.m21 + lhs.m33 * rhs.m31;
    out.m32 = lhs.m30 * rhs.m02 + lhs.m31 * rhs.m12 + lhs.m32 * rhs.m22 + lhs.m33 * rhs.m32;
    out.m33 = lhs.m30 * rhs.m03 + lhs.m31 * rhs.m13 + lhs.m32 * rhs.m23 + lhs.m33 * rhs.m33;

    return *this = out;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Mul(const Mtx44& lhs, float rhs)
{
    for (int i = 0; i < DIM; ++i)
    {
        a[i] = lhs.a[i] * rhs;
    }
    return *this;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Mul(const Mtx44& lhs, const Mtx34& rhs)
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

    f8  = __PSQ_LX(&lhs, 3 * ROW + 0, 0, 0);
    f9  = __PS_MULS0(f0, f8);
    f10 = __PS_MULS0(f4, f8);
    f9  = __PS_MADDS1(f1, f8, f9);
    f10 = __PS_MADDS1(f5, f8, f10);
    f8  = __PSQ_LX(&lhs, 3 * ROW + 8, 0, 0);
    f9  = __PS_MADDS0(f2, f8, f9);
    f10 = __PS_MADDS0(f6, f8, f10);
    f10 = __PS_MADDS1(f7, f8, f10);
    __PSQ_STX(this, 3 * ROW + 0, f9,  0, 0);
    __PSQ_STX(this, 3 * ROW + 8, f10, 0, 0);

    return *this;
#else
    Mtx44 out;
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
    out.m30 = lhs.m30 * rhs.m00 + lhs.m31 * rhs.m10 + lhs.m32 * rhs.m20;
    out.m31 = lhs.m30 * rhs.m01 + lhs.m31 * rhs.m11 + lhs.m32 * rhs.m21;
    out.m32 = lhs.m30 * rhs.m02 + lhs.m31 * rhs.m12 + lhs.m32 * rhs.m22;
    out.m33 = lhs.m30 * rhs.m03 + lhs.m31 * rhs.m13 + lhs.m32 * rhs.m23 + lhs.m33;

    return *this = out;
#endif
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Mul(const Mtx34& lhs, const Mtx44& rhs)
{
#if defined( __ghs__ )
    const int ROW = 16;

    f32x2 f0 = __PSQ_LX(&rhs,  0, 0, 0);
    f32x2 f1 = __PSQ_LX(&rhs, 16, 0, 0);
    f32x2 f2 = __PSQ_LX(&rhs, 32, 0, 0);
    f32x2 f3 = __PSQ_LX(&rhs, 48, 0, 0);
    f32x2 f4 = __PSQ_LX(&rhs,  8, 0, 0);
    f32x2 f5 = __PSQ_LX(&rhs, 24, 0, 0);
    f32x2 f6 = __PSQ_LX(&rhs, 40, 0, 0);
    f32x2 f7 = __PSQ_LX(&rhs, 56, 0, 0);

    f32x2 f8, f9, f10;

    f8  = __PSQ_LX(&lhs, 0 * ROW + 0, 0, 0);
    f9  = __PS_MULS0(f0, f8);
    f10 = __PS_MULS0(f4, f8);
    f9  = __PS_MADDS1(f1, f8, f9);
    f10 = __PS_MADDS1(f5, f8, f10);
    f8  = __PSQ_LX(&lhs, 0 * ROW + 8, 0, 0);
    f9  = __PS_MADDS0(f2, f8, f9);
    f10 = __PS_MADDS0(f6, f8, f10);
    f9 = __PS_MADDS1(f3, f8, f9);
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
    f9 = __PS_MADDS1(f3, f8, f9);
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
    f9 = __PS_MADDS1(f3, f8, f9);
    f10 = __PS_MADDS1(f7, f8, f10);
    __PSQ_STX(this, 2 * ROW + 0, f9,  0, 0);
    __PSQ_STX(this, 2 * ROW + 8, f10, 0, 0);

    __PSQ_STX(this, 3 * ROW + 0, f3, 0, 0);
    __PSQ_STX(this, 3 * ROW + 8, f7, 0, 0);

    return *this;
#else
    Mtx44 out;
    out.m00 = lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20 + lhs.m03 * rhs.m30;
    out.m01 = lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21 + lhs.m03 * rhs.m31;
    out.m02 = lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22 + lhs.m03 * rhs.m32;
    out.m03 = lhs.m00 * rhs.m03 + lhs.m01 * rhs.m13 + lhs.m02 * rhs.m23 + lhs.m03 * rhs.m33;
    out.m10 = lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20 + lhs.m13 * rhs.m30;
    out.m11 = lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21 + lhs.m13 * rhs.m31;
    out.m12 = lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22 + lhs.m13 * rhs.m32;
    out.m13 = lhs.m10 * rhs.m03 + lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13 * rhs.m33;
    out.m20 = lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20 + lhs.m23 * rhs.m30;
    out.m21 = lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21 + lhs.m23 * rhs.m31;
    out.m22 = lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22 + lhs.m23 * rhs.m32;
    out.m23 = lhs.m20 * rhs.m03 + lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23 * rhs.m33;
    out.m30 = rhs.m30;
    out.m31 = rhs.m31;
    out.m32 = rhs.m32;
    out.m33 = rhs.m33;

    return *this = out;
#endif
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Div(const Mtx44& lhs, float rhs)
{
    float rcp = Math::Rcp(rhs);
    this->Mul(lhs, rcp);
    return *this;
}

NW_G3D_MATH_INLINE
float Mtx44::Det(const Mtx44& m)
{
    float det =
        m.m00 * m.m11 * m.m22 * m.m33 + m.m00 * m.m12 * m.m23 * m.m31 + m.m00 * m.m13 * m.m21 * m.m32 +
        m.m01 * m.m10 * m.m23 * m.m32 + m.m01 * m.m12 * m.m20 * m.m33 + m.m01 * m.m13 * m.m22 * m.m30 +
        m.m02 * m.m10 * m.m21 * m.m33 + m.m02 * m.m11 * m.m23 * m.m30 + m.m02 * m.m13 * m.m20 * m.m31 +
        m.m03 * m.m10 * m.m22 * m.m31 + m.m03 * m.m11 * m.m20 * m.m32 + m.m03 * m.m12 * m.m21 * m.m30 -
        m.m00 * m.m11 * m.m23 * m.m32 - m.m00 * m.m12 * m.m21 * m.m33 - m.m00 * m.m13 * m.m22 * m.m31 -
        m.m01 * m.m10 * m.m22 * m.m33 - m.m01 * m.m12 * m.m23 * m.m30 - m.m01 * m.m13 * m.m20 * m.m32 -
        m.m02 * m.m10 * m.m23 * m.m31 - m.m02 * m.m11 * m.m20 * m.m33 - m.m02 * m.m13 * m.m21 * m.m30 -
        m.m03 * m.m10 * m.m21 * m.m32 - m.m03 * m.m11 * m.m22 * m.m30 - m.m03 * m.m12 * m.m20 * m.m31;
    return det;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Adjugate(const Mtx44& m)
{
    Mtx44 out;
    out.m00 = m.m11 * m.m22 * m.m33 + m.m12 * m.m23 * m.m31 + m.m13 * m.m21 * m.m32 - m.m11 * m.m23 * m.m32 - m.m12 * m.m21 * m.m33 - m.m13 * m.m22 * m.m31;
    out.m01 = m.m01 * m.m23 * m.m32 + m.m02 * m.m21 * m.m33 + m.m03 * m.m22 * m.m31 - m.m01 * m.m22 * m.m33 - m.m02 * m.m23 * m.m31 - m.m03 * m.m21 * m.m32;
    out.m02 = m.m01 * m.m12 * m.m33 + m.m02 * m.m13 * m.m31 + m.m03 * m.m11 * m.m32 - m.m01 * m.m13 * m.m32 - m.m02 * m.m11 * m.m33 - m.m03 * m.m12 * m.m31;
    out.m03 = m.m01 * m.m13 * m.m22 + m.m02 * m.m11 * m.m23 + m.m03 * m.m12 * m.m21 - m.m01 * m.m12 * m.m23 - m.m02 * m.m13 * m.m21 - m.m03 * m.m11 * m.m22;
    out.m10 = m.m10 * m.m23 * m.m32 + m.m12 * m.m20 * m.m33 + m.m13 * m.m22 * m.m30 - m.m10 * m.m22 * m.m33 - m.m12 * m.m23 * m.m30 - m.m13 * m.m20 * m.m32;
    out.m11 = m.m00 * m.m22 * m.m33 + m.m02 * m.m23 * m.m30 + m.m03 * m.m20 * m.m32 - m.m00 * m.m23 * m.m32 - m.m02 * m.m20 * m.m33 - m.m03 * m.m22 * m.m30;
    out.m12 = m.m00 * m.m13 * m.m32 + m.m02 * m.m10 * m.m33 + m.m03 * m.m12 * m.m30 - m.m00 * m.m12 * m.m33 - m.m02 * m.m13 * m.m30 - m.m03 * m.m10 * m.m32;
    out.m13 = m.m00 * m.m12 * m.m23 + m.m02 * m.m13 * m.m20 + m.m03 * m.m10 * m.m22 - m.m00 * m.m13 * m.m22 - m.m02 * m.m10 * m.m23 - m.m03 * m.m12 * m.m20;
    out.m20 = m.m10 * m.m21 * m.m33 + m.m11 * m.m23 * m.m30 + m.m13 * m.m20 * m.m31 - m.m10 * m.m23 * m.m31 - m.m11 * m.m20 * m.m33 - m.m13 * m.m21 * m.m30;
    out.m21 = m.m00 * m.m23 * m.m31 + m.m01 * m.m20 * m.m33 + m.m03 * m.m21 * m.m30 - m.m00 * m.m21 * m.m33 - m.m01 * m.m23 * m.m30 - m.m03 * m.m20 * m.m31;
    out.m22 = m.m00 * m.m11 * m.m33 + m.m01 * m.m13 * m.m30 + m.m03 * m.m10 * m.m31 - m.m00 * m.m13 * m.m31 - m.m01 * m.m10 * m.m33 - m.m03 * m.m11 * m.m30;
    out.m23 = m.m00 * m.m13 * m.m21 + m.m01 * m.m10 * m.m23 + m.m03 * m.m11 * m.m20 - m.m00 * m.m11 * m.m23 - m.m01 * m.m13 * m.m20 - m.m03 * m.m10 * m.m21;
    out.m30 = m.m10 * m.m22 * m.m31 + m.m11 * m.m20 * m.m32 + m.m12 * m.m21 * m.m30 - m.m10 * m.m21 * m.m32 - m.m11 * m.m22 * m.m30 - m.m12 * m.m20 * m.m31;
    out.m31 = m.m00 * m.m21 * m.m32 + m.m01 * m.m22 * m.m30 + m.m02 * m.m20 * m.m31 - m.m00 * m.m22 * m.m31 - m.m01 * m.m20 * m.m32 - m.m02 * m.m21 * m.m30;
    out.m32 = m.m00 * m.m12 * m.m31 + m.m01 * m.m10 * m.m32 + m.m02 * m.m11 * m.m30 - m.m00 * m.m11 * m.m32 - m.m01 * m.m12 * m.m30 - m.m02 * m.m10 * m.m31;
    out.m33 = m.m00 * m.m11 * m.m22 + m.m01 * m.m12 * m.m20 + m.m02 * m.m10 * m.m21 - m.m00 * m.m12 * m.m21 - m.m01 * m.m10 * m.m22 - m.m02 * m.m11 * m.m20;
    return *this = out;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Inverse(float* pDet, const Mtx44& m)
{
    float det = Det(m);
    *pDet = det;
    Adjugate(m);
    Div(*this, det);
    return *this;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Perspective(float fovy, float aspect, float n, float f)
{
    const float angle = fovy * 0.5f;

    const float cot = Math::Rcp(Math::Tan(angle));
    const float rcpAspect = Math::Rcp(aspect);

    m[0][0] =  cot * rcpAspect;
    m[0][1] =  0.0f;
    m[0][2] =  0.0f;
    m[0][3] =  0.0f;

    m[1][0] =  0.0f;
    m[1][1] =   cot;
    m[1][2] =  0.0f;
    m[1][3] =  0.0f;

    m[2][0] =  0.0f;
    m[2][1] =  0.0f;

    const float tmp = -Math::Rcp((f - n));
    m[2][2] = (f + n) * tmp;
    m[2][3] = 2 * f * n * tmp;

    m[3][0] =  0.0f;
    m[3][1] =  0.0f;
    m[3][2] = -1.0f;
    m[3][3] =  0.0f;

    return *this;
}

NW_G3D_MATH_INLINE
Mtx44& Mtx44::Ortho(float l, float r, float b, float t, float n, float f)
{
    register float tmp1     =  Math::Rcp(r - l);
    register float tmp2     =  Math::Rcp(t - b);
    register float tmp3     =  Math::Rcp(f - n);
    register float m00, m03, m11, m13, m22, m23;

    m00 =  2.0f * tmp1;
    m03 = -(r + l) * tmp1;

    m11 =  2.0f * tmp2;
    m13 = -(t + b) * tmp2;

    m22 = -2.0f * tmp3;
    m23 = -(f + n) * tmp3;

    m[0][1] =  0.0f;
    m[0][2] =  0.0f;

    m[1][0] =  0.0f;
    m[1][2] =  0.0f;

    m[2][0] =  0.0f;
    m[2][1] =  0.0f;

    m[3][0] =  0.0f;
    m[3][1] =  0.0f;
    m[3][2] =  0.0f;
    m[3][3] =  1.0f;

    m[0][0] =  m00;
    m[0][3] =  m03;
    m[1][1] =  m11;
    m[1][3] =  m13;
    m[2][2] =  m22;
    m[2][3] =  m23;

    return *this;
}

} } } // namespace nw::g3d::math
