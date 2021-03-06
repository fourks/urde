#include "CScriptDebris.hpp"
#include "Collision/CCollisionInfoList.hpp"
#include "Particle/CElementGen.hpp"
#include "TCastTo.hpp"

namespace urde
{

CScriptDebris::CScriptDebris(TUniqueId uid, std::string_view name, const CEntityInfo& info, const zeus::CTransform& xf,
                             CModelData&& mData, const CActorParameters& aParams, CAssetId, const zeus::CVector3f&, float,
                             const zeus::CVector3f&, const zeus::CColor&, float f1, float f2, float f3,
                             CScriptDebris::EScaleType, bool, bool, bool active)
: CPhysicsActor(uid, active, name, info, xf, std::move(mData),
                CMaterialList(EMaterialTypes::Solid, EMaterialTypes::Debris),
                mData.GetBounds(xf.getRotation()), SMoverData(f2), aParams, 0.3f, 0.1f)

{
}

CScriptDebris::CScriptDebris(TUniqueId uid, std::string_view name, const CEntityInfo& info, const zeus::CTransform& xf,
                             CModelData&& mData, const CActorParameters& aParams, float, float, float, float, float,
                             float, float, float, float, const zeus::CColor&, const zeus::CColor&, float,
                             const zeus::CVector3f&, const zeus::CVector3f&, float, float,
                             const zeus::CVector3f&, CAssetId, const zeus::CVector3f&, bool, bool,
                             CScriptDebris::EOrientationType, CAssetId, const zeus::CVector3f&, bool, bool,
                             CScriptDebris::EOrientationType, CAssetId, const zeus::CVector3f&, CScriptDebris::EOrientationType,
                             bool, bool, bool, bool active)
: CPhysicsActor(uid, active, name, info, xf, std::move(mData),
                CMaterialList(EMaterialTypes::Solid, EMaterialTypes::Debris),
                mData.GetBounds(xf.getRotation()), SMoverData(1.f), aParams, 0.3f, 0.1f)
{   
}

void CScriptDebris::Accept(IVisitor& visitor)
{
    visitor.Visit(this);
}

std::experimental::optional<zeus::CAABox> CScriptDebris::GetTouchBounds() const
{
    return {};
}

void CScriptDebris::CollidedWith(TUniqueId, const CCollisionInfoList& colList, CStateManager&)
{
    if (colList.GetCount() == 0)
        return;

    if (x282_24_)
    {
        x274_ = x270_;
        SetVelocityWR(zeus::CVector3f::skZero);
    }
    else
    {
        x2c8_collisionNormal = colList.GetItem(0).GetNormalLeft();
    }
}

}
