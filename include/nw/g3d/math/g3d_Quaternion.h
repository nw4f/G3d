#ifndef NW_G3D_MATH_QUATERNION_H_
#define NW_G3D_MATH_QUATERNION_H_

#include <nw/g3d/g3d_config.h>

namespace nw { namespace g3d { namespace math {

class Mtx34;

struct Quat_t
{
    enum { DIM = 4, DIM_PS = DIM / 2 };

    union
    {
        struct
        {
            float x, y, z, w;
        };
        float a[4];
#if defined( __ghs__ )
        f32x2 ps[DIM_PS];
#endif // defined( __ghs__ )
    };
};

class Quat : public Quat_t
{
public:
    static Quat Make(float x, float y, float z, float w);
    static Quat* Cast(float* a);
    static const Quat* Cast(const float* a);

    Quat& Set(float x, float y, float z, float w);
    Quat& Set(const float* a);
    Quat& Set(const Mtx34& m);
    Quat& Zero();
    Quat& Identity();
    Quat& Neg(const Quat& q);

    Quat& Add(const Quat& lhs, const Quat& rhs);
    Quat& Sub(const Quat& lhs, const Quat& rhs);
    Quat& Mul(const Quat& lhs, const Quat& rhs);
    Quat& Mul(const Quat& lhs, float rhs);
    Quat& Div(const Quat& lhs, float rhs);

    static float Length(const Quat& q);
    static float LengthSq(const Quat& q);
    static float Dot(const Quat& lhs, const Quat& rhs);
    float Normalize(const Quat& q);
    Quat& Conjugate(const Quat& q);
    Quat& Inverse(const Quat& q);
    Quat& Lerp(const Quat& q0, const Quat& q1, float t);
    Quat& Slerp(const Quat& q0, const Quat& q1, float t);
};

} } } // namespace nw::g3d::math

#include "inl/g3d_Quaternion-inl.h"

#endif // NW_G3D_MATH_VECTOR3_H_
