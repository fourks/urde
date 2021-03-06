#ifndef __URDE_CSTATICINTERFERENCE_HPP__
#define __URDE_CSTATICINTERFERENCE_HPP__

#include <vector>
#include "RetroTypes.hpp"

namespace urde
{
class CStateManager;

struct CStaticInterferenceSource
{
    TUniqueId id;
    float magnitude;
    float timeLeft;
};

class CStaticInterference
{
    std::vector<CStaticInterferenceSource> m_sources;
public:
    CStaticInterference(int sourceCount);
    void RemoveSource(TUniqueId id);
    void Update(CStateManager&, float dt);
    float GetTotalInterference() const;
    void AddSource(TUniqueId id, float magnitude, float duration);
};

}

#endif // __URDE_CSTATICINTERFERENCE_HPP__
