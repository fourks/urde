#ifndef _DNAMP1_CMDL_HPP_
#define _DNAMP1_CMDL_HPP_

#include "../DNACommon/DNACommon.hpp"
#include "../DNACommon/CMDL.hpp"
#include "CMDLMaterials.hpp"
#include "DNAMP1.hpp"
#include "CINF.hpp"
#include "CSKR.hpp"

#include <athena/FileReader.hpp>

namespace DataSpec::DNAMP1
{

struct CMDL
{
    static bool Extract(const SpecBase& dataSpec,
                        PAKEntryReadStream& rs,
                        const hecl::ProjectPath& outPath,
                        PAKRouter<PAKBridge>& pakRouter,
                        const PAK::Entry& entry,
                        bool force,
                        hecl::blender::Token& btok,
                        std::function<void(const hecl::SystemChar*)> fileChanged);

    static void Name(const SpecBase& dataSpec,
                     PAKEntryReadStream& rs,
                     PAKRouter<PAKBridge>& pakRouter,
                     PAK::Entry& entry)
    {
        DNACMDL::NameCMDL<PAKRouter<PAKBridge>, MaterialSet>(rs, pakRouter, entry, dataSpec);
    }

    static bool Cook(const hecl::ProjectPath& outPath,
                     const hecl::ProjectPath& inPath,
                     const DNACMDL::Mesh& mesh);

    static bool HMDLCook(const hecl::ProjectPath& outPath,
                         const hecl::ProjectPath& inPath,
                         const DNACMDL::Mesh& mesh);
};

}

#endif // _DNAMP1_CMDL_HPP_
