#ifndef _DNAMP1_MEMORYRELAY_HPP_
#define _DNAMP1_MEMORYRELAY_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1
{
struct MemoryRelay : IScriptObject
{
    AT_DECL_DNA_YAML
    AT_DECL_DNAV
    String<-1> name;
    Value<bool> unknown;
    Value<bool> active;
};
}

#endif
