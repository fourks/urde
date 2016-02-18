#include "CElementGenShaders.hpp"
#include "CElementGen.hpp"
#include "CGenDescription.hpp"

namespace pshag
{

static const char* VS_METAL_TEX =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"struct VertData\n"
"{\n"
"    float4 posIn[4];\n"
"    float4 colorIn;\n"
"    float4 uvsIn[4];\n"
"};\n"
"\n"
"struct ParticleUniform\n"
"{\n"
"    float4x4 mvp;\n"
"    float4 moduColor;\n"
"};\n"
"\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float2 uv;\n"
"};\n"
"\n"
"vertex VertToFrag vmain(constant VertData* va [[ buffer(1) ]],\n"
"                        uint vertId [[ vertex_id ]], uint instId [[ instance_id ]],\n"
"                        constant ParticleUniform& particle [[ buffer(2) ]])\n"
"{\n"
"    VertToFrag vtf;\n"
"    constant VertData& v = va[instId];\n"
"    vtf.color = v.colorIn * particle.moduColor;\n"
"    vtf.uv = v.uvsIn[vertId].xy;\n"
"    vtf.position = particle.mvp * v.posIn[vertId];\n"
"    return vtf;\n"
"}\n";

static const char* FS_METAL_TEX =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"constexpr sampler samp(address::repeat, filter::linear, mip_filter::linear);\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float2 uv;\n"
"};\n"
"\n"
"fragment float4 fmain(VertToFrag vtf [[ stage_in ]],\n"
"                      texture2d<float> tex0 [[ texture(0) ]])\n"
"{\n"
"    return vtf.color * tex0.sample(samp, vtf.uv);\n"
"}\n";

static const char* FS_METAL_TEX_REDTOALPHA =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"constexpr sampler samp(address::repeat, filter::linear, mip_filter::linear);\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float2 uv;\n"
"};\n"
"\n"
"fragment float4 fmain(VertToFrag vtf [[ stage_in ]],\n"
"                      texture2d<float> tex0 [[ texture(0) ]])\n"
"{\n"
"    float4 colr = vtf.color * tex0.sample(samp, vtf.uv);\n"
"    return float4(colr.rgb, colr.r);"
"}\n";

static const char* VS_METAL_INDTEX =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"struct VertData\n"
"{\n"
"    float4 posIn[4];\n"
"    float4 colorIn;\n"
"    float4 uvsInTexrTind[4];\n"
"    float4 uvsInScene[4];\n"
"};\n"
"\n"
"struct ParticleUniform\n"
"{\n"
"    float4x4 mvp;\n"
"    float4 moduColor;\n"
"};\n"
"\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float2 uvTexr;\n"
"    float2 uvScene;\n"
"    float2 uvTind;\n"
"};\n"
"\n"
"vertex VertToFrag vmain(constant VertData* va [[ buffer(1) ]],\n"
"                        uint vertId [[ vertex_id ]], uint instId [[ instance_id ]],\n"
"                        constant ParticleUniform& particle [[ buffer(2) ]])\n"
"{\n"
"    VertToFrag vtf;\n"
"    constant VertData& v = va[instId];\n"
"    vtf.color = v.colorIn * particle.moduColor;\n"
"    vtf.uvTexr = v.uvsInTexrTind[vertId].xy;\n"
"    vtf.uvScene = v.uvsInScene[vertId].xy;\n"
"    vtf.uvTind = v.uvsInTexrTind[vertId].zw;\n"
"    vtf.position = particle.mvp * v.posIn[vertId];\n"
"    return vtf;\n"
"}\n";

static const char* FS_METAL_INDTEX =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"constexpr sampler samp(address::repeat, filter::linear, mip_filter::linear);\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float2 uvTexr;\n"
"    float2 uvScene;\n"
"    float2 uvTind;\n"
"};\n"
"\n"
"fragment float4 fmain(VertToFrag vtf [[ stage_in ]],\n"
"                      texture2d<float> tex0 [[ texture(0) ]],\n"
"                      texture2d<float> tex1 [[ texture(1) ]],\n"
"                      texture2d<float> tex2 [[ texture(2) ]])\n"
"{\n"
"    float2 tindTexel = tex2.sample(samp, vtf.uvTind).ba;\n"
"    float4 sceneTexel = tex1.sample(samp, vtf.uvScene + tindTexel);\n"
"    float4 texrTexel = tex0.sample(samp, vtf.uvTexr);\n"
"    float4 colr = vtf.color * sceneTexel + texrTexel;\n"
"    return float4(colr.rgb, vtf.color.a * texrTexel.a);"
"}\n";

static const char* FS_METAL_CINDTEX =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"constexpr sampler samp(address::repeat, filter::linear, mip_filter::linear);\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float2 uvTexr;\n"
"    float2 uvScene;\n"
"    float2 uvTind;\n"
"};\n"
"\n"
"fragment float4 fmain(VertToFrag vtf [[ stage_in ]],\n"
"                      texture2d<float> tex0 [[ texture(0) ]],\n"
"                      texture2d<float> tex1 [[ texture(1) ]],\n"
"                      texture2d<float> tex2 [[ texture(2) ]])\n"
"{\n"
"    float2 tindTexel = tex2.sample(samp, vtf.uvTind).ba;\n"
"    float4 sceneTexel = tex1.sample(samp, vtf.uvScene + tindTexel);\n"
"    return vtf.color * sceneTexel * tex0.sample(samp, vtf.uvTexr);\n"
"}\n";

static const char* VS_METAL_NOTEX =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"struct VertData\n"
"{\n"
"    float4 posIn[4];\n"
"    float4 colorIn;\n"
"};\n"
"\n"
"struct ParticleUniform\n"
"{\n"
"    float4x4 mvp;\n"
"    float4 moduColor;\n"
"};\n"
"\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"};\n"
"\n"
"vertex VertToFrag vmain(constant VertData* va [[ buffer(1) ]],\n"
"                        uint vertId [[ vertex_id ]], uint instId [[ instance_id ]],\n"
"                        constant ParticleUniform& particle [[ buffer(2) ]])\n"
"{\n"
"    VertToFrag vtf;\n"
"    constant VertData& v = va[instId];\n"
"    vtf.color = v.colorIn * particle.moduColor;\n"
"    vtf.position = particle.mvp * v.posIn[vertId];\n"
"    return vtf;\n"
"}\n";

