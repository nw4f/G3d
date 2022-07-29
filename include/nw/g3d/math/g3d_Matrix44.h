#ifndef NW_G3D_MATH_MATRIX44_H_
#define NW_G3D_MATH_MATRIX44_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/math/g3d_Vector4.h>

namespace nw { namespace g3d { namespace math {

class Mtx34;

struct Mtx44_t
{
    enum { DIM_MAJOR = 4, DIM_MINOR = 4, DIM = DIM_MAJOR * DIM_MINOR, DIM_PS = DIM / 2 };

    union
    {
        struct
        {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
        float a[DIM];
        float m[DIM_MAJOR][DIM_MINOR];
        Vec4 v[DIM_MAJOR];
#if defined( __ghs__ )
        f32x2 ps[DIM_PS];
#endif // defined( __ghs__ )
    };
};
#if defined( _MSC_VER )
#pragma warning(pop)
#endif

class Mtx44 : public Mtx44_t
{
public:
    static Mtx44 Make(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33);
    static Mtx44* Cast(float* a);
    static const Mtx44* Cast(const float* a);

    Mtx44& Set(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33);
    Mtx44& Set(const float* a);
    Mtx44& Set(const Mtx44& m);
    Mtx44& Set(const Vec4& x, const Vec4& y, const Vec4& z, const Vec4& w);
    Mtx44& Zero();
    Mtx44& Identity();
    Mtx44& Neg(const Mtx44& m);

    Mtx44& Add(const Mtx44& lhs, const Mtx44& rhs);
    Mtx44& Sub(const Mtx44& lhs, const Mtx44& rhs);
    Mtx44& Mul(const Mtx44& lhs, const Mtx44& rhs);
    Mtx44& Mul(const Mtx44& lhs, float rhs);
    Mtx44& Mul(const Mtx44& lhs, const Mtx34& rhs);
    Mtx44& Mul(const Mtx34& lhs, const Mtx44& rhs);
    Mtx44& Div(const Mtx44& lhs, float rhs);

    static float Det(const Mtx44& m);
    Mtx44& Adjugate(const Mtx44& m);
    Mtx44& Inverse(float *pDet, const Mtx44& m);

    Mtx44& Perspective(float fovy, float aspect, float n, float f);
    Mtx44& Ortho(float l, float r, float b, float t, float n, float f);
};

} } } // namespace nw::g3d::math

#include "inl/g3d_Matrix44-inl.h"

#endif // NW_G3D_MATH_MATRIX44_H_
