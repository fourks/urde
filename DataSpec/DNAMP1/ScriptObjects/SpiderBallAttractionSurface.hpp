#ifndef _DNAMP1_SPIDERBALLATTRACTIONSURFACE_HPP_
#define _DNAMP1_SPIDERBALLATTRACTIONSURFACE_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1
{
struct SpiderBallAttractionSurface : IScriptObject
{
    AT_DECL_DNA_YAML
    AT_DECL_DNAV
    String<-1> name;
    Value<atVec3f> location;
    Value<atVec3f> orientation;
    Value<atVec3f> scale;
    Value<bool> active;
};
}

#endif
