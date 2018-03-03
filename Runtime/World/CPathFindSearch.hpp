#ifndef __URDE_CPATHFINDSEARCH_HPP__
#define __URDE_CPATHFINDSEARCH_HPP__

#include "RetroTypes.hpp"
#include "CPathFindArea.hpp"

namespace urde
{

class CPathFindSearch
{
public:
    enum class EResult
    {
        Success,
        InvalidArea,
        NoSourcePoint,
        NoDestPoint,
        NoPath
    };
private:
    CPFArea* x0_area;
    rstl::reserved_vector<zeus::CVector3f, 16> x4_waypoints;
    u32 xc8_ = 0;
    EResult xcc_result;
    float xd0_chHeight;
    float xd4_chRadius;
    float xd8_padding = 10.f;
    u32 xdc_flags; // 0x2: flyer, 0x4: path-always-exists (swimmers)
    u32 xe0_indexMask;
    bool Search(rstl::reserved_vector<CPFRegion*, 4>& regs1, const zeus::CVector3f& p1,
                rstl::reserved_vector<CPFRegion*, 4>& regs2, const zeus::CVector3f& p2);
public:
    CPathFindSearch(CPFArea* area, u32 flags, u32 index, float chRadius, float chHeight);
    EResult Search(const zeus::CVector3f& p1, const zeus::CVector3f& p2);
    EResult FindClosestReachablePoint(const zeus::CVector3f& p1, zeus::CVector3f& p2) const;
};

}

#endif // __URDE_CPATHFINDSEARCH_HPP__
