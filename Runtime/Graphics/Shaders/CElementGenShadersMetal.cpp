#include "CElementGenShaders.hpp"
#include "Particle/CElementGen.hpp"
#include "Particle/CGenDescription.hpp"

namespace urde
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
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float2 uv;\n"
"};\n"
"\n"
"fragment float4 fmain(VertToFrag vtf [[ stage_in ]],\n"
"                      sampler samp [[ sampler(0) ]],\n"
"                      texture2d<float> tex0 [[ texture(0) ]])\n"
"{\n"
"    return vtf.color * tex0.sample(samp, vtf.uv);\n"
"}\n";

static const char* FS_METAL_TEX_REDTOALPHA =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float2 uv;\n"
"};\n"
"\n"
"fragment float4 fmain(VertToFrag vtf [[ stage_in ]],\n"
"                      sampler samp [[ sampler(0) ]],\n"
"                      texture2d<float> tex0 [[ texture(0) ]])\n"
"{\n"
"    return float4(vtf.color.rgb, tex0.sample(samp, vtf.uv).r);\n"
"}\n";

static const char* VS_METAL_INDTEX =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"struct VertData\n"
"{\n"
"    float4 posIn[4];\n"
"    float4 colorIn;\n"
"    float4 uvsInTexrTind[4];\n"
"    float4 uvsInScene;\n"
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
"    float4 uvScene;\n"
"    float2 uvTexr;\n"
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
"    vtf.uvScene = v.uvsInScene;\n"
"    vtf.uvScene.y = 1.0 - vtf.uvScene.y;\n"
"    vtf.uvScene.w = 1.0 - vtf.uvScene.w;\n"
"    vtf.uvTexr = v.uvsInTexrTind[vertId].xy;\n"
"    vtf.uvTind = v.uvsInTexrTind[vertId].zw;\n"
"    vtf.position = particle.mvp * v.posIn[vertId];\n"
"    return vtf;\n"
"}\n";

static const char* FS_METAL_INDTEX =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float4 uvScene;\n"
"    float2 uvTexr;\n"
"    float2 uvTind;\n"
"};\n"
"\n"
"fragment float4 fmain(VertToFrag vtf [[ stage_in ]],\n"
"                      sampler samp [[ sampler(0) ]],"
"                      texture2d<float> tex0 [[ texture(0) ]],\n"
"                      texture2d<float> tex1 [[ texture(1) ]],\n"
"                      texture2d<float> tex2 [[ texture(2) ]])\n"
"{\n"
"    float2 tindTexel = tex2.sample(samp, vtf.uvTind).ba;\n"
"    float4 sceneTexel = tex1.sample(samp, mix(vtf.uvScene.xy, vtf.uvScene.zw, tindTexel));\n"
"    float4 texrTexel = tex0.sample(samp, vtf.uvTexr);\n"
"    float4 colr = vtf.color * float4(sceneTexel.rgb, 1.0) + texrTexel;\n"
"    return float4(colr.rgb, vtf.color.a * texrTexel.a);"
"}\n";

static const char* FS_METAL_CINDTEX =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float4 uvScene;\n"
"    float2 uvTexr;\n"
"    float2 uvTind;\n"
"};\n"
"\n"
"fragment float4 fmain(VertToFrag vtf [[ stage_in ]],\n"
"                      sampler samp [[ sampler(0) ]],\n"
"                      texture2d<float> tex0 [[ texture(0) ]],\n"
"                      texture2d<float> tex1 [[ texture(1) ]],\n"
"                      texture2d<float> tex2 [[ texture(2) ]])\n"
"{\n"
"    float2 tindTexel = tex2.sample(samp, vtf.uvTind).ba;\n"
"    float4 sceneTexel = tex1.sample(samp, mix(vtf.uvScene.xy, vtf.uvScene.zw, tindTexel));\n"
"    return vtf.color * float4(sceneTexel.rgb, 1.0) * tex0.sample(samp, vtf.uvTexr);\n"
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

