#ifndef _DNAMP1_STREAMEDAUDIO_HPP_
#define _DNAMP1_STREAMEDAUDIO_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1
{
struct StreamedAudio : IScriptObject
{
    AT_DECL_DNA_YAML
    AT_DECL_DNAV
    String<-1> name;
    Value<bool> active;
    String<-1> fileName;
    Value<bool> noStopOnDeactivate;
    Value<float> fadeInTime;
    Value<float> fadeOutTime;
    Value<atUint32> volume;
    Value<atUint32> oneShot;
    Value<bool> music;
};
}

#endif
