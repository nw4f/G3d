#include <nw/g3d/g3d_Billboard.h>

namespace nw { namespace g3d {

void Billboard::CalcWorld(Mtx34* pWorldView, const Mtx34& view, const Mtx34& world)
{
    Vec3 vy;
    vy.x = view.m00 * world.m01 + view.m01 * world.m11 + view.m02 * world.m21;
    vy.y = view.m10 * world.m01 + view.m11 * world.m11 + view.m12 * world.m21;
    vy.z = 0.0f;

    if ( vy.Normalize(vy) > 0.0f )
    {
        SetR(pWorldView, vy);
        return;
    }
    else
    {
        NW_G3D_WARNING( false, "Y axis of a 'world' billboard is parallel to eye axis." );
    }

    float* a = pWorldView->a;
    for (int i = 0; i < Mtx34::DIM; i++)
    {
        a[i] = 0.0f;
    }
}

void Billboard::CalcWorldViewpoint(Mtx34* pWorldView, const Mtx34& view, const Mtx34& world)
{
    Vec3 vx, vy, vz;
    vy.x = view.m00 * world.m01 + view.m01 * world.m11 + view.m02 * world.m21;
    vy.y = view.m10 * world.m01 + view.m11 * world.m11 + view.m12 * world.m21;
    vy.z = view.m20 * world.m01 + view.m21 * world.m11 + view.m22 * world.m21;
    vz.x = -pWorldView->m03;
    vz.y = -pWorldView->m13;
    vz.z = -pWorldView->m23;

    if ( vz.Normalize(vz) > 0.0f )
    {
        vx.Cross(vy, vz);
        if (vx.Normalize(vx) > 0.0f)
        {
            vy.Cross(vz, vx);

            SetR(pWorldView, vx, vy, vz);
            return;
        }
        else
        {
            NW_G3D_WARNING( false, "The camera is locating on Y axis of a 'world_viewpoint' billboard." );
        }
    }
    else
    {
        NW_G3D_WARNING( false, "The camera is locating at a 'world_viewpoint' billboard." );
    }

    float* a = pWorldView->a;
    for (int i = 0; i < Mtx34::DIM; i++)
    {
        a[i] = 0.0f;
    }
}

void Billboard::CalcYAxis(Mtx34* pWorldView, const Mtx34& view, const Mtx34& world)
{
    Vec3 vx, vy, vz;
    vy.x = view.m00 * world.m01 + view.m01 * world.m11 + view.m02 * world.m21;
    vy.y = view.m10 * world.m01 + view.m11 * world.m11 + view.m12 * world.m21;
    vy.z = view.m20 * world.m01 + view.m21 * world.m11 + view.m22 * world.m21;
    vx.Set(vy.y, -vy.x, 0.0f);

    float lenX = vx.Normalize(vx);
    float lenY = vy.Normalize(vy);

    if (lenX > 0.0f)
    {
        if (lenY > 0.0f )
        {
            vz.Cross(vx, vy);

            SetR(pWorldView, vx, vy, vz);
            return;
        }
        else
        {
            NW_G3D_WARNING( false, "Y axis of a 'yaxis' billboard is degenerated." );
        }
    }
    else
    {
        NW_G3D_WARNING( false, "Y axis of a 'yaxis' billboard is parallel to eye axis." );
    }

    float* a = pWorldView->a;
    for (int i = 0; i < Mtx34::DIM; i++)
    {
        a[i] = 0.0f;
    }
}

void Billboard::CalcYAxisViewpoint(Mtx34* pWorldView, const Mtx34& view, const Mtx34& world)
{
    Vec3 vx, vy, vz;
    vy.x = view.m00 * world.m01 + view.m01 * world.m11 + view.m02 * world.m21;
    vy.y = view.m10 * world.m01 + view.m11 * world.m11 + view.m12 * world.m21;
    vy.z = view.m20 * world.m01 + view.m21 * world.m11 + view.m22 * world.m21;
    vz.x = -pWorldView->m03;
    vz.y = -pWorldView->m13;
    vz.z = -pWorldView->m23;

    if ( vy.Normalize(vy) > 0.0f )
    {
        vx.Cross(vy, vz);
        if (vx.Normalize(vx) > 0.0f)
        {
            vz.Cross(vx, vy);

            SetR(pWorldView, vx, vy, vz);
            return;
        }
        else
        {
            NW_G3D_WARNING( false, "The camera is locating on Y axis of a 'yaxis_viewpoint' billboard." );
        }
    }
    else
    {
        NW_G3D_WARNING( false, "Y axis of a 'yaxis_viewpoint' billboard is degenerated." );
    }

    float* a = pWorldView->a;
    for (int i = 0; i < Mtx34::DIM; i++)
    {
        a[i] = 0.0f;
    }
}

void Billboard::CalcScreen(Mtx34* pWorldView, const Mtx34& local)
{
    Vec3 vy;
    vy.x = local.m01;
    vy.y = local.m11;
    vy.z = 0.0f;

    if ( vy.Normalize(vy) > 0.0f )
    {
        SetR(pWorldView, vy);
        return;
    }
    else
    {
        NW_G3D_WARNING( false, "Y axis of a 'screen' billboard is parallel to eye axis." );
    }

    float* a = pWorldView->a;
    for (int i = 0; i < Mtx34::DIM; i++)
    {
        a[i] = 0.0f;
    }
}

void Billboard::CalcScreenViewpoint(Mtx34* pWorldView, const Mtx34& local)
{
    Vec3 vx, vy, vz;
    vy.x = local.m01;
    vy.y = local.m11;
    vy.z = 0.0f;
    vz.x = -pWorldView->m03;
    vz.y = -pWorldView->m13;
    vz.z = -pWorldView->m23;

    if ( vz.Normalize(vz) > 0.0f )
    {
        vx.Cross(vy, vz);
        if (vx.Normalize(vx) > 0.0f)
        {
            vy.Cross(vz, vx);

            SetR(pWorldView, vx, vy, vz);
            return;
        }
        else
        {
            NW_G3D_WARNING( false, "The camera is locating on Y axis of a 'screen_viewpoint' billboard." );
        }
    }
    else
    {
        NW_G3D_WARNING( false, "The camera is locating at a 'screen_viewpoint' billboard." );
    }

    float* a = pWorldView->a;
    for (int i = 0; i < Mtx34::DIM; i++)
    {
        a[i] = 0.0f;
    }
}

void Billboard::Calc(bit32 billboardMode, Mtx34* pWorldView,
    const Mtx34& view, const Mtx34& world, const Mtx34& local)
{
    NW_G3D_ASSERT(ResBone::BILLBOARD_WORLD_VIEWVECTOR <= billboardMode &&
        billboardMode <= ResBone::BILLBOARD_MAX);

    Vec3 s;
    s.x = Mtx34::ExtractBaseScale(world, 0);
    s.y = Mtx34::ExtractBaseScale(world, 1);
    s.z = Mtx34::ExtractBaseScale(world, 2);

    static void (* const pCalcBillboard[])(Mtx34*, const Mtx34&, const Mtx34&, const Mtx34&) = {
        &Billboard::CalcWorld,
        &Billboard::CalcWorldViewpoint,
        &Billboard::CalcScreen,
        &Billboard::CalcScreenViewpoint,
        &Billboard::CalcYAxis,
        &Billboard::CalcYAxisViewpoint
    };

    int funcIndex = (billboardMode - ResBone::BILLBOARD_WORLD_VIEWVECTOR) >> ResBone::BILLBOARD_SHIFT;
    pCalcBillboard[funcIndex](pWorldView, view, world, local);

    pWorldView->ScaleBases(*pWorldView, s);
}

void Billboard::SetR(Mtx34* pWorldView, const Vec3& vy)
{
    pWorldView->m00 = vy.y;
    pWorldView->m01 = vy.x;
    pWorldView->m02 = 0.0f;

    pWorldView->m10 = -vy.x;
    pWorldView->m11 =  vy.y;
    pWorldView->m12 = 0.0f;

    pWorldView->m20 = 0.0f;
    pWorldView->m21 = 0.0f;
    pWorldView->m22 = 1.0f;
}

void Billboard::SetR(Mtx34* pWorldView, const Vec3& vx, const Vec3& vy, const Vec3& vz)
{
    pWorldView->m00 = vx.x;
    pWorldView->m01 = vy.x;
    pWorldView->m02 = vz.x;

    pWorldView->m10 = vx.y;
    pWorldView->m11 = vy.y;
    pWorldView->m12 = vz.y;

    pWorldView->m20 = vx.z;
    pWorldView->m21 = vy.z;
    pWorldView->m22 = vz.z;
}

} } // namespace nw::g3d
