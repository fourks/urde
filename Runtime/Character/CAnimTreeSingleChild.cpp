#include "CAnimTreeSingleChild.hpp"

namespace urde
{

CAnimTreeSingleChild::CAnimTreeSingleChild(const std::weak_ptr<CAnimTreeNode>& node, const std::string& name)
: CAnimTreeNode(name)
{

}

SAdvancementResults CAnimTreeSingleChild::VAdvanceView(const CCharAnimTime& a)
{
}

void CAnimTreeSingleChild::VGetTimeRemaining() const
{
}

bool CAnimTreeSingleChild::VHasOffset(const CSegId& seg) const
{
}

void CAnimTreeSingleChild::VGetOffset(const CSegId& seg) const
{
}

void CAnimTreeSingleChild::VGetRotation(const CSegId& seg) const
{
}

void CAnimTreeSingleChild::VGetBoolPOIList(const CCharAnimTime& time,
                                           CBoolPOINode* listOut,
                                           u32, u32, u32) const
{
}

void CAnimTreeSingleChild::VGetInt32POIList(const CCharAnimTime& time,
                                            CInt32POINode* listOut,
                                            u32, u32, u32) const
{
}

void CAnimTreeSingleChild::VGetParticlePOIList(const CCharAnimTime& time,
                                               CParticlePOINode* listOut,
                                               u32, u32, u32) const
{
}

void CAnimTreeSingleChild::VGetSoundPOIList(const CCharAnimTime& time,
                                            CSoundPOINode* listOut,
                                            u32, u32, u32) const
{
}

void CAnimTreeSingleChild::VGetBoolPOIState(const char*) const
{
}

void CAnimTreeSingleChild::VGetInt32POIState(const char*) const
{
}

void CAnimTreeSingleChild::VGetParticlePOIState(const char*) const
{
}

void CAnimTreeSingleChild::VGetSegStatementSet(const CSegIdList& list,
                                               CSegStatementSet& setOut) const
{
}

void CAnimTreeSingleChild::VGetSegStatementSet(const CSegIdList& list,
                                               CSegStatementSet& setOut,
                                               const CCharAnimTime& time) const
{
}

void CAnimTreeSingleChild::VSetPhase(float)
{
}

SAdvancementResults
CAnimTreeSingleChild::VGetAdvancementResults(const CCharAnimTime& a,
                                             const CCharAnimTime& b) const
{
}

void CAnimTreeSingleChild::Depth() const
{
}

void CAnimTreeSingleChild::VGetNumChildren() const
{
}

}