#ifndef __URDE_CANIMDATA_HPP__
#define __URDE_CANIMDATA_HPP__

#include "RetroTypes.hpp"
#include "CToken.hpp"
#include "CCharacterInfo.hpp"
#include "CParticleDatabase.hpp"
#include "CPoseAsTransforms.hpp"
#include "CHierarchyPoseBuilder.hpp"
#include "CAdditiveAnimPlayback.hpp"
#include "CCharLayoutInfo.hpp"
#include "CAnimPlaybackParms.hpp"
#include "IAnimReader.hpp"
#include <set>

enum class EUserEventType
{
    Projectile = 0,
    EggLay = 1,
    LoopedSoundStop = 2,
    AlignTargetPos = 3,
    AlignTargetRot = 4,
    ChangeMaterial = 5,
    Delete = 6,
    GenerateEnd = 7,
    DamageOn = 8,
    DamageOff = 9,
    AlignTargetPosStart = 10,
    DeGenerate = 11,
    Landing = 12,
    TakeOff = 13,
    FadeIn = 14,
    FadeOut = 15,
    ScreenShake = 16,
    BeginAction = 17,
    EndAction = 18,
    BecomeRagDoll = 19,
    IkLock = 20,
    IkRelease = 21,
    BreakLockOn = 22,
    BecomeShootThrough = 23,
    RemoveCollision = 24,
    ObjectPickUp = 25,
    ObjectDrop = 26,
    EventStart = 27,
    EventStop = 28,
    Activate = 29,
    Deactivate = 30,
    SoundPlay = 31,
    SoundStop = 32,
    EffectOn = 33,
    EffectOff = 34
};

namespace urde
{
class CCharLayoutInfo;
class CSkinnedModel;
class CMorphableSkinnedModel;
struct CAnimSysContext;
class CAnimationManager;
class CTransitionManager;
class CCharacterFactory;
class IMetaAnim;
struct CModelFlags;
class CVertexMorphEffect;
class CPrimitive;
class CRandom16;
class CStateManager;
class CCharAnimTime;
class CModel;
class CSkinRules;
class CAnimTreeNode;
class CSegIdList;
class CSegStatementSet;
class CBoolPOINode;
class CInt32POINode;
class CParticlePOINode;
class CSoundPOINode;
class IAnimReader;
struct SAdvancementDeltas;
struct SAdvancementResults;

class CAnimData
{
    friend class CModelData;
    friend class CActor;
    friend class CPlayerGun;
    friend class CGrappleArm;
public:
    enum class EAnimDir
    {
        Forward,
        Backward
    };
private:

    TLockedToken<CCharacterFactory> x0_charFactory;
    CCharacterInfo xc_charInfo;
    TLockedToken<CCharLayoutInfo> xcc_layoutData;
    TCachedToken<CSkinnedModel> xd8_modelData;
    TLockedToken<CMorphableSkinnedModel> xe4_iceModelData;
    std::shared_ptr<CSkinnedModel> xf4_xrayModel;
    std::shared_ptr<CSkinnedModel> xf8_infraModel;
    std::shared_ptr<CAnimSysContext> xfc_animCtx;
    std::shared_ptr<CAnimationManager> x100_animMgr;
    EAnimDir x104_animDir = EAnimDir::Forward;
    zeus::CAABox x108_aabb;
    CParticleDatabase x120_particleDB;
    CAssetId x1d8_selfId;
    zeus::CVector3f x1dc_alignPos;
    zeus::CQuaternion x1e8_alignRot;
    std::shared_ptr<CAnimTreeNode> x1f8_animRoot;
    std::shared_ptr<CTransitionManager> x1fc_transMgr;

    float x200_speedScale = 1.f;
    s32 x204_charIdx;
    u16 x208_defaultAnim;
    u32 x20c_passedBoolCount = 0;
    u32 x210_passedIntCount = 0;
    u32 x214_passedParticleCount = 0;
    u32 x218_passedSoundCount = 0;
    s32 x21c_particleLightIdx = 0;

