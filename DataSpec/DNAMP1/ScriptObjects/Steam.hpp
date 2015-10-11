#ifndef _DNAMP1_STEAM_HPP_
#define _DNAMP1_STEAM_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace Retro
{
namespace DNAMP1
{
struct Steam : IScriptObject
{
    DECL_YAML
    String<-1> name;
    Value<atVec3f> location;
    Value<atVec3f> volume;
    DamageInfo damageInfo;
    Value<atVec3f> unknown1;
    Value<atUint32> unknown2;
    Value<bool> unknown3;
    UniqueID32 texture;
    Value<float> unknown4;
    Value<float> unknown5;
    Value<float> unknown6;
    Value<float> unknown7;
    Value<bool> unknown8;
};
}
}

#endif