#include "CFlameWarp.hpp"
#include "CStateManager.hpp"

namespace urde
{

void CFlameWarp::ModifyParticles(std::vector<CParticle>& particles)
{
    if (x9c_stateMgr == 0 || particles.size() < 9)
        return;

    std::vector<std::pair<float, u8>> vec;
    vec.reserve(particles.size());

    x90_minSize = FLT_MAX;
    x94_maxSize = FLT_MIN;
    float maxTransp = 0.f;
    u8 idx = 0;
    for (CParticle& particle : particles)
    {
        float transp = 1.f - particle.x34_color.a;
        if (transp > maxTransp)
        {
            float distSq = (particle.x4_pos - x74_warpPoint).magSquared();
            if (distSq > x8c_maxDistSq && distSq < x98_maxInfluenceDistSq)
            {
                x8c_maxDistSq = distSq;
                maxTransp = transp;
                x80_floatingPoint = particle.x4_pos;
            }
        }

        if (particle.x2c_lineLengthOrSize < x90_minSize)
            x90_minSize = particle.x2c_lineLengthOrSize;
        if (particle.x2c_lineLengthOrSize > x94_maxSize)
            x94_maxSize = particle.x2c_lineLengthOrSize;

        vec.emplace_back(transp, idx);

        if (xa0_25_collisionWarp)
        {
            zeus::CVector3f delta = particle.x4_pos - particle.x10_prevPos;
            if (delta.magSquared() >= 0.0011920929f)
            {
                zeus::CVector3f deltaNorm = delta.normalized();
                zeus::CVector3f behindPos = particle.x10_prevPos - deltaNorm * 5.f;
                zeus::CVector3f fullDelta = particle.x4_pos - behindPos;
                CRayCastResult result =
                x9c_stateMgr->RayStaticIntersection(behindPos, deltaNorm, fullDelta.magnitude(),
                    CMaterialFilter::MakeIncludeExclude({EMaterialTypes::Solid},
                                                        {EMaterialTypes::ProjectilePassthrough}));
                if (result.IsValid())
                {
                    float dist = result.GetPlane().pointToPlaneDist(particle.x4_pos);
                    if (dist <= 0.f)
                    {
                        particle.x4_pos =- result.GetPlane().normal() * dist;
                        if (result.GetPlane().normal().dot(particle.x1c_vel) < 0.f)
                        {
                            zeus::CVector3f prevStepPos = particle.x4_pos - particle.x1c_vel;
                            particle.x4_pos += (-result.GetPlane().pointToPlaneDist(prevStepPos) /
                                particle.x1c_vel.dot(result.GetPlane().normal()) - 1.f) * particle.x1c_vel;
                            particle.x1c_vel -= particle.x1c_vel * 0.001f;
                        }
                    }
                }
            }
        }

        ++idx;
    }

    std::sort(vec.begin(), vec.end(), [](auto& a, auto& b) { return a.first < b.first; });

    for (int i=0 ; i<9 ; ++i)
    {
        CParticle& part = particles[vec[i].second];
        x4_vecs[i] = part.x4_pos;
        if (i > 0)
        {
            zeus::CVector3f delta = x4_vecs[i] - x4_vecs[i-1];
            if (delta.magnitude() < 0.0011920929f)
                x4_vecs[i] += delta.normalized() * 0.0011920929f;
        }
    }

    x4_vecs[0] = x74_warpPoint;
    x80_floatingPoint = x4_vecs[8];
    xa0_26_processed = true;
}

}
