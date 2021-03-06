#ifndef __URDE_CDESTROYABLEROCK_HPP__
#define __URDE_CDESTROYABLEROCK_HPP__

#include "CAi.hpp"

namespace urde
{

class CDestroyableRock : public CAi
{
public:
    CDestroyableRock(TUniqueId id, bool active, std::string_view name, const CEntityInfo& info,
                     const zeus::CTransform& xf, CModelData&& modelData, float mass, const CHealthInfo& health,
                     const CDamageVulnerability& vulnerability, const CMaterialList& matList, CAssetId fsm,
                     const CActorParameters& actParams, const CModelData& modelData2);
};

}

#endif // __URDE_CDESTROYABLEROCK_HPP__