struct MetalElementDataBindingFactory : TShader<CElementGenShaders>::IDataBindingFactory
{
    boo::ObjToken<boo::IShaderDataBinding> BuildShaderDataBinding(boo::IGraphicsDataFactory::Context& ctx,
                                                                  CElementGenShaders& shader)
    {
        CElementGen& gen = shader.m_gen;
        CGenDescription* desc = gen.GetDesc();

        CUVElement* texr = desc->x54_x40_TEXR.get();
        CUVElement* tind = desc->x58_x44_TIND.get();
        int texCount = 0;
        boo::ObjToken<boo::ITexture> textures[3];

        if (texr)
        {
            textures[0] = texr->GetValueTexture(0).GetObj()->GetBooTexture();
            texCount = 1;
            if (tind)
            {
                textures[1] = CGraphics::g_SpareTexture.get();
                textures[2] = tind->GetValueTexture(0).GetObj()->GetBooTexture();
                texCount = 3;
            }
        }

        if (gen.m_instBuf)
        {
            boo::ObjToken<boo::IGraphicsBuffer> uniforms[] = {gen.m_uniformBuf.get()};

            if (shader.m_regPipeline)
                gen.m_normalDataBind = ctx.newShaderDataBinding(shader.m_regPipeline, nullptr, nullptr,
                                                                gen.m_instBuf.get(), nullptr, 1, uniforms,
                                                                nullptr, texCount, textures, nullptr, nullptr);
            if (shader.m_regPipelineSub)
                gen.m_normalSubDataBind = ctx.newShaderDataBinding(shader.m_regPipelineSub, nullptr, nullptr,
                                                                   gen.m_instBuf.get(), nullptr, 1, uniforms,
                                                                   nullptr, texCount, textures, nullptr, nullptr);
            if (shader.m_redToAlphaPipeline)
                gen.m_redToAlphaDataBind = ctx.newShaderDataBinding(shader.m_redToAlphaPipeline, nullptr, nullptr,
                                                                    gen.m_instBuf.get(), nullptr, 1, uniforms,
                                                                    nullptr, texCount, textures, nullptr, nullptr);
            if (shader.m_redToAlphaPipelineSub)
                gen.m_redToAlphaSubDataBind = ctx.newShaderDataBinding(shader.m_redToAlphaPipelineSub, nullptr, nullptr,
                                                                       gen.m_instBuf.get(), nullptr, 1, uniforms,
                                                                       nullptr, texCount, textures, nullptr, nullptr);
        }

        if (gen.m_instBufPmus)
        {
            boo::ObjToken<boo::IGraphicsBuffer> uniforms[] = {gen.m_uniformBufPmus.get()};
            texCount = std::min(texCount, 1);

            if (shader.m_regPipelinePmus)
                gen.m_normalDataBindPmus = ctx.newShaderDataBinding(shader.m_regPipelinePmus, nullptr, nullptr,
                                                                    gen.m_instBufPmus.get(), nullptr, 1, uniforms,
                                                                    nullptr, texCount, textures, nullptr, nullptr);
            if (shader.m_redToAlphaPipelinePmus)
                gen.m_redToAlphaDataBindPmus = ctx.newShaderDataBinding(shader.m_redToAlphaPipelinePmus, nullptr, nullptr,
                                                                        gen.m_instBufPmus.get(), nullptr, 1, uniforms,
                                                                        nullptr, texCount, textures, nullptr, nullptr);
        }

        return {};
    }
};

