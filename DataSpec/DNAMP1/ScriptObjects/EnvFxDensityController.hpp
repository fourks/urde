#ifndef _DNAMP1_ENVFXDENSITYCONTROLLER_HPP_
#define _DNAMP1_ENVFXDENSITYCONTROLLER_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1
{
struct EnvFxDensityController : IScriptObject
{
    AT_DECL_DNA_YAML
    AT_DECL_DNAV
    String<-1> name;
    Value<bool> unknown1;
    Value<float> unknown2;
    Value<atUint32> unknown3;
};
}

#endif
