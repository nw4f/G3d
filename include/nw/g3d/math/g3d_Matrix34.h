#ifndef NW_G3D_MATH_MATRIX34_H_
#define NW_G3D_MATH_MATRIX34_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/math/g3d_Vector4.h>

namespace nw { namespace g3d { namespace math {

class Vec3;
class Quat;

struct Mtx34_t
{
    enum { DIM_MAJOR = 3, DIM_MINOR = 4, DIM = DIM_MAJOR * DIM_MINOR, DIM_PS = DIM / 2 };

    union
    {
        struct
        {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
        };
        float a[DIM];
        float m[DIM_MAJOR][DIM_MINOR];
        Vec4 v[DIM_MAJOR];
#if defined( __ghs__ )
        f32x2 ps[DIM_PS];
#endif // defined( __ghs__ )
    };
};

class Mtx34 : public Mtx34_t
{
public:
    static Mtx34 Make(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23);
    static Mtx34* Cast(float* a);
    static const Mtx34* Cast(const float* a);

    Mtx34& Set(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23);
    Mtx34& Set(const float* a);
    Mtx34& Set(const Mtx34& m);
    Mtx34& Set(const Vec3& x, const Vec3& y, const Vec3& z, const Vec3& w);
    Mtx34& Zero();
    Mtx34& Identity();
    Mtx34& Neg(const Mtx34& m);

    Mtx34& SetS(const Vec3& s);
    Mtx34& SetR(const Vec3& r);
    Mtx34& SetR(const Quat& r);
    Mtx34& SetT(const Vec3& t);
    Mtx34& SetSR(const Vec3& s, const Vec3& r);
    Mtx34& SetSR(const Vec3& s, const Quat& r);

    Mtx34& Add(const Mtx34& lhs, const Mtx34& rhs);
    Mtx34& Sub(const Mtx34& lhs, const Mtx34& rhs);
    Mtx34& Mul(const Mtx34& lhs, const Mtx34& rhs);
    Mtx34& Mul(const Mtx34& lhs, float rhs);
    Mtx34& Div(const Mtx34& lhs, float rhs);

    static float Det(const Mtx34& m);
    Mtx34& Transpose(const Mtx34& m);
    Mtx34& Adjugate(const Mtx34& m);
    Mtx34& Cofactor(const Mtx34& m);
    Mtx34& Inverse(float* pDet, const Mtx34& m);
    Mtx34& InvTranspose(float* pDet, const Mtx34& m);

    Mtx34& ScaleBases(const Mtx34& m, const Vec3& s);
    static float ExtractBaseScale(const Mtx34& m, int axis);
    static float ExtractBaseScaleSq(const Mtx34& m, int axis);
    Mtx34& LookAt(const Vec3& camPos, const Vec3& camUp, const Vec3& target);

    Mtx34& TexProjFrustum(float l, float r, float b, float t, float n);

    Mtx34& TexProjPerspective(float fovy, float aspect);

    Mtx34& TexProjOrtho(float l, float r, float b, float t);
};

} } } // namespace nw::g3d::math

#include "inl/g3d_Matrix34-inl.h"

#endif // NW_G3D_MATH_MATRIX34_H_