TShader<CElementGenShaders>::IDataBindingFactory* CElementGenShaders::Initialize(boo::MetalDataFactory::Context& ctx)
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
    m_vtxFormatTex = ctx.newVertexFormat(9, TexFmtTex);

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
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 4}
    };
    m_vtxFormatIndTex = ctx.newVertexFormat(10, TexFmtIndTex);

    static const boo::VertexElementDescriptor TexFmtNoTex[] =
    {
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 0},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 1},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 2},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 3},
        {nullptr, nullptr, boo::VertexSemantic::Color | boo::VertexSemantic::Instanced}
    };
    m_vtxFormatNoTex = ctx.newVertexFormat(5, TexFmtNoTex);

    m_texZTestZWrite = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, nullptr, nullptr, m_vtxFormatTex,
                                             boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                             boo::Primitive::TriStrips, boo::ZTest::LEqual, true, true, false, boo::CullMode::None);
    m_texNoZTestZWrite = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, nullptr, nullptr, m_vtxFormatTex,
                                               boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                               boo::Primitive::TriStrips, boo::ZTest::None, true, true, false, boo::CullMode::None);
    m_texZTestNoZWrite = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, nullptr, nullptr, m_vtxFormatTex,
                                               boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                               boo::Primitive::TriStrips, boo::ZTest::LEqual, false, true, false, boo::CullMode::None);
    m_texNoZTestNoZWrite = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, nullptr, nullptr, m_vtxFormatTex,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                 boo::Primitive::TriStrips, boo::ZTest::None, false, true, false, boo::CullMode::None);

    m_texAdditiveZTest = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, nullptr, nullptr, m_vtxFormatTex,
                                               boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                               boo::Primitive::TriStrips, boo::ZTest::LEqual, false, true, false, boo::CullMode::None);
    m_texAdditiveNoZTest = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, nullptr, nullptr, m_vtxFormatTex,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                                 boo::Primitive::TriStrips, boo::ZTest::None, false, true, false, boo::CullMode::None);

    m_texRedToAlphaZTest = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX_REDTOALPHA, nullptr, nullptr, m_vtxFormatTex,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                 boo::Primitive::TriStrips, boo::ZTest::LEqual, false, true, true, boo::CullMode::None);
    m_texRedToAlphaNoZTest = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX_REDTOALPHA, nullptr, nullptr, m_vtxFormatTex,
                                                   boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                   boo::Primitive::TriStrips, boo::ZTest::None, false, true, true, boo::CullMode::None);

    m_texZTestNoZWriteSub = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, nullptr, nullptr, m_vtxFormatTex,
                                                  boo::BlendFactor::Subtract, boo::BlendFactor::Subtract,
                                                  boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                                  true, false, boo::CullMode::None);
    m_texNoZTestNoZWriteSub = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX, nullptr, nullptr, m_vtxFormatTex,
                                                    boo::BlendFactor::Subtract, boo::BlendFactor::Subtract,
                                                    boo::Primitive::TriStrips, boo::ZTest::None, false,
                                                    true, false, boo::CullMode::None);

    m_texRedToAlphaZTestSub = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX_REDTOALPHA, nullptr, nullptr, m_vtxFormatTex,
                                                    boo::BlendFactor::Subtract, boo::BlendFactor::Subtract,
                                                    boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                                    true, true, boo::CullMode::None);
    m_texRedToAlphaNoZTestSub = ctx.newShaderPipeline(VS_METAL_TEX, FS_METAL_TEX_REDTOALPHA, nullptr, nullptr, m_vtxFormatTex,
                                                      boo::BlendFactor::Subtract, boo::BlendFactor::Subtract,
                                                      boo::Primitive::TriStrips, boo::ZTest::None, false,
                                                      true, true, boo::CullMode::None);

    m_indTexZWrite = ctx.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_INDTEX, nullptr, nullptr, m_vtxFormatIndTex,
                                           boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                           boo::Primitive::TriStrips, boo::ZTest::None, true, true, false, boo::CullMode::None);
    m_indTexNoZWrite = ctx.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_INDTEX, nullptr, nullptr, m_vtxFormatIndTex,
                                             boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                             boo::Primitive::TriStrips, boo::ZTest::None, false, true, false, boo::CullMode::None);
    m_indTexAdditive = ctx.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_INDTEX, nullptr, nullptr, m_vtxFormatIndTex,
                                             boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                             boo::Primitive::TriStrips, boo::ZTest::None, true, true, false, boo::CullMode::None);

    m_cindTexZWrite = ctx.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_CINDTEX, nullptr, nullptr, m_vtxFormatIndTex,
                                            boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                            boo::Primitive::TriStrips, boo::ZTest::None, true, true, false, boo::CullMode::None);
    m_cindTexNoZWrite = ctx.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_CINDTEX, nullptr, nullptr, m_vtxFormatIndTex,
                                              boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                              boo::Primitive::TriStrips, boo::ZTest::None, false, true, false, boo::CullMode::None);
    m_cindTexAdditive = ctx.newShaderPipeline(VS_METAL_INDTEX, FS_METAL_CINDTEX, nullptr, nullptr, m_vtxFormatIndTex,
                                              boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                              boo::Primitive::TriStrips, boo::ZTest::None, true, true, false, boo::CullMode::None);

    m_noTexZTestZWrite = ctx.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, nullptr, nullptr, m_vtxFormatNoTex,
                                               boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                               boo::Primitive::TriStrips, boo::ZTest::LEqual, true, true, false, boo::CullMode::None);
    m_noTexNoZTestZWrite = ctx.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, nullptr, nullptr, m_vtxFormatNoTex,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                 boo::Primitive::TriStrips, boo::ZTest::None, true, true, false, boo::CullMode::None);
    m_noTexZTestNoZWrite = ctx.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, nullptr, nullptr, m_vtxFormatNoTex,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                 boo::Primitive::TriStrips, boo::ZTest::LEqual, false, true, false, boo::CullMode::None);
    m_noTexNoZTestNoZWrite = ctx.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, nullptr, nullptr, m_vtxFormatNoTex,
                                                   boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                   boo::Primitive::TriStrips, boo::ZTest::None, false, true, false, boo::CullMode::None);

    m_noTexAdditiveZTest = ctx.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, nullptr, nullptr, m_vtxFormatNoTex,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                                 boo::Primitive::TriStrips, boo::ZTest::LEqual, false, true, false, boo::CullMode::None);
    m_noTexAdditiveNoZTest = ctx.newShaderPipeline(VS_METAL_NOTEX, FS_METAL_NOTEX, nullptr, nullptr, m_vtxFormatNoTex,
                                                   boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                                   boo::Primitive::TriStrips, boo::ZTest::None, false, true, false, boo::CullMode::None);

    return new struct MetalElementDataBindingFactory;
}

