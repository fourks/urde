#ifndef __URDE_CMETATRANSTRANS_HPP__
#define __URDE_CMETATRANSTRANS_HPP__

#include "IMetaTrans.hpp"
#include "IOStreams.hpp"
#include "CCharAnimTime.hpp"

namespace urde
{

class CMetaTransTrans : public IMetaTrans
{
    CCharAnimTime x4_transDur;
    bool xc_;
    bool xd_runA;
    u32 x10_flags;
public:
    CMetaTransTrans(CInputStream& in);
    EMetaTransType GetType() const {return EMetaTransType::Trans;}

    std::shared_ptr<CAnimTreeNode> VGetTransitionTree(const std::weak_ptr<CAnimTreeNode>& a,
                                                      const std::weak_ptr<CAnimTreeNode>& b,
                                                      const CAnimSysContext& animSys) const;
};

}

#endif // __URDE_CMETATRANSTRANS_HPP__
