#ifndef __COMMON_DGRP_HPP__
#define __COMMON_DGRP_HPP__

#include "DNACommon.hpp"
#include "PAK.hpp"

namespace DataSpec::DNADGRP
{

template <class IDType>
struct AT_SPECIALIZE_PARMS(DataSpec::UniqueID32, DataSpec::UniqueID64) DGRP : BigDNA
{
    AT_DECL_DNA_YAML
    Value<atUint32> dependCount;
    struct ObjectTag : BigDNA
    {
        AT_DECL_DNA_YAML
        DNAFourCC type;
        Value<IDType> id;

        bool validate() const
        {
            if (!id.operator bool())
                return false;
            hecl::ProjectPath path = UniqueIDBridge::TranslatePakIdToPath(id);
            return path && !path.isNone();
        }
    };

    Vector<ObjectTag, AT_DNA_COUNT(dependCount)> depends;

    void validateDeps()
    {
        std::vector<ObjectTag> newDeps;
        newDeps.reserve(depends.size());
        for (const ObjectTag& tag : depends)
            if (tag.validate())
                newDeps.push_back(tag);
        depends = std::move(newDeps);
        dependCount = atUint32(depends.size());
    }
};

template <class IDType>
bool ExtractDGRP(PAKEntryReadStream& rs, const hecl::ProjectPath& outPath);
template <class IDType>
bool WriteDGRP(const DGRP<IDType>& dgrp, const hecl::ProjectPath& outPath);

}

#endif // __COMMON_DGRP_HPP__
