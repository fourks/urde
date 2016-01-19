#ifndef URDE_CAMERA_HPP
#define URDE_CAMERA_HPP
#include <CProjection.hpp>
#include <CFrustum.hpp>
#include <CQuaternion.hpp>
#include <CVector3f.hpp>
#include <Math.hpp>

namespace URDE
{
class Camera
{
    Zeus::CFrustum    m_frustum;
    Zeus::CProjection m_projection;
    Zeus::CVector3f   m_position;
    Zeus::CQuaternion m_orientation;
public:

    Camera(const Zeus::CVector3f& position, Zeus::EProjType projType=Zeus::EProjType::Perspective,
           const Zeus::CVector3f& up=Zeus::Math::kUpVec)
        : m_position(position)
    {
    }

    const Zeus::CMatrix4f& projectionMatrix() const { return m_projection.getCachedMatrix(); }
    const Zeus::CProjection& projection() const { return m_projection; }

    virtual void think()
    {

    }
};
}


#endif // URDE_CAMERA_HPP