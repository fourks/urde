#ifndef _DNAMP1_SHADOWPROJECT_HPP_
#define _DNAMP1_SHADOWPROJECT_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1
{
struct ShadowProjector : IScriptObject
{
    AT_DECL_DNA_YAML
    AT_DECL_DNAV
    String<-1> name;
    Value<atVec3f> location;
    Value<bool> unknown1;
    Value<float> unknown2;
    Value<atVec3f> unknown3;
    Value<float> unknown4;
    Value<float> unknown5;
    Value<float> unknown6;
    Value<bool> unknown7;
    Value<atUint32> unknown8;
};
}

#endif
