#ifndef __DNAMP1_MAZESEEDS_HPP__
#define __DNAMP1_MAZESEEDS_HPP__

#include <vector>

#include "DNAMP1.hpp"

namespace DataSpec::DNAMP1
{
struct MazeSeeds : BigDNA
{
    AT_DECL_DNA_YAML
    Value<atUint32> seeds[300];
};
}

#endif // __DNAMP1_MAZESEEDS_HPP__
