#include "CRippleManager.hpp"

namespace urde
{

CRippleManager::CRippleManager(int maxRipples, float f1)
: x14_(f1)
{
    Init(maxRipples);
}

void CRippleManager::Init(int maxRipples)
{
    x4_ripples.resize(maxRipples, CRipple(kInvalidUniqueId, zeus::CVector3f::skZero, 0.f));
    for (CRipple& r : x4_ripples)
        r.SetTime(9999.f);
}

void CRippleManager::SetTime(float)
{

}

void CRippleManager::Update(float dt)
{
    for (CRipple& ripple : x4_ripples)
    {
        ripple.SetTime(ripple.GetTime() + dt);
        if (ripple.GetTime() > 9999.f)
            ripple.SetTime(9999.f);
    }
}

float CRippleManager::GetLastRippleDeltaTime(TUniqueId rippler) const
{
    float res = 9999.f;
    for (const CRipple& r : x4_ripples)
        if (r.GetUniqueId() == rippler)
            if (r.GetTime() < res)
                res = r.GetTime();
    return res;
}

void CRippleManager::AddRipple(const CRipple& ripple)
{

}

void CRippleManager::SetMaxTimeFalloff(float time)
{

}

float CRippleManager::GetMaxTimeFalloff() const
{
    return 0.f;
}

}