    union
    {
        u32 x220_flags = 0;
        struct
        {
            bool x220_24_animating : 1;
            bool x220_25_loop : 1;
            bool x220_26_aligningPos : 1;
            bool x220_27_ : 1;
            bool x220_28_ : 1;
            bool x220_29_animationJustStarted : 1;
            bool x220_30_poseBuilt : 1;
            bool x220_31_poseCached : 1;
        };
    };

    CPoseAsTransforms x224_pose;
    CHierarchyPoseBuilder x2fc_poseBuilder;

    CAnimPlaybackParms x40c_playbackParms;
    rstl::reserved_vector<std::pair<u32, CAdditiveAnimPlayback>, 8> x434_additiveAnims;

    static rstl::reserved_vector<CBoolPOINode, 8> g_BoolPOINodes;
    static rstl::reserved_vector<CInt32POINode, 16> g_Int32POINodes;
    static rstl::reserved_vector<CParticlePOINode, 20> g_ParticlePOINodes;
    static rstl::reserved_vector<CSoundPOINode, 20> g_SoundPOINodes;
    static rstl::reserved_vector<CInt32POINode, 16> g_TransientInt32POINodes;

    int m_drawInstCount;

public:
    CAnimData(CAssetId,
              const CCharacterInfo& character,
              int defaultAnim, int charIdx, bool loop,
              const TLockedToken<CCharLayoutInfo>& layout,
              const TToken<CSkinnedModel>& model,
              const std::experimental::optional<TToken<CMorphableSkinnedModel>>& iceModel,
              const std::weak_ptr<CAnimSysContext>& ctx,
              const std::shared_ptr<CAnimationManager>& animMgr,
              const std::shared_ptr<CTransitionManager>& transMgr,
              const TLockedToken<CCharacterFactory>& charFactory,
              int drawInstCount);

