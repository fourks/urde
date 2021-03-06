#ifndef __URDE_CSOUNDPOINODE_HPP__
#define __URDE_CSOUNDPOINODE_HPP__

#include "CPOINode.hpp"
#include "CCharAnimTime.hpp"

namespace urde
{
class IAnimSourceInfo;

class CSoundPOINode : public CPOINode
{
    u32 x38_sfxId;
    float x3c_falloff;
    float x40_maxDist;
public:
    CSoundPOINode();
    CSoundPOINode(CInputStream& in);
    CSoundPOINode(std::string_view name, EPOIType type,
                  const CCharAnimTime& time, u32 b, bool c,
                  float d, u32 e, u32 f, u32 sfxId, float falloff, float maxDist);

    static CSoundPOINode CopyNodeMinusStartTime(const CSoundPOINode& node,
                                                const CCharAnimTime& startTime);
    u32 GetSfxId() const { return x38_sfxId; }
    float GetFalloff() const { return x3c_falloff; }
    float GetMaxDist() const { return x40_maxDist; }
};

}

#endif // __URDE_CSOUNDPOINODE_HPP__
