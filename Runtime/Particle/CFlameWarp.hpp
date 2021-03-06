#ifndef __URDE_CFLAMEWARP_HPP__
#define __URDE_CFLAMEWARP_HPP__

#include "CWarp.hpp"

namespace urde
{
class CStateManager;

class CFlameWarp : public CWarp
{
    rstl::reserved_vector<zeus::CVector3f, 9> x4_vecs;
    zeus::CVector3f x74_warpPoint;
    zeus::CVector3f x80_floatingPoint;
    float x8c_maxDistSq = 0.f;
    float x90_minSize = FLT_MAX;
    float x94_maxSize = FLT_MIN;
    float x98_maxInfluenceDistSq;
    CStateManager* x9c_stateMgr = nullptr;
    bool xa0_24_activated : 1;
    bool xa0_25_collisionWarp : 1;
    bool xa0_26_processed : 1;
public:
    CFlameWarp(float maxInfluenceDist, const zeus::CVector3f& warpPoint, bool collisionWarp)
    : x74_warpPoint(warpPoint), x80_floatingPoint(warpPoint),
      x98_maxInfluenceDistSq(maxInfluenceDist * maxInfluenceDist)
    {
        x4_vecs.resize(9, warpPoint);
        xa0_24_activated = false;
        xa0_25_collisionWarp = collisionWarp;
        xa0_26_processed = false;
    }

    bool UpdateWarp() { return xa0_24_activated; }
    void ModifyParticles(std::vector<CParticle>& particles);
    void Activate(bool val) { xa0_24_activated = val; }
    bool IsActivated() { return xa0_24_activated; }
    FourCC Get4CharID() { return FOURCC('FWRP'); }
};
}

#endif // __URDE_CFLAMEWARP_HPP__
