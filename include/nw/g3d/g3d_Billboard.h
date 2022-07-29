#ifndef NW_G3D_BILLBOARD_H_
#define NW_G3D_BILLBOARD_H_

#include <nw/g3d/g3d_config.h>
#include <nw/g3d/math/g3d_Matrix34.h>
#include <nw/g3d/g3d_SkeletonObj.h>

namespace nw { namespace g3d {

class Billboard
{
public:
    static void CalcWorld(Mtx34* pWorldView, const Mtx34& view, const Mtx34& world);
    static void CalcWorldViewpoint(Mtx34* pWorldView, const Mtx34& view, const Mtx34& world);
    static void CalcScreen(Mtx34* pWorldView, const Mtx34& local);
    static void CalcScreenViewpoint(Mtx34* pWorldView, const Mtx34& local);
    static void CalcYAxis(Mtx34* pWorldView, const Mtx34& view, const Mtx34& world);
    static void CalcYAxisViewpoint(Mtx34* pWorldView, const Mtx34& view, const Mtx34& world);

    static void CalcWorld(Mtx34* pWorldView,
        const Mtx34& view, const Mtx34& world, const Mtx34& /*local*/)
    {
        CalcWorld(pWorldView, view, world);
    }

    static void CalcWorldViewpoint(Mtx34* pWorldView,
        const Mtx34& view, const Mtx34& world, const Mtx34& /*local*/)
    {
        CalcWorldViewpoint(pWorldView, view, world);
    }

    static void CalcScreen(Mtx34* pWorldView,
        const Mtx34& /*view*/, const Mtx34& /*world*/, const Mtx34& local)
    {
        CalcScreen(pWorldView, local);
    }

    static void CalcScreenViewpoint(Mtx34* pWorldView,
        const Mtx34& /*view*/, const Mtx34& /*world*/, const Mtx34& local)
    {
        CalcScreenViewpoint(pWorldView, local);
    }

    static void CalcYAxis(Mtx34* pWorldView,
        const Mtx34& view, const Mtx34& world, const Mtx34& /*local*/)
    {
        CalcYAxis(pWorldView, view, world);
    }

    static void CalcYAxisViewpoint(Mtx34* pWorldView,
        const Mtx34& view, const Mtx34& world, const Mtx34& /*local*/)
    {
        CalcYAxisViewpoint(pWorldView, view, world);
    }

    static void Calc(bit32 billboardMode, Mtx34* pWorldView,
        const Mtx34& view, const Mtx34& world, const Mtx34& local);

protected:
    static void SetR(Mtx34* pWorldView, const Vec3& vx, const Vec3& vy, const Vec3& vz);
    static void SetR(Mtx34* pWorldView, const Vec3& vy);
};

} } // namespace nw::g3d

#endif // NW_G3D_BILLBOARD_H_
