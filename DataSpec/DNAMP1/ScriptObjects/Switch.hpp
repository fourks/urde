#ifndef _DNAMP1_SWITCH_HPP_
#define _DNAMP1_SWITCH_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1
{
struct Switch : IScriptObject
{
    AT_DECL_DNA_YAML
    AT_DECL_DNAV
    String<-1> name;
    Value<bool> unknown1;
    Value<bool> unknown2;
    Value<bool> unknown3;
};
}

#endif