template <>
void CElementGenShaders::Shutdown<boo::MetalDataFactory>()
{
    m_texZTestZWrite.reset();
    m_texNoZTestZWrite.reset();
    m_texZTestNoZWrite.reset();
    m_texNoZTestNoZWrite.reset();
    m_texAdditiveZTest.reset();
    m_texAdditiveNoZTest.reset();
    m_texRedToAlphaZTest.reset();
    m_texRedToAlphaNoZTest.reset();
    m_texZTestNoZWriteSub.reset();
    m_texNoZTestNoZWriteSub.reset();
    m_texRedToAlphaZTestSub.reset();
    m_texRedToAlphaNoZTestSub.reset();

    m_indTexZWrite.reset();
    m_indTexNoZWrite.reset();
    m_indTexAdditive.reset();

    m_cindTexZWrite.reset();
    m_cindTexNoZWrite.reset();
    m_cindTexAdditive.reset();

    m_noTexZTestZWrite.reset();
    m_noTexNoZTestZWrite.reset();
    m_noTexZTestNoZWrite.reset();
    m_noTexNoZTestNoZWrite.reset();
    m_noTexAdditiveZTest.reset();
    m_noTexAdditiveNoZTest.reset();

    m_vtxFormatTex.reset();
    m_vtxFormatIndTex.reset();
    m_vtxFormatNoTex.reset();
}

}