static const char* FS_METAL_NOTEX =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"};\n"
"\n"
"fragment float4 fmain(VertToFrag vtf [[ stage_in ]])\n"
"{\n"
"    return vtf.color;\n"
"}\n";

struct MetalElementDataBindingFactory : CElementGenShaders::IDataBindingFactory
{
    void BuildShaderDataBinding(CElementGen& gen,
                                boo::IShaderPipeline* regPipeline,
                                boo::IShaderPipeline* redToAlphaPipeline)
    {
        CGenDescription* desc = gen.GetDesc();

        CUVElement* texr = desc->x54_TEXR.get();
        CUVElement* tind = desc->x58_TIND.get();
        int texCount = 0;
        boo::ITexture* textures[3];

        if (texr)
        {
            textures[0] = texr->GetValueTexture(0).GetObj()->GetBooTexture();
            texCount = 1;
            if (tind)
            {
                textures[1] = CGraphics::g_SpareTexture;
                textures[2] = tind->GetValueTexture(0).GetObj()->GetBooTexture();
                texCount = 3;
            }
        }

        boo::IGraphicsBuffer* uniforms[] = {gen.m_uniformBuf};

        if (regPipeline)
            gen.m_normalDataBind = CGraphics::g_BooFactory->newShaderDataBinding(regPipeline, nullptr, nullptr,
                                                                                 gen.m_instBuf, nullptr, 1, uniforms,
                                                                                 texCount, textures);
        if (redToAlphaPipeline)
            gen.m_redToAlphaDataBind = CGraphics::g_BooFactory->newShaderDataBinding(redToAlphaPipeline, nullptr, nullptr,
                                                                                     gen.m_instBuf, nullptr, 1, uniforms,
                                                                                     texCount, textures);
    }
};

