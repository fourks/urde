#include "CScriptWorldTeleporter.hpp"
#include "CStateManager.hpp"
#include "CWorldTransManager.hpp"
#include "CWorld.hpp"
#include "CGameState.hpp"
#include "IMain.hpp"
#include "TCastTo.hpp"

namespace urde
{
CScriptWorldTeleporter::CScriptWorldTeleporter(TUniqueId uid, std::string_view name, const CEntityInfo& info,
                                               bool active, CAssetId worldId, CAssetId areaId)
: CEntity(uid, info, active, name)
, x34_worldId(worldId)
, x38_areaId(areaId)
, x3c_type(ETeleporterType::NoTransition)
, x40_24_upElevator(false)
, x40_25_inTransition(false)
, x40_27_fadeWhite(false)
{
}

CScriptWorldTeleporter::CScriptWorldTeleporter(TUniqueId uid, std::string_view name, const CEntityInfo& info,
                                               bool active, CAssetId worldId, CAssetId areaId,
                                               CAssetId playerAncs, u32 charIdx, u32 defaultAnim,
                                               const zeus::CVector3f& playerScale, CAssetId platformModel,
                                               const zeus::CVector3f& platformScale, CAssetId backgroundModel,
                                               const zeus::CVector3f& backgroundScale, bool upElevator, u16 soundId,
                                               u8 volume, u8 panning)
: CEntity(uid, info, active, name)
, x34_worldId(worldId)
, x38_areaId(areaId)
, x3c_type(ETeleporterType::Elevator)
, x40_24_upElevator(upElevator)
, x40_25_inTransition(false)
, x40_27_fadeWhite(false)
, x50_playerAnim(playerAncs, charIdx, defaultAnim)
, x5c_playerScale(playerScale)
, x68_platformModel(platformModel)
, x6c_platformScale(platformScale)
, x78_backgroundModel(backgroundModel)
, x7c_backgroundScale(backgroundScale)
, x88_soundId(CSfxManager::TranslateSFXID(soundId))
, x8a_volume(volume)
, x8b_panning(panning)
{
}

CScriptWorldTeleporter::CScriptWorldTeleporter(TUniqueId uid, std::string_view name, const CEntityInfo& info,
                                               bool active, CAssetId worldId, CAssetId areaId, u16 soundId, u8 volume,
                                               u8 panning, CAssetId fontId, CAssetId stringId, bool fadeWhite,
                                               float charFadeIn, float charsPerSecond, float showDelay)
: CEntity(uid, info, active, name)
, x34_worldId(worldId)
, x38_areaId(areaId)
, x3c_type(ETeleporterType::Text)
, x40_24_upElevator(false)
, x40_25_inTransition(false)
, x40_27_fadeWhite(fadeWhite)
, x44_charFadeIn(charFadeIn)
, x48_charsPerSecond(charsPerSecond)
, x4c_showDelay(showDelay)
, x88_soundId(CSfxManager::TranslateSFXID(soundId))
, x8a_volume(volume)
, x8b_panning(panning)
, x8c_fontId(fontId)
, x90_stringId(stringId)
{
}

void CScriptWorldTeleporter::Accept(IVisitor& visitor)
{
    visitor.Visit(this);
}

void CScriptWorldTeleporter::AcceptScriptMsg(EScriptObjectMessage msg, TUniqueId uid, CStateManager& mgr)
{
    if (GetActive())
    {
        const std::shared_ptr<CWorldTransManager>& transMgr = mgr.WorldTransManager();
        switch(msg)
        {
        case EScriptObjectMessage::Stop:
            x40_25_inTransition = false;
            transMgr->DisableTransition();
            transMgr->SfxStop();
            break;
        case EScriptObjectMessage::Play:
            StartTransition(mgr);
            transMgr->SetSfx(x88_soundId, x8a_volume, x8b_panning);
            transMgr->SfxStart();
            break;
        case EScriptObjectMessage::SetToZero:
        {
            const auto& world = mgr.WorldNC();
            world->SetPauseState(true);
            CAssetId currentWorld = g_GameState->CurrentWorldAssetId();

            if (g_ResFactory->GetResourceTypeById(currentWorld) == SBIG('MLVL'))
            {
                StartTransition(mgr);
                g_GameState->SetCurrentWorldId(x34_worldId);
                g_GameState->CurrentWorldState().SetDesiredAreaAssetId(x38_areaId);
                g_Main->SetFlowState(EFlowState::None);
                mgr.SetShouldQuitGame(true);
            }
            else
            {
                x40_25_inTransition = false;
                transMgr->DisableTransition();
                g_GameState->SetCurrentWorldId(currentWorld);
            }
            break;
        }
        default:
            break;
        }
    }
    CEntity::AcceptScriptMsg(msg, uid, mgr);
}

void CScriptWorldTeleporter::StartTransition(CStateManager& mgr)
{
    if (!x40_25_inTransition)
    {
        const auto& transMgr = mgr.WorldTransManager();
        switch(x3c_type)
        {
        case ETeleporterType::NoTransition:
            transMgr->DisableTransition();
            break;
        case ETeleporterType::Elevator:
            if (x50_playerAnim.GetACSFile().IsValid() && x50_playerAnim.GetCharacter() != -1)
            {
                transMgr->EnableTransition(CAnimRes(x50_playerAnim.GetACSFile(), x50_playerAnim.GetCharacter(),
                                                    x5c_playerScale, x50_playerAnim.GetInitialAnimation(),true),
                                           x68_platformModel, x6c_platformScale, x78_backgroundModel,
                                           x7c_backgroundScale, x40_24_upElevator);
                x40_25_inTransition = true;
            }
            break;
        case ETeleporterType::Text:
            transMgr->EnableTransition(x8c_fontId, x90_stringId, 0, x40_27_fadeWhite, x44_charFadeIn, x48_charsPerSecond,
                                       x4c_showDelay);
            x40_25_inTransition = true;
            break;
        }
    }
}

}
