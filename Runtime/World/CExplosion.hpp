#ifndef __URDE_CEXPLOSION_HPP__
#define __URDE_CEXPLOSION_HPP__

#include "CEffect.hpp"
#include "Particle/CElementGen.hpp"
#include "Particle/CGenDescription.hpp"

namespace urde
{

class CExplosion : public CEffect
{
    std::unique_ptr<CElementGen> xe8_particleGen;
    TUniqueId xec_explosionLight = kInvalidUniqueId;
    const CGenDescription* xf0_particleDesc;
    bool xf4_24_renderThermalHot:1;
    bool xf4_25_:1;
    bool xf4_26_renderXray:1;
    float xf8_time = 0.f;

public:
    CExplosion(const TLockedToken<CGenDescription>& particle, TUniqueId uid, bool active,
               const CEntityInfo& info, std::string_view name, const zeus::CTransform& xf,
               u32, const zeus::CVector3f& scale, const zeus::CColor& color);

     void Accept(IVisitor&);
     void AcceptScriptMsg(EScriptObjectMessage, TUniqueId, CStateManager&);
     void Think(float, CStateManager&);
     void PreRender(CStateManager&, const zeus::CFrustum&);
     void AddToRenderer(const zeus::CFrustum&, const CStateManager&) const;
     void Render(const CStateManager&) const;
     bool CanRenderUnsorted(const CStateManager&) const;
};

}

#endif // __URDE_CEXPLOSION_HPP__
