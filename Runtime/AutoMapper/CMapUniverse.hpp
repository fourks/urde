#ifndef __URDE_CMAPUNIVERSE_HPP__
#define __URDE_CMAPUNIVERSE_HPP__

#include "RetroTypes.hpp"
#include "zeus/CVector3f.hpp"
#include "zeus/CColor.hpp"
#include "zeus/CTransform.hpp"
#include "IFactory.hpp"
#include "CToken.hpp"
#include "CMapArea.hpp"

namespace urde
{
class CStateManager;
class CMapUniverse
{
public:
    class CMapUniverseDrawParms
    {
        float x0_alpha;
        int x4_wldIdx;
        ResId x8_wldRes;
        int xc_closestHex;
        float x10_flashPulse;
        const CStateManager& x14_mgr;
        const zeus::CTransform& x18_model;
        const zeus::CTransform& x1c_view;
    public:
        CMapUniverseDrawParms(float alpha, int wldIdx, ResId wldRes, int closestHex,
                              float flashPulse, const CStateManager& mgr,
                              const zeus::CTransform& model, const zeus::CTransform& view)
        : x0_alpha(alpha),
          x4_wldIdx(wldIdx),
          x8_wldRes(wldRes),
          xc_closestHex(closestHex),
          x10_flashPulse(flashPulse),
          x14_mgr(mgr),
          x18_model(model),
          x1c_view(view)
        {}
        int GetFocusWorldIndex() const { return x4_wldIdx; }
        const zeus::CTransform& GetCameraTransform() const { return x1c_view; }
        const zeus::CTransform& GetPaneProjectionTransform() const { return x18_model; }
        float GetAlpha() const { return x0_alpha; }
    };

    class CMapObjectSortInfo
    {
    public:
        CMapObjectSortInfo(float, int, int, int, const zeus::CColor&, const zeus::CColor&);
        zeus::CColor GetOutlineColor() const;
        zeus::CColor GetSurfaceColor() const;
        s32 GetObjectIndex() const;
        s32 GetAreaIndex() const;
        s32 GetWorldIndex() const;
        float GetZDistance() const;
    };

    class CMapWorldData
    {
        std::string x0_label;
        ResId x10_worldAssetId;
        zeus::CTransform x14_transform;
        std::vector<zeus::CTransform> x44_hexagonXfs;
        zeus::CColor x54_;
        zeus::CColor x58_ = zeus::CColor(1.0f, 0.0f, 1.0f);
        zeus::CColor x5c_ = zeus::CColor(1.0f, 0.0f, 1.0f);
        zeus::CColor x60_ = zeus::CColor(1.0f, 0.0f, 1.0f);
        zeus::CVector3f x64_centerPoint = zeus::CVector3f::skZero;
    public:
        CMapWorldData(CInputStream& in, u32 version);
        ResId GetWorldAssetId() const { return x10_worldAssetId; }
        const zeus::CVector3f& GetWorldCenterPoint() const { return x64_centerPoint; }
        const std::string& GetWorldLabel() const { return x0_label; }
        const zeus::CTransform& GetWorldTransform() const { return x14_transform; }
        const zeus::CTransform& GetMapAreaData(s32 idx) const { return x44_hexagonXfs[idx]; }
        u32 GetNumMapAreaDatas() const { return x44_hexagonXfs.size(); }
        zeus::CColor GetOutlineColorUnselected() const;
        zeus::CColor GetOutlineColorSelected() const;
        zeus::CColor GetSurfaceColorUnselected() const;
        zeus::CColor GetSurfaceColorSelected() const;
    };

private:
    ResId x0_hexagonId;
    TLockedToken<CMapArea> x4_hexagonToken;
    std::vector<CMapWorldData> x10_worldDatas;
    zeus::CVector3f x20_universeCenter = zeus::CVector3f::skZero;
    float x2c_universeRadius = 1600.f;
public:
    CMapUniverse(CInputStream&, u32);
    const CMapWorldData& GetMapWorldData(s32 idx) const { return x10_worldDatas[idx]; }
    const CMapWorldData& GetMapWorldDataByWorldId(ResId id) const
    {
        for (const CMapWorldData& data : x10_worldDatas)
            if (data.GetWorldAssetId() == id)
                return data;
        return x10_worldDatas.front();
    }
    u32 GetNumMapWorldDatas() const { return x10_worldDatas.size(); }
    float GetMapUniverseRadius() const { return x2c_universeRadius; }
    const zeus::CVector3f& GetMapUniverseCenterPoint() const { return x20_universeCenter; }
    void Draw(const CMapUniverseDrawParms&, const zeus::CVector3f&, float, float) const;
    std::vector<CMapWorldData>::const_iterator begin() const { return x10_worldDatas.cbegin(); }
    std::vector<CMapWorldData>::const_iterator end() const { return x10_worldDatas.cend(); }
};

CFactoryFnReturn FMapUniverseFactory(const SObjectTag& tag, CInputStream& in, const CVParamTransfer& vparms,
                                     CObjectReference*);

}
#endif
