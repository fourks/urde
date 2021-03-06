#include "CCollisionSurface.hpp"

namespace urde
{
CCollisionSurface::CCollisionSurface(const zeus::CVector3f& a, const zeus::CVector3f& b, const zeus::CVector3f& c, u32 flags)
    : x0_a(a),
      xc_b(b),
      x18_c(c),
      x24_flags(flags)
{
}

zeus::CVector3f CCollisionSurface::GetNormal() const
{
    zeus::CVector3f v1 = (xc_b - x0_a).cross(x18_c - x0_a);
    return zeus::CUnitVector3f(v1, true);
}

zeus::CPlane CCollisionSurface::GetPlane() const
{
    zeus::CVector3f norm = GetNormal();
    return {norm, norm.dot(x0_a)};
}

}
