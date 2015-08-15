#ifndef __DNAMP3_HPP__
#define __DNAMP3_HPP__

#include "../DNACommon/DNACommon.hpp"
#include "PAK.hpp"

namespace Retro
{
namespace DNAMP3
{

extern LogVisor::LogModule Log;

/* MP3-specific, one-shot PAK traversal/extraction class */
class PAKBridge
{
    HECL::Database::Project& m_project;
    const NOD::DiscBase::IPartition::Node& m_node;
    PAK m_pak;
    HECL::SystemString m_levelString;
public:
    PAKBridge(HECL::Database::Project& project, const NOD::DiscBase::IPartition::Node& node);
    void build();
    static ResExtractor<PAKBridge> LookupExtractor(const PAK::Entry& entry);
    inline const std::string& getName() const {return m_node.getName();}
    inline HECL::SystemString getLevelString() const {return m_levelString;}

    typedef PAK PAKType;
    inline const PAKType& getPAK() const {return m_pak;}
    inline const NOD::DiscBase::IPartition::Node& getNode() const {return m_node;}
};

}
}

#endif // __DNAMP3_HPP__