    void InitializeEffects(CStateManager&, TAreaId, const zeus::CVector3f&);
    CAssetId GetEventResourceIdForAnimResourceId(CAssetId) const;
    void AddAdditiveSegData(const CSegIdList& list, CSegStatementSet& stSet);
    static SAdvancementResults AdvanceAdditiveAnim(std::shared_ptr<CAnimTreeNode>& anim, const CCharAnimTime& time);
    SAdvancementDeltas AdvanceAdditiveAnims(float);
    SAdvancementDeltas UpdateAdditiveAnims(float);
    bool IsAdditiveAnimation(u32) const;
    bool IsAdditiveAnimationAdded(u32) const;
    const std::shared_ptr<CAnimTreeNode>& GetRootAnimationTree() const { return x1f8_animRoot; }
    const std::shared_ptr<CAnimTreeNode>& GetAdditiveAnimationTree(u32) const;
    bool IsAdditiveAnimationActive(u32) const;
    void DelAdditiveAnimation(u32);
    void AddAdditiveAnimation(u32, float, bool, bool);
    float GetAdditiveAnimationWeight(u32 idx) const;
    std::shared_ptr<CAnimationManager> GetAnimationManager();
    const CCharacterInfo& GetCharacterInfo() const { return xc_charInfo; }
    const CCharLayoutInfo& GetCharLayoutInfo() const { return *xcc_layoutData.GetObj(); }
    void SetPhase(float);
    void Touch(const CSkinnedModel& model, int shaderIdx) const;
    SAdvancementDeltas GetAdvancementDeltas(const CCharAnimTime& a, const CCharAnimTime& b) const;
    CCharAnimTime GetTimeOfUserEvent(EUserEventType, const CCharAnimTime& time) const;
    void MultiplyPlaybackRate(float);
    void SetPlaybackRate(float);
    void SetRandomPlaybackRate(CRandom16&);
    void CalcPlaybackAlignmentParms(const CAnimPlaybackParms& parms,
                                    const std::shared_ptr<CAnimTreeNode>& node);
    zeus::CTransform GetLocatorTransform(CSegId id, const CCharAnimTime* time) const;
    zeus::CTransform GetLocatorTransform(std::string_view name, const CCharAnimTime* time) const;
    bool IsAnimTimeRemaining(float, std::string_view name) const;
    float GetAnimTimeRemaining(std::string_view name) const;
    float GetAnimationDuration(int) const;
    bool GetIsLoop() const {return x220_25_loop;}
    void EnableLooping(bool val) {x220_25_loop = val; x220_24_animating = true;}
    void EnableAnimation(bool val) {x220_24_animating = val;}
    bool IsAnimating() const {return x220_24_animating;}
    void SetAnimDir(EAnimDir dir) { x104_animDir = dir; }
    std::shared_ptr<CAnimSysContext> GetAnimSysContext() const;
    std::shared_ptr<CAnimationManager> GetAnimationManager() const;
    void RecalcPoseBuilder(const CCharAnimTime*);
    void RenderAuxiliary(const zeus::CFrustum& frustum) const;
    void Render(CSkinnedModel& model, const CModelFlags& drawFlags,
                const std::experimental::optional<CVertexMorphEffect>& morphEffect,
                const float* morphMagnitudes);
    void SetupRender(CSkinnedModel& model,
                     const CModelFlags& drawFlags,
                     const std::experimental::optional<CVertexMorphEffect>& morphEffect,
                     const float* morphMagnitudes);
    static void DrawSkinnedModel(CSkinnedModel& model, const CModelFlags& flags);
    void PreRender();
    void BuildPose();
    static void PrimitiveSetToTokenVector(const std::set<CPrimitive>& primSet,
                                          std::vector<CToken>& tokensOut, bool preLock);
    void GetAnimationPrimitives(const CAnimPlaybackParms& parms, std::set<CPrimitive>& primsOut) const;
    void SetAnimation(const CAnimPlaybackParms& parms, bool);
    SAdvancementDeltas DoAdvance(float, bool& suspendParticles, CRandom16&, bool advTree);
    SAdvancementDeltas Advance(float, const zeus::CVector3f&, CStateManager& stateMgr, TAreaId aid, bool advTree);
    SAdvancementDeltas AdvanceIgnoreParticles(float, CRandom16&, bool advTree);
    void AdvanceAnim(CCharAnimTime& time, zeus::CVector3f&, zeus::CQuaternion&);
    void SetXRayModel(const TLockedToken<CModel>& model, const TLockedToken<CSkinRules>& skinRules);
    void SetInfraModel(const TLockedToken<CModel>& model, const TLockedToken<CSkinRules>& skinRules);
    const TCachedToken<CSkinnedModel>& GetModelData() const { return xd8_modelData; }

    static void PoseSkinnedModel(CSkinnedModel& model, const CPoseAsTransforms& pose,
                                 const CModelFlags& drawFlags,
                                 const std::experimental::optional<CVertexMorphEffect>& morphEffect,
                                 const float* morphMagnitudes);
    void AdvanceParticles(const zeus::CTransform& xf, float dt,
                          const zeus::CVector3f&, CStateManager& stateMgr);
    float GetAverageVelocity(int animIn) const;
    void ResetPOILists();
    CSegId GetLocatorSegId(std::string_view name) const;
    zeus::CAABox GetBoundingBox(const zeus::CTransform& xf) const;
    zeus::CAABox GetBoundingBox() const;
    void SubstituteModelData(const TCachedToken<CSkinnedModel>& model);
    static void FreeCache();
    static void InitializeCache();
    const CHierarchyPoseBuilder& GetPoseBuilder() const { return x2fc_poseBuilder; }
    const CParticleDatabase& GetParticleDB() const { return x120_particleDB; }
    CParticleDatabase& GetParticleDB() { return x120_particleDB; }
    void SetParticleCEXTValue(std::string_view name, int idx, float value);

    u32 GetPassedBoolPOICount() const { return x20c_passedBoolCount; }
    u32 GetPassedIntPOICount() const { return x210_passedIntCount; }
    u32 GetPassedParticlePOICount() const { return x214_passedParticleCount; }
    u32 GetPassedSoundPOICount() const { return x218_passedSoundCount; }

    s32 GetCharacterIndex() const { return x204_charIdx; }
};

}

#endif // __URDE_CANIMDATA_HPP__
