#ifndef _DNAMP1_EYEBALL_HPP_
#define _DNAMP1_EYEBALL_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1
{
struct Eyeball : IScriptObject
{
    AT_DECL_DNA_YAML
    AT_DECL_DNAV
    String<-1> name;
    Value<atUint32> unknown1;
    Value<atVec3f> location;
    Value<atVec3f> orientation;
    Value<atVec3f> scale;
    PatternedInfo patternedInfo;
    ActorParameters actorParameters;
    Value<float> unknown2;
    Value<float> unknown3;
    UniqueID32 wpsc;
    DamageInfo damageInfo;
    UniqueID32 particle1;
    UniqueID32 particle2;
    UniqueID32 texture1;
    UniqueID32 texture2;
    Value<atUint32> unknown4;
    Value<atUint32> unknown5; // always ff
    Value<atUint32> unknown6; // always ff
    Value<atUint32> unknown7; // always ff
    Value<atUint32> unknown8;
    Value<bool> unknown9;

    void addCMDLRigPairs(PAKRouter<PAKBridge>& pakRouter,
            std::unordered_map<UniqueID32, std::pair<UniqueID32, UniqueID32>>& addTo) const
    {
        actorParameters.addCMDLRigPairs(addTo, patternedInfo.animationParameters.getCINF(pakRouter));
    }

    void nameIDs(PAKRouter<PAKBridge>& pakRouter) const
    {
        if (wpsc)
        {
            PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(wpsc);
            ent->name = name + "_wpsc";
        }
        if (particle1)
        {
            PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle1);
            ent->name = name + "_part1";
        }
        if (particle2)
        {
            PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle2);
            ent->name = name + "_part2";
        }
        if (texture1)
        {
            PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(texture1);
            ent->name = name + "_tex1";
        }
        if (texture2)
        {
            PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(texture2);
            ent->name = name + "_tex2";
        }
        patternedInfo.nameIDs(pakRouter, name + "_patterned");
        actorParameters.nameIDs(pakRouter, name + "_actp");
    }

    void gatherDependencies(std::vector<hecl::ProjectPath>& pathsOut,
                            std::vector<hecl::ProjectPath>& lazyOut) const
    {
        g_curSpec->flattenDependencies(wpsc, pathsOut);
        g_curSpec->flattenDependencies(particle1, pathsOut);
        g_curSpec->flattenDependencies(particle2, pathsOut);
        g_curSpec->flattenDependencies(texture1, pathsOut);
        g_curSpec->flattenDependencies(texture2, pathsOut);
        patternedInfo.depIDs(pathsOut);
        actorParameters.depIDs(pathsOut, lazyOut);
    }

    void gatherScans(std::vector<Scan>& scansOut) const
    {
        actorParameters.scanIDs(scansOut);
    }
};
}

#endif
