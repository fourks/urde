#include "CGraphics.hpp"
#include <Math.hpp>

namespace pshag
{

CGraphics::CProjectionState CGraphics::g_Proj;
u32 CGraphics::g_NumLightsActive = 0;
ERglLight CGraphics::g_LightActive = ERglLight::None;
ERglLight CGraphics::g_LightsWereOn = ERglLight::None;
Zeus::CTransform CGraphics::g_GXModelView;
Zeus::CTransform CGraphics::g_GXModelMatrix;
Zeus::CTransform CGraphics::g_ViewMatrix;
Zeus::CVector3f CGraphics::g_ViewPoint;
Zeus::CTransform CGraphics::g_GXViewPointMatrix;
Zeus::CTransform CGraphics::g_CameraMatrix;
Zeus::CVector2i CGraphics::g_ViewportResolution;
Zeus::CVector2i CGraphics::g_ViewportResolutionHalf;
bool CGraphics::g_IsGXModelMatrixIdentity;

void CGraphics::DisableAllLights()
{
    g_NumLightsActive = 0;
    g_LightActive = ERglLight::None;
    // TODO: turn lights off for real
}

void CGraphics::EnableLight(ERglLight light)
{
    if ((light & g_LightActive) == ERglLight::None)
    {
        g_LightActive |= light;
        ++g_NumLightsActive;
        // TODO: turn light on for real
    }
    g_LightsWereOn = g_LightActive;
}

void CGraphics::SetLightState(ERglLight lightState)
{
    // TODO: set state for real
    g_LightActive = lightState;
    g_NumLightsActive = Zeus::Math::PopCount(lightState);
}

void CGraphics::SetDepthWriteMode(bool test, ERglEnum comp, bool write)
{

}

void CGraphics::SetBlendMode(ERglBlendMode, ERglBlendFactor, ERglBlendFactor, ERglLogicOp)
{
}

void CGraphics::SetCullMode(ERglCullMode)
{
}

void CGraphics::SetAlphaCompare(ERglAlphaFunc comp0, u8 ref0, ERglAlphaOp op, ERglAlphaFunc comp1, u8 ref1)
{
}

void CGraphics::SetViewPointMatrix(const Zeus::CTransform& xf)
{
    g_ViewMatrix = xf;
    g_ViewPoint = xf.m_origin;
    g_GXViewPointMatrix.m_basis = g_ViewMatrix.m_basis.transposed();
    g_GXViewPointMatrix.m_origin = -g_ViewPoint;
    SetViewMatrix();
}

void CGraphics::SetViewMatrix()
{
    g_CameraMatrix = g_GXViewPointMatrix * Zeus::CTransform::Translate(-g_ViewPoint);
    if (g_IsGXModelMatrixIdentity)
        g_GXModelView = g_CameraMatrix;
    else
        g_GXModelView = g_CameraMatrix * g_GXModelMatrix;
    /* Load position matrix */
    /* Inverse-transpose */
    /* Load normal matrix */
}

void CGraphics::SetModelMatrix(const Zeus::CTransform& xf)
{
    g_IsGXModelMatrixIdentity = false;
    g_GXModelMatrix = xf;
    SetViewMatrix();
}

Zeus::CMatrix4f CGraphics::GetPerspectiveProjectionMatrix()
{
    float rml = g_Proj.x8_right - g_Proj.x4_left;
    float rpl = g_Proj.x8_right + g_Proj.x4_left;
    float tmb = g_Proj.xc_top - g_Proj.x10_bottom;
    float tpb = g_Proj.xc_top + g_Proj.x10_bottom;
    float fmn = g_Proj.x18_far - g_Proj.x14_near;
    float fpn = g_Proj.x18_far + g_Proj.x14_near;
    return Zeus::CMatrix4f(2.f * g_Proj.x14_near / rml, 0.f, rpl / rml, 0.f,
                           0.f, 2.f * g_Proj.x14_near / tmb, tpb / tmb, 0.f,
                           0.f, 0.f, -fpn / fmn, -2.f * g_Proj.x18_far * g_Proj.x14_near / fmn,
                           0.f, 0.f, -1.f, 0.f);
}

const CGraphics::CProjectionState& CGraphics::GetProjectionState()
{
    return g_Proj;
}

void CGraphics::SetProjectionState(const CGraphics::CProjectionState& proj)
{
    g_Proj = proj;
    FlushProjection();
}

void CGraphics::SetPerspective(float fovy, float aspect, float near, float far)
{
    float tfov = tanf(fovy * 0.5f * M_PI / 180.f);
    g_Proj.x0_persp = true;
    g_Proj.x14_near = near;
    g_Proj.x18_far = far;
    g_Proj.xc_top = near * tfov;
    g_Proj.x10_bottom = -g_Proj.xc_top;
    g_Proj.x8_right = aspect * near * tfov;
    g_Proj.x4_left = -g_Proj.x8_right;
}

void CGraphics::FlushProjection()
{
    if (g_Proj.x0_persp)
    {
        // Convert and load persp
    }
    else
    {
        // Convert and load ortho
    }
}

Zeus::CVector2i CGraphics::ProjectPoint(const Zeus::CVector3f& point)
{
    Zeus::CVector3f projPt = GetPerspectiveProjectionMatrix().multiplyOneOverW(point);
    return {int(projPt.x * g_ViewportResolutionHalf.x) + g_ViewportResolutionHalf.x,
            int(projPt.y * g_ViewportResolutionHalf.y) + g_ViewportResolutionHalf.y};
}

SClipScreenRect CGraphics::ClipScreenRectFromMS(const Zeus::CVector3f& p1,
                                                const Zeus::CVector3f& p2)
{
    Zeus::CVector3f xfExt = (g_GXModelMatrix * p2) - g_ViewMatrix.m_origin;
    xfExt = g_ViewMatrix.transposeRotate(xfExt);
    Zeus::CVector3f xfPos = (g_GXModelMatrix * p1) - g_ViewMatrix.m_origin;
    xfPos = g_ViewMatrix.transposeRotate(xfPos);
    return ClipScreenRectFromVS(xfPos, xfExt);
}

SClipScreenRect CGraphics::ClipScreenRectFromVS(const Zeus::CVector3f& p1,
                                                const Zeus::CVector3f& p2)
{
    if (p1.x == 0.f && p1.y == 0.f && p1.z == 0.f)
        return {};
    if (p2.x == 0.f && p2.y == 0.f && p2.z == 0.f)
        return {};

    if (p1.y < GetProjectionState().x14_near || p2.y < GetProjectionState().x14_near)
        return {};
    if (p1.y > GetProjectionState().x18_far || p2.y > GetProjectionState().x18_far)
        return {};

    Zeus::CVector2i sp1 = ProjectPoint(p1);
    Zeus::CVector2i sp2 = ProjectPoint(p2);
    int minX = std::min(sp2.x, sp1.x);
    int minX2 = minX & 0xfffffffe;
    int minY = std::min(sp2.y, sp1.y);
    int minY2 = minY & 0xfffffffe;


    if (minX2 >= g_ViewportResolution.x)
        return {};

    int maxX = abs(sp1.x - sp2.x) + minX;
    int maxX2 = (maxX + 2) & 0xfffffffe;
    if (maxX2 <= 0 /* ViewportX origin */)
        return {};

    int finalMinX = std::max(minX, 0 /* ViewportX origin */);
    int finalMaxX = std::min(maxX, g_ViewportResolution.x);


    if (minY2 >= g_ViewportResolution.y)
        return {};

    int maxY = abs(sp1.y - sp2.y) + minY;
    int maxY2 = (maxY + 2) & 0xfffffffe;
    if (maxY2 <= 0 /* ViewportY origin */)
        return {};

    int finalMinY = std::max(minY, 0 /* ViewportY origin */);
    int finalMaxY = std::min(maxY, g_ViewportResolution.y);

    int width = maxX2 - minX2;
    int height = maxY2 - minY2;
    return {true, minX2, minY2, width, height, width,
            (finalMinX - minX2) / float(width), (finalMaxX - minX2) / float(width),
            (finalMinY - minY2) / float(height), (finalMaxY - minY2) / float(height)};

}

boo::IGraphicsDataFactory* CGraphics::g_BooFactory = nullptr;
boo::IGraphicsCommandQueue* CGraphics::g_BooMainCommandQueue = nullptr;
boo::ITextureR* CGraphics::g_SpareTexture = nullptr;

}