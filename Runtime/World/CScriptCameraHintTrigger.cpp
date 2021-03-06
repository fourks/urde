#include "CScriptCameraHintTrigger.hpp"
#include "CActorParameters.hpp"
#include "TCastTo.hpp"

namespace urde
{

CScriptCameraHintTrigger::CScriptCameraHintTrigger(TUniqueId uid, bool active, std::string_view name,
                                                   const CEntityInfo& info, const zeus::CVector3f& scale,
                                                   const zeus::CTransform& xf, bool b2, bool b3)
: CActor(uid, active, name, info, xf, CModelData::CModelDataNull(), CMaterialList(EMaterialTypes::Trigger),
         CActorParameters::None(), kInvalidUniqueId)
, xe8_obb(xf, scale)
, x124_scale(scale)
{
    x130_24_ = b2;
    x130_25_ = b3;
}

void CScriptCameraHintTrigger::Accept(IVisitor& visitor)
{
    visitor.Visit(this);
}

}
