#ifndef _DNACOMMON_EGMC_HPP_
#define _DNACOMMON_EGMC_HPP_

#include "DNACommon.hpp"

namespace DataSpec::DNACommon
{
struct EGMC : public BigDNA
{
    AT_DECL_DNA
    Value<atUint32> count;

    struct Object : BigDNA
    {
        AT_DECL_DNA
        Value<atUint32> mesh;
        Value<atUint32> instanceId;
    };

    Vector<Object, AT_DNA_COUNT(count)> objects;
};
}
#endif // _DNACOMMON_EGMC_HPP_
