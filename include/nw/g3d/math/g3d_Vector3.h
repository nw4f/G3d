#ifndef NW_G3D_MATH_VECTOR3_H_
#define NW_G3D_MATH_VECTOR3_H_

#include <nw/g3d/g3d_config.h>

namespace nw { namespace g3d { namespace math {

class Mtx34;
class Mtx44;
class Quat;

struct Vec3_t
{
    enum { DIM = 3, DIM_PS = DIM / 2 };

    union
    {
        struct
        {
            float x, y, z;
        };
        float a[DIM];
#if defined( __ghs__ )
        f32x2 ps[DIM_PS];
#endif // defined( __ghs__ )
    };
};

class Vec3 : public Vec3_t
{
public:
    static Vec3 Make(float x, float y, float z);
    static Vec3* Cast(float* a);
    static const Vec3* Cast(const float* a);

    Vec3& Set(float x, float y, float z);
    Vec3& Set(const float* a);
    Vec3& Set(const Vec3& v);
    Vec3& Zero();
    Vec3& Neg(const Vec3& v);
    Vec3& Rcp(const Vec3& v);

    Vec3& Add(const Vec3& lhs, const Vec3& rhs);
    Vec3& Sub(const Vec3& lhs, const Vec3& rhs);
    Vec3& Mul(const Vec3& lhs, const Vec3& rhs);
    Vec3& Div(const Vec3& lhs, const Vec3& rhs);
    Vec3& Mul(const Vec3& lhs, float rhs);
    Vec3& Div(const Vec3& lhs, float rhs);
    Vec3& Mad(const Vec3& lhsMul, float rhsMul, const Vec3& add);
    Vec3& Mul(const Mtx34& lhs, const Vec3& rhs);

    static float Length(const Vec3& v);
    static float LengthSq(const Vec3& v);
    static float Distance(const Vec3& lhs, const Vec3& rhs);
    static float DistanceSq(const Vec3& lhs, const Vec3& rhs);
    static float Dot(const Vec3& lhs, const Vec3& rhs);
    Vec3& Cross(const Vec3& lhs, const Vec3& rhs);
    float Normalize(const Vec3& v);

    Vec3& Transform(const Mtx34& m, const Vec3& v);
    Vec3& Rotate(const Mtx34& m, const Vec3& v);
    Vec3& Rotate(const Quat& q, const Vec3& v);
    Vec3& Project(const Mtx44& m, const Vec3& v);
};

} } } // namespace nw::g3d::math

#include "inl/g3d_Vector3-inl.h"

#endif // NW_G3D_MATH_VECTOR3_H_
