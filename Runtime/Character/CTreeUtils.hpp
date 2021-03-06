#ifndef __URDE_CTREEUTILS_HPP__
#define __URDE_CTREEUTILS_HPP__

#include "RetroTypes.hpp"

namespace urde
{
class CAnimTreeNode;
struct CAnimSysContext;

class CTreeUtils
{
public:
    static std::shared_ptr<CAnimTreeNode> GetTransitionTree(const std::weak_ptr<CAnimTreeNode>& a,
                                                            const std::weak_ptr<CAnimTreeNode>& b,
                                                            const CAnimSysContext& animCtx);
};

}

#endif // __URDE_CTREEUTILS_HPP__
