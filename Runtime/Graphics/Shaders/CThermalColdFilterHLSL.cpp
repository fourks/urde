#include "CThermalColdFilter.hpp"

namespace urde
{

static const char* VS =
"struct VertData\n"
"{\n"
"    float4 posIn : POSITION;\n"
"    float4 uvIn : UV;\n"
"};\n"
"\n"
"cbuffer ThermalColdUniform : register(b0)\n"
"{\n"
"    float4x4 shiftMtx;\n"
"    float4x4 indMtx;\n"
"    float4 shiftScale;\n"
"    float4 colorReg0;\n"
"    float4 colorReg1;\n"
"    float4 colorReg2;\n"
"};\n"
"\n"
"struct VertToFrag\n"
"{\n"
"    float4 position : SV_Position;\n"
"    float3x3 indMtx : INDMTX;\n"
"    float4 colorReg0 : COLORREG0;\n"
"    float4 colorReg1 : COLORREG1;\n"
"    float4 colorReg2 : COLORREG2;\n"
"    float2 sceneUv : SCENEUV;\n"
"    float2 shiftUv : SHIFTUV;\n"
"    float2 shiftScale : SHIFTSCALE;\n"
"};\n"
"\n"
"VertToFrag main(in VertData v)\n"
"{\n"
"    VertToFrag vtf;\n"
"    vtf.indMtx = float3x3(indMtx[0].xyz, indMtx[1].xyz, indMtx[2].xyz);\n"
"    vtf.colorReg0 = colorReg0;\n"
"    vtf.colorReg1 = colorReg1;\n"
"    vtf.colorReg2 = colorReg2;\n"
"    vtf.sceneUv = v.uvIn.xy;\n"
"    vtf.sceneUv.y = 1.0 - vtf.sceneUv.y;\n"
"    vtf.shiftUv = (mul(float3x3(shiftMtx[0].xyz, shiftMtx[1].xyz, shiftMtx[2].xyz), v.uvIn.xyz)).xy;\n"
"    vtf.shiftScale = shiftScale.xy;\n"
"    vtf.position = float4(v.posIn.xyz, 1.0);\n"
"    return vtf;\n"
"}\n";

static const char* FS =
"Texture2D sceneTex : register(t0);\n"
"Texture2D shiftTex : register(t1);\n"
"SamplerState samp : register(s2);\n"
"struct VertToFrag\n"
"{\n"
"    float4 position : SV_Position;\n"
"    float3x3 indMtx : INDMTX;\n"
"    float4 colorReg0 : COLORREG0;\n"
"    float4 colorReg1 : COLORREG1;\n"
"    float4 colorReg2 : COLORREG2;\n"
"    float2 sceneUv : SCENEUV;\n"
"    float2 shiftUv : SHIFTUV;\n"
"    float2 shiftScale : SHIFTSCALE;\n"
"};\n"
"\n"
"static const float4 kRGBToYPrime = {0.299, 0.587, 0.114, 0.0};\n"
"float4 main(in VertToFrag vtf) : SV_Target0\n"
"{\n"
"    float2 shiftCoordTexel = shiftTex.Sample(samp, vtf.shiftUv).xy;\n"
"    float2 shiftCoord = vtf.sceneUv + shiftCoordTexel * vtf.shiftScale;\n"
"    float shiftScene0 = dot(sceneTex.Sample(samp, shiftCoord), kRGBToYPrime);\n"
"    float shiftScene1 = dot(sceneTex.Sample(samp, shiftCoord + float2(vtf.shiftScale.x / 8.0, 0.0)), kRGBToYPrime);\n"
"    float2 indCoord = (mul(vtf.indMtx, float3(shiftScene0 - 0.5, shiftScene1 - 0.5, 1.0))).xy;\n"
"    float indScene = dot(sceneTex.Sample(samp, vtf.sceneUv + indCoord), kRGBToYPrime);\n"
"    return vtf.colorReg0 * indScene + vtf.colorReg1 * shiftScene0 + vtf.colorReg2;\n"
"}\n";

URDE_DECL_SPECIALIZE_SHADER(CThermalColdFilter)

static boo::ObjToken<boo::IVertexFormat> s_VtxFmt;
static boo::ObjToken<boo::IShaderPipeline> s_Pipeline;

struct CThermalColdFilterD3DDataBindingFactory : TShader<CThermalColdFilter>::IDataBindingFactory
{
    boo::ObjToken<boo::IShaderDataBinding>
    BuildShaderDataBinding(boo::IGraphicsDataFactory::Context& ctx,
                           CThermalColdFilter& filter)
    {
        boo::ID3DDataFactory::Context& cctx = static_cast<boo::ID3DDataFactory::Context&>(ctx);

        boo::ObjToken<boo::IGraphicsBuffer> bufs[] = {filter.m_uniBuf.get()};
        boo::ObjToken<boo::ITexture> texs[] = {CGraphics::g_SpareTexture.get(), filter.m_shiftTex.get()};
        return cctx.newShaderDataBinding(s_Pipeline, s_VtxFmt,
                                         filter.m_vbo.get(), nullptr, nullptr, 1, bufs,
                                         nullptr, nullptr, nullptr, 2, texs, nullptr, nullptr);
    }
};

TShader<CThermalColdFilter>::IDataBindingFactory* CThermalColdFilter::Initialize(boo::ID3DDataFactory::Context& ctx)
{
    const boo::VertexElementDescriptor VtxVmt[] =
    {
        {nullptr, nullptr, boo::VertexSemantic::Position4},
        {nullptr, nullptr, boo::VertexSemantic::UV4}
    };
    s_VtxFmt = ctx.newVertexFormat(2, VtxVmt);
    s_Pipeline = ctx.newShaderPipeline(VS, FS, nullptr, nullptr, nullptr,
                                       s_VtxFmt, boo::BlendFactor::One,
                                       boo::BlendFactor::Zero, boo::Primitive::TriStrips,
                                       boo::ZTest::None, false, true, false, boo::CullMode::None);
    return new CThermalColdFilterD3DDataBindingFactory;
}

template <>
void CThermalColdFilter::Shutdown<boo::ID3DDataFactory>()
{
    s_VtxFmt.reset();
    s_Pipeline.reset();
}
}
