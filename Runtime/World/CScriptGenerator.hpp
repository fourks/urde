#ifndef __URDE_CSCRIPTGENERATOR_HPP__
#define __URDE_CSCRIPTGENERATOR_HPP__

#include "CEntity.hpp"
#include "zeus/CVector3f.hpp"

namespace urde
{

class CScriptGenerator : public CEntity
{
    u32 x34_spawnCount;
    union {
        struct
        {
            bool x38_24_reuseFollowers : 1;
            bool x38_25_inheritTransform : 1;
        };
        u8 dummy1 = 0;
    };
    zeus::CVector3f x3c_offset;
    float x48_minScale;
    float x4c_maxScale;

public:
    CScriptGenerator(TUniqueId uid, std::string_view name, const CEntityInfo& info, u32, bool, const zeus::CVector3f&,
                     bool, bool, float, float);

    void Accept(IVisitor& visitor);
    void AcceptScriptMsg(EScriptObjectMessage msg, TUniqueId objId, CStateManager& stateMgr);
};
}

#endif // __URDE_CSCRIPTGENERATOR_HPP__
