#ifndef _DNAMP1_ATOMICBETA_HPP_
#define _DNAMP1_ATOMICBETA_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1
{
struct AtomicBeta : IScriptObject
{
    AT_DECL_DNA_YAML
    AT_DECL_DNAV
    String<-1> name;
    Value<atVec3f> location;
    Value<atVec3f> orientation;
    Value<atVec3f> scale;
    PatternedInfo patternedInfo;
    ActorParameters actorParameters;
    UniqueID32 elsc;
    UniqueID32 wpsc;
    DamageInfo damageInfo;
    UniqueID32 part;
    Value<float> unknown1;
    Value<float> unknown2;
    Value<float> unknown3;
    DamageVulnerability damageVulnerabilty;
    Value<float> unknown4;
    Value<float> unknown5;
    Value<float> unknown6;
    Value<atUint32> unknown7;
    Value<atUint32> unknown8;
    Value<atUint32> unknown9;
    Value<float> unknown10;

    void addCMDLRigPairs(PAKRouter<PAKBridge>& pakRouter,
            std::unordered_map<UniqueID32, std::pair<UniqueID32, UniqueID32>>& addTo) const
    {
        actorParameters.addCMDLRigPairs(addTo, patternedInfo.animationParameters.getCINF(pakRouter));
    }

    void nameIDs(PAKRouter<PAKBridge>& pakRouter) const
    {
        if (elsc)
        {
            PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(elsc);
            ent->name = name + "_elsc";
        }
        if (wpsc)
        {
            PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(wpsc);
            ent->name = name + "_wpsc";
        }
        if (part)
        {
            PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(part);
            ent->name = name + "_part";
        }
        patternedInfo.nameIDs(pakRouter, name + "_patterned");
        actorParameters.nameIDs(pakRouter, name + "_actp");
    }

    void gatherDependencies(std::vector<hecl::ProjectPath>& pathsOut,
                            std::vector<hecl::ProjectPath>& lazyOut) const
    {
        g_curSpec->flattenDependencies(elsc, pathsOut);
        g_curSpec->flattenDependencies(wpsc, pathsOut);
        g_curSpec->flattenDependencies(part, pathsOut);
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
