#ifndef _DNAMP1_DOORAREA_HPP_
#define _DNAMP1_DOORAREA_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace Retro
{
namespace DNAMP1
{
struct DoorArea : IScriptObject
{
    DECL_YAML
    String<-1> name;
    Value<atVec3f> location;
    Value<atVec3f> orientation;
    Value<atVec3f> scale;
    AnimationParameters animationParameters;
    ActorParameters actorParameters;
    Value<atVec3f> unknown1;
    Value<atVec3f> unknown2;
    Value<atVec3f> unknown3;
    Value<bool> unknown4;
    Value<bool> unknown5;
    Value<bool> unknown6;
    Value<float> unknown7;
    Value<bool> unknown8;
};
}
}

#endif