CElementGenShaders::IDataBindingFactory* CElementGenShaders::Initialize(boo::MetalDataFactory& factory)
{
    static const boo::VertexElementDescriptor TexFmtTex[] =
    {
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 0},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 1},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 2},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 3},
        {nullptr, nullptr, boo::VertexSemantic::Color | boo::VertexSemantic::Instanced},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 0},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 1},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 2},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 3}
    };
    m_vtxFormatTex = factory.newVertexFormat(9, TexFmtTex);

    static const boo::VertexElementDescriptor TexFmtIndTex[] =
    {
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 0},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 1},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 2},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 3},
        {nullptr, nullptr, boo::VertexSemantic::Color | boo::VertexSemantic::Instanced},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 0},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 1},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 2},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 3},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 4},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 5},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 6},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 7}
    };
    m_vtxFormatIndTex = CGraphics::g_BooFactory->newVertexFormat(13, TexFmtIndTex);

    static const boo::VertexElementDescriptor TexFmtNoTex[] =
    {
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 0},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 1},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 2},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 3},
        {nullptr, nullptr, boo::VertexSemantic::Color | boo::VertexSemantic::Instanced}
    };
    m_vtxFormatNoTex = CGraphics::g_BooFactory->newVertexFormat(5, TexFmtNoTex);

    m_texZTestZWrite = factory.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, m_vtxFormatTex,
                                                 CGraphics::g_ViewportSamples,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                 true, true, false);
    m_texNoZTestZWrite = factory.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, m_vtxFormatTex,
                                                   CGraphics::g_ViewportSamples,
                                                   boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                   false, true, false);
    m_texZTestNoZWrite = factory.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, m_vtxFormatTex,
                                                   CGraphics::g_ViewportSamples,
                                                   boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                   true, false, false);
    m_texNoZTestNoZWrite = factory.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, m_vtxFormatTex,
                                                     CGraphics::g_ViewportSamples,
                                                     boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                     false, false, false);

    m_texAdditiveZTest = factory.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, m_vtxFormatTex,
                                                   CGraphics::g_ViewportSamples,
                                                   boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                                   true, false, false);
    m_texAdditiveNoZTest = factory.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, m_vtxFormatTex,
                                                     CGraphics::g_ViewportSamples,
                                                     boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                                     false, false, false);

    m_texRedToAlphaZTest = factory.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX_REDTOALPHA, m_vtxFormatTex,
                                                     CGraphics::g_ViewportSamples,
                                                     boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                     true, false, false);
    m_texRedToAlphaNoZTest = factory.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX_REDTOALPHA, m_vtxFormatTex,
                                                       CGraphics::g_ViewportSamples,
                                                       boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                       false, false, false);

    m_indTexZWrite = factory.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_INDTEX, m_vtxFormatIndTex,
                                               CGraphics::g_ViewportSamples,
                                               boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                               true, true, false);
    m_indTexNoZWrite = factory.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_INDTEX, m_vtxFormatIndTex,
                                                 CGraphics::g_ViewportSamples,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                 true, false, false);
    m_indTexAdditive = factory.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_INDTEX, m_vtxFormatIndTex,
                                                 CGraphics::g_ViewportSamples,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                                 true, true, false);

    m_cindTexZWrite = factory.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_CINDTEX, m_vtxFormatIndTex,
                                                CGraphics::g_ViewportSamples,
                                                boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                true, true, false);
    m_cindTexNoZWrite = factory.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_CINDTEX, m_vtxFormatIndTex,
                                                  CGraphics::g_ViewportSamples,
                                                  boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                  true, false, false);
    m_cindTexAdditive = factory.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_CINDTEX, m_vtxFormatIndTex,
                                                  CGraphics::g_ViewportSamples,
                                                  boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                                  true, true, false);

    m_noTexZTestZWrite = factory.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, m_vtxFormatNoTex,
                                                   CGraphics::g_ViewportSamples,
                                                   boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                   true, true, false);
    m_noTexNoZTestZWrite = factory.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, m_vtxFormatNoTex,
                                                     CGraphics::g_ViewportSamples,
                                                     boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                     false, true, false);
    m_noTexZTestNoZWrite = factory.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, m_vtxFormatNoTex,
                                                     CGraphics::g_ViewportSamples,
                                                     boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                     true, false, false);
    m_noTexNoZTestNoZWrite = factory.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, m_vtxFormatNoTex,
                                                       CGraphics::g_ViewportSamples,
                                                       boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                       false, false, false);

    m_noTexAdditiveZTest = factory.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, m_vtxFormatNoTex,
                                                     CGraphics::g_ViewportSamples,
                                                     boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                                     true, false, false);
    m_noTexAdditiveNoZTest = factory.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, m_vtxFormatNoTex,
                                                       CGraphics::g_ViewportSamples,
                                                       boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                                       false, false, false);

    return new struct MetalElementDataBindingFactory;
}

}
