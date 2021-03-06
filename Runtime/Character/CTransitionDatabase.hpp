#ifndef __URDE_CTRANSITIONDATABASE_HPP__
#define __URDE_CTRANSITIONDATABASE_HPP__

#include "../RetroTypes.hpp"

namespace urde
{
class IMetaTrans;

class CTransitionDatabase
{
public:
    virtual const std::shared_ptr<IMetaTrans>& GetMetaTrans(u32, u32) const=0;
};

}

#endif // __URDE_CTRANSITIONDATABASE_HPP